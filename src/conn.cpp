/*!
 * Copyright by Shogin Michael
 * See contributors list in README
 */

#include "conn.h"
#include "st.h"
#include "bind.h"

Persistent<FunctionTemplate> conn::persistent_function_template;

void conn::Init(Handle<Object> target) {
	HandleScope scope;

    Local<FunctionTemplate> local_function_template = FunctionTemplate::New(New);

    persistent_function_template = Persistent<FunctionTemplate>::New(local_function_template);
    persistent_function_template->InstanceTemplate()->SetInternalFieldCount(1); // 1 since this is a constructor function
    persistent_function_template->SetClassName(String::NewSymbol("conn"));

    NODE_SET_PROTOTYPE_METHOD(conn::persistent_function_template, "prepare", Prepare);
    NODE_SET_PROTOTYPE_METHOD(conn::persistent_function_template, "connect", Connect);

    target->Set(String::NewSymbol("conn"), conn::persistent_function_template->GetFunction());
}

Handle<Value> conn::New(const Arguments& args) {
	HandleScope scope;

	conn* lInstance = new conn();

	lInstance->Wrap(args.This());

    return args.This();
}

Handle<Value> conn::Connect(const Arguments& args){
	HandleScope scope;
	conn* lInstance = node::ObjectWrap::Unwrap<conn>(args.This());

	std::string schema = TSTDS(args[0]);
	std::string password = TSTDS(args[1]);
	std::string connection_string = TSTDS(args[2]);
	std::string client_charset = TSTDS(args[3]);
	std::string client_national_charset = TSTDS(args[4]);

	lInstance->mEnvironment = Environment::createEnvironment(client_charset,client_national_charset);
	lInstance->mConnection = lInstance->mEnvironment->createConnection(schema, password, connection_string);

    return args.This();
}


Handle<Value> conn::Prepare(const Arguments& args) {
	HandleScope scope;
	conn* lInstance = node::ObjectWrap::Unwrap<conn>(args.This());
      
	String::Utf8Value sql(args[0]);

    Statement* lStatement = NULL;

    try {
    	lStatement = lInstance->mConnection->createStatement(*sql);
    }
    catch(oracle::occi::SQLException &sqlExcp) {
    	throw sqlExcp.getMessage();
    }

    Local<Value> lArg = External::New(lStatement);
    Persistent<Object> lJsResult( st::persistent_function_template->GetFunction()->NewInstance(1, &lArg) );

    return scope.Close(lJsResult);
}

conn::conn(){ }

conn::~conn(){
	if (mEnvironment && mConnection)
		mEnvironment->terminateConnection(mConnection);

	if (mEnvironment)
		Environment::terminateEnvironment(mEnvironment);
}
