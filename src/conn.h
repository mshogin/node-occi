/*!
 * Copyright by Shogin Michael
 * See contributors list in README
 *
 * See license text in LICENSE file
 */


#pragma once
#ifndef H_NODE_OCCI_CONN
#define H_NODE_OCCI_CONN

#include <v8.h>
#include <node.h>
#include <node_events.h>
#include "occi.h"

using namespace v8;
using namespace node;
using namespace oracle::occi;

class conn : public node::EventEmitter {

public:
    static Persistent<FunctionTemplate> persistent_function_template;

    static void Init(Handle<Object> target);

    static Handle<Value> Prepare(const Arguments& args);
    static Handle<Value> Connect(const Arguments& args);

    ~conn();

protected:

    conn();
    static Handle<Value> New(const Arguments& args);

private:
	Connection* mConnection;
	Environment* mEnvironment;
};

#endif
