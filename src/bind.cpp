/*!
 * Copyright by Shogin Michael
 * See contributors list in README
 */

#include <v8.h>
#include "bind.h"
#include "conn.h"
#include "st.h"
#include "rs.h"

std::string TSTDS(const v8::Local<v8::Value>& value) {
	v8::String::Utf8Value val(value);

	std::string stdstring(*val);

	return stdstring;
}


extern "C" void init(v8::Handle<v8::Object> target) {
    conn::Init(target);
    st::Init(target);
    rs::Init(target);
}
