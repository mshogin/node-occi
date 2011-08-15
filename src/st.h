/*!
 * Copyright by Shogin Michael
 * See contributors list in README
 */

#pragma once
#ifndef H_NODE_OCCI_ST
#define H_NODE_OCCI_ST


#include <v8.h>
#include <node.h>
#include <node_events.h>
#include "occi.h"

using namespace v8;
using namespace oracle::occi;

class st : public node::EventEmitter {

public:
	Statement *_stmt;

    st();
    ~st();
	static Persistent<FunctionTemplate> persistent_function_template;

    static void Init(Handle<Object> target);

    static Handle<Value> Bind_Param(const v8::Arguments& args);

    static Handle<Value> Bind_Param_InOut(const v8::Arguments& args);

    static Handle<Value> Execute(const v8::Arguments& args);

    static Handle<Value> Get_Cursor(const v8::Arguments& args);

    static Handle<Value> Get_Clob(const v8::Arguments& args);

    static Handle<Value> Get_String(const v8::Arguments& args);

protected:

    static Handle<Value> New(const Arguments& args);

};

#endif  // SRC_ORACLE_ST_H_
