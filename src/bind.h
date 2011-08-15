/*!
 * Copyright by Shogin Michael
 * See contributors list in README
 */

#pragma once
#ifndef H_NODE_OCCI_BIND
#define H_NODE_OCCI_BIND

#include <v8.h>
#include <string>

#define V8STR(str) String::New(str)

std::string TSTDS(const v8::Local<v8::Value>& value);

#endif
