/*!
 * Copyright by Shogin Michael
 * See contributors list in README
 */

#pragma once
#ifndef H_NODE_OCCI_RS
#define H_NODE_OCCI_RS


#include <v8.h>
#include <node.h>
#include <node_events.h>
#include "occi.h"

using namespace v8;
using namespace oracle::occi;

#define V8STR(str) String::New(str)

class rs : public node::EventEmitter {

public:
	ResultSet *_rs;

    rs();
    ~rs();

	static Persistent<FunctionTemplate> persistent_function_template;

    static void Init(Handle<Object> target);
    static Handle<Value> Next(const v8::Arguments& args);

    static Handle<Value> Get_Int(const v8::Arguments& args);
    static Handle<Value> Get_Double(const v8::Arguments& args);
    static Handle<Value> Get_Clob(const v8::Arguments& args);
    static Handle<Value> Get_String(const v8::Arguments& args);

    static Handle<Value> Fetch_All(const v8::Arguments& args);
    static Handle<Value> Fetch(const v8::Arguments& args);

protected:

    static Handle<Value> New(const Arguments& args);

};

#endif
