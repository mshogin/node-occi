/*!
 * Copyright by Shogin Michael
 * See contributors list in README
 */

#include <v8.h>
#include "occi.h"
#include "bind.h"
#include "st.h"
#include "rs.h"
#include <iostream>

using namespace v8;
using namespace oracle::occi;

Persistent<FunctionTemplate> st::persistent_function_template;

Handle<Value> st::New(const Arguments& args) {
	HandleScope scope;

    st* _instance = new st();

    Local<External> VAR = Local<External>::Cast(args[0]);
    Statement* statement = static_cast<Statement*>(VAR->Value());

    _instance->_stmt = statement;

    _instance->Wrap(args.This());

    return args.This();
}

void st::Init(Handle<Object> target) {
	HandleScope scope;

    Local<FunctionTemplate> local_function_template = FunctionTemplate::New(New);

    persistent_function_template = Persistent<FunctionTemplate>::New(local_function_template);
    persistent_function_template->InstanceTemplate()->SetInternalFieldCount(1);
    persistent_function_template->SetClassName(String::NewSymbol("st"));

    Local<ObjectTemplate> instance_template = persistent_function_template->InstanceTemplate();

    NODE_SET_PROTOTYPE_METHOD(local_function_template, "bind_param", Bind_Param);
    NODE_SET_PROTOTYPE_METHOD(local_function_template, "bind_param_inout", Bind_Param_InOut);
    NODE_SET_PROTOTYPE_METHOD(local_function_template, "execute", Execute);
    NODE_SET_PROTOTYPE_METHOD(local_function_template, "get_cursor", Get_Cursor);
    NODE_SET_PROTOTYPE_METHOD(local_function_template, "get_clob", Get_Clob);
    NODE_SET_PROTOTYPE_METHOD(local_function_template, "get_string", Get_String);

    target->Set(String::NewSymbol("st"), persistent_function_template->GetFunction());
}

Handle<Value> st::Execute(const Arguments& args) {
	HandleScope scope;
    st* _this = node::ObjectWrap::Unwrap<st>(args.This());

    _this->_stmt->execute();

    return scope.Close(True());
}

Handle<Value> st::Bind_Param(const Arguments& args) {
    HandleScope scope;
    st* _this = node::ObjectWrap::Unwrap<st>(args.This());

    int32_t pNum = args[0]->Int32Value();
    int32_t pType = args[1]->Int32Value();

    if ( (oracle::occi::Type)pType == OCCINUMBER ) {
    	int32_t pValue = args[2]->Int32Value();
    	_this->_stmt->setNumber(pNum, pValue );
    }
    else if ( (oracle::occi::Type)pType == OCCISTRING ) {
    	std::string pValue = TSTDS(args[2]);
    	_this->_stmt->setString(pNum, pValue );
    }
    else if ( (oracle::occi::Type)pType == OCCICLOB ) {
    	std::string pValue = TSTDS(args[2]);

    	Statement* stmtCreateClob = _this->_stmt->getConnection()->createStatement ( "BEGIN DBMS_LOB.CREATETEMPORARY (:1, FALSE); END;" );
    	stmtCreateClob->registerOutParam(1, oracle::occi::OCCICLOB);
    	stmtCreateClob->execute();

    	oracle::occi::Clob wCLOB = stmtCreateClob->getClob(1);
    	stmtCreateClob->setNull (1, oracle::occi::OCCICLOB);

    	if ( !wCLOB.isOpen () ) wCLOB.open ();
    	wCLOB.write (pValue.length (), (unsigned char *)(pValue.c_str ()), pValue.length () );
    	wCLOB.close ();

    	_this->_stmt->getConnection()->terminateStatement ( stmtCreateClob );
    	stmtCreateClob = 0;

    	_this->_stmt->setClob((unsigned int)pNum, wCLOB );
    }

    return scope.Close(True());
}

Handle<Value> st::Bind_Param_InOut(const Arguments& args) {
    HandleScope scope;
    st* _this = node::ObjectWrap::Unwrap<st>(args.This());

    int32_t pNum = args[0]->Int32Value();
    int32_t pType = args[1]->Int32Value();

    if (pType == OCCISTRING) {
        int32_t pSize = args[2]->Int32Value();
    	_this->_stmt->registerOutParam(pNum, (oracle::occi::Type)pType, pSize );
    }
    else {
    	_this->_stmt->registerOutParam(pNum, (oracle::occi::Type)pType );
    }

    return scope.Close(True());
}

Handle<Value> st::Get_Cursor(const Arguments& args) {
    HandleScope scope;
    st* _this = node::ObjectWrap::Unwrap<st>(args.This());

    int32_t num = args[0]->Int32Value();

    ResultSet* rs = _this->_stmt->getCursor(num);

    Local<Value> arg = External::New(rs);

    Persistent<Object> js_result( rs::persistent_function_template->GetFunction()->NewInstance(1, &arg) );

	return scope.Close(js_result);
}

Handle<Value> st::Get_Clob(const Arguments& args) {
    HandleScope scope;
    st* _this = node::ObjectWrap::Unwrap<st>(args.This());

    int32_t num = args[0]->Int32Value();
    int32_t len = args[1]->Int32Value();

    Clob lClob = _this->_stmt->getClob(num);

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

Handle<Value> st::Get_String(const Arguments& args) {
    HandleScope scope;
    st* _this = node::ObjectWrap::Unwrap<st>(args.This());

    int32_t num = args[0]->Int32Value();

    std::string result = _this->_stmt->getString(num);

    return scope.Close(String::New(result.c_str()));
}

st::st() {

}

st::~st(){

}
