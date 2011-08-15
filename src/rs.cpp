/*!
 * Copyright by Shogin Michael
 * See contributors list in README
 */

#include <v8.h>
#include "occi.h"
#include "rs.h"
#include <iostream>
#include <vector>

using namespace v8;
using namespace oracle::occi;

Persistent<FunctionTemplate> rs::persistent_function_template;

Handle<Value> rs::New(const Arguments& args) {

    HandleScope scope;

    rs* _instance = new rs();

    Local<External> VAR = Local<External>::Cast(args[0]);
    ResultSet* rs = static_cast<ResultSet*>(VAR->Value());

    _instance->_rs = rs;

    _instance->Wrap(args.This());

	return args.This();
}

void rs::Init(Handle<Object> target) {
    HandleScope scope;

    Local<FunctionTemplate> local_function_template = FunctionTemplate::New(New);

    persistent_function_template = Persistent<FunctionTemplate>::New(local_function_template);
    persistent_function_template->InstanceTemplate()->SetInternalFieldCount(1);
    persistent_function_template->SetClassName(String::NewSymbol("rs"));

    Local<ObjectTemplate> instance_template = persistent_function_template->InstanceTemplate();

    NODE_SET_PROTOTYPE_METHOD(local_function_template, "next", Next);
    NODE_SET_PROTOTYPE_METHOD(local_function_template, "get_int", Get_Int);
    NODE_SET_PROTOTYPE_METHOD(local_function_template, "get_double", Get_Double);
    NODE_SET_PROTOTYPE_METHOD(local_function_template, "get_clob", Get_Clob);
    NODE_SET_PROTOTYPE_METHOD(local_function_template, "get_string", Get_String);
    NODE_SET_PROTOTYPE_METHOD(local_function_template, "fetch_all", Fetch_All);
    NODE_SET_PROTOTYPE_METHOD(local_function_template, "fetch", Fetch);

    target->Set(String::NewSymbol("rs"), persistent_function_template->GetFunction());
}

Handle<Value> rs::Next(const Arguments& args) {
	HandleScope scope;
    rs* _this = node::ObjectWrap::Unwrap<rs>(args.This());

    if ( _this->_rs->next() ){
    	return scope.Close(True());
    }

	return scope.Close(False());
}

Handle<Value> rs::Get_Int(const Arguments& args) {
    HandleScope scope;
    rs* _this = node::ObjectWrap::Unwrap<rs>(args.This());

    int32_t num = args[0]->Int32Value();

    int32_t result = _this->_rs->getInt(num);

    return scope.Close(Integer::New(result));
}

Handle<Value> rs::Get_Double(const Arguments& args) {
    HandleScope scope;
    rs* _this = node::ObjectWrap::Unwrap<rs>(args.This());

    int32_t num = args[0]->Int32Value();

    double result = _this->_rs->getDouble(num);

    return scope.Close(v8::Number::New(result));
}

Handle<Value> rs::Get_String(const Arguments& args) {
    HandleScope scope;
    rs* _this = node::ObjectWrap::Unwrap<rs>(args.This());

    int32_t num = args[0]->Int32Value();

    std::string result = _this->_rs->getString(num);

    return scope.Close(String::New(result.c_str()));
}

Handle<Value> rs::Get_Clob(const Arguments& args) {
    HandleScope scope;
    rs* _this = node::ObjectWrap::Unwrap<rs>(args.This());

    int32_t num = args[0]->Int32Value();
    int32_t len = args[1]->Int32Value();

    Clob lClob = _this->_rs->getClob(num);

    if (lClob.isNull() ){
        return scope.Close(String::New(""));
    }

	lClob.open(OCCI_LOB_READONLY);

    const unsigned int BUFSIZE = len;
    unsigned char buffer[BUFSIZE];
    unsigned int readAmt=BUFSIZE;
    unsigned int offset=1;

    lClob.read(readAmt,buffer,BUFSIZE,offset);

    lClob.close();

	return scope.Close(String::New((const char*)buffer));
}

Handle<Value> rs::Fetch_All(const v8::Arguments& args){
    HandleScope scope;
    rs* _this = node::ObjectWrap::Unwrap<rs>(args.This());

    std::vector<MetaData> metaData = _this->_rs->getColumnListMetaData();

    int columnCount = metaData.size();

    Local<Array> js_fields = Array::New();

    if ( columnCount > 0 ){

    	int32_t arrayIndex = 0;

    	while ( _this->_rs->next() ){
    		Local<Object> js_field_obj = Object::New();

	    	Local<String> pName;

	    	for ( int i = 0; i < columnCount; i++ ) {
	    		pName = V8STR(metaData[i].getString(MetaData::ATTR_NAME).c_str());

	    		int pNum = i+1;
    	    	switch ( metaData[i].getInt(MetaData::ATTR_DATA_TYPE) ){
    	    		case OCCI_SQLT_AFC :
    	    		case OCCI_SQLT_VCS :
    	    		case OCCI_SQLT_STR :
    	    		case OCCI_SQLT_CHR : {
    	    				std::string pVal = _this->_rs->getString(pNum);
    	    	    	    js_field_obj->Set(
    	    					pName,
    	    					V8STR(pVal.c_str())
    	    				);
    	    			}
    	    	        break;
    	    		case OCCIFLOAT :
    	    		case OCCI_SQLT_NUM : {
    	    				double pVal = _this->_rs->getDouble(pNum);
    	    	    	    js_field_obj->Set(
    	    					pName,
    	    					v8::Number::New(pVal)
    	    				);
    	    			}
    	    	        break;
    	    		case OCCIINT : {
    	    				int32_t pVal = _this->_rs->getInt(pNum);
    	    				js_field_obj->Set(
    	    					pName,
    	    					v8::Integer::NewFromUnsigned(pVal)
    	    	    		);
    	    			}
    	    			break;
    	    		case OCCI_SQLT_CLOB: {
							Clob lClob = _this->_rs->getClob(pNum);
							lClob.setCharSetForm(OCCI_SQLCS_NCHAR);
							if (lClob.isNull() ){
								js_field_obj->Set(
	    	    					pName,
	    	    					String::New("")
	    	    	    		);
							}
							else {
								lClob.open(OCCI_LOB_READONLY);

								const unsigned int BUFSIZE = 100000;
								unsigned char buffer[BUFSIZE];
								unsigned int readAmt=BUFSIZE;
								unsigned int offset=1;

								lClob.read(readAmt,buffer,BUFSIZE,offset);

								lClob.close();

								js_field_obj->Set(
	    	    					pName,
	    	    					String::New((const char*)buffer)
	    	    	    		);
							}
    	    			}
    	    	        break;
    	    	}
    	    }


    	    js_fields->Set(arrayIndex++, js_field_obj);
    	}
    }

    return scope.Close(js_fields);
}

Handle<Value> rs::Fetch(const v8::Arguments& args){
    HandleScope scope;
    rs* _this = node::ObjectWrap::Unwrap<rs>(args.This());

    std::vector<MetaData> metaData = _this->_rs->getColumnListMetaData();

    int columnCount = metaData.size();

    if ( columnCount > 0 ){

    	int32_t arrayIndex = 0;

    	if ( _this->_rs->next() ){
    		Local<Object> js_field_obj = Object::New();

	    	Local<String> pName;

	    	for ( int i = 0; i < columnCount; i++ ) {
	    		pName = V8STR(metaData[i].getString(MetaData::ATTR_NAME).c_str());

	    		int pNum = i+1;
    	    	switch ( metaData[i].getInt(MetaData::ATTR_DATA_TYPE) ){
    	    		case OCCI_SQLT_AFC :
    	    		case OCCI_SQLT_VCS :
    	    		case OCCI_SQLT_STR :
    	    		case OCCI_SQLT_CHR : {
    	    				std::string pVal = _this->_rs->getString(pNum);
    	    	    	    js_field_obj->Set(
    	    					pName,
    	    					V8STR(pVal.c_str())
    	    				);
    	    			}
    	    	        break;
    	    		case OCCIFLOAT :
    	    		case OCCI_SQLT_NUM : {
    	    				double pVal = _this->_rs->getDouble(pNum);
    	    	    	    js_field_obj->Set(
    	    					pName,
    	    					v8::Number::New(pVal)
    	    				);
    	    			}
    	    	        break;
    	    		case OCCIINT : {
    	    				int32_t pVal = _this->_rs->getInt(pNum);
    	    				js_field_obj->Set(
    	    					pName,
    	    					v8::Integer::NewFromUnsigned(pVal)
    	    	    		);
    	    			}
    	    			break;
    	    		case OCCI_SQLT_CLOB: {
							Clob lClob = _this->_rs->getClob(pNum);
							lClob.setCharSetForm(OCCI_SQLCS_NCHAR);
							if (lClob.isNull() ){
								js_field_obj->Set(
	    	    					pName,
	    	    					String::New("")
	    	    	    		);
							}
							else {
								lClob.open(OCCI_LOB_READONLY);

								const unsigned int BUFSIZE = 100000;
								unsigned char buffer[BUFSIZE];
								unsigned int readAmt=BUFSIZE;
								unsigned int offset=1;

								memset(buffer,0,sizeof(buffer));
								lClob.read(readAmt,buffer,BUFSIZE,offset);

								lClob.close();

								js_field_obj->Set(
	    	    					pName,
	    	    					String::New((const char*)buffer)
	    	    	    		);
							}
    	    			}
    	    	        break;
    	    	}
    	    }

	    	return scope.Close(js_field_obj);
    	}
    }

    return scope.Close(False());
}

rs::rs() {

}

rs::~rs(){

}
