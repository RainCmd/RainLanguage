#include "KeyWords.h"

static StringAgency KeyWordPool = StringAgency(512);

const String StringKeyWord_namespace = KeyWordPool.Add(TEXT("namespace"));
const String StringKeyWord_import = KeyWordPool.Add(TEXT("import"));
const String StringKeyWord_native = KeyWordPool.Add(TEXT("native"));
const String StringKeyWord_public = KeyWordPool.Add(TEXT("public"));
const String StringKeyWord_internal = KeyWordPool.Add(TEXT("internal"));
const String StringKeyWord_space = KeyWordPool.Add(TEXT("space"));
const String StringKeyWord_protected = KeyWordPool.Add(TEXT("protected"));
const String StringKeyWord_private = KeyWordPool.Add(TEXT("private"));
const String StringKeyWord_enum = KeyWordPool.Add(TEXT("enum"));
const String StringKeyWord_struct = KeyWordPool.Add(TEXT("struct"));
const String StringKeyWord_class = KeyWordPool.Add(TEXT("class"));
const String StringKeyWord_interface = KeyWordPool.Add(TEXT("interface"));
const String StringKeyWord_const = KeyWordPool.Add(TEXT("const"));

const String StringKeyWord_kernel = KeyWordPool.Add(TEXT("kernel"));
const String StringKeyWord_global = KeyWordPool.Add(TEXT("global"));
const String StringKeyWord_base = KeyWordPool.Add(TEXT("base"));
const String StringKeyWord_this = KeyWordPool.Add(TEXT("this"));
const String StringKeyWord_true = KeyWordPool.Add(TEXT("true"));
const String StringKeyWord_false = KeyWordPool.Add(TEXT("false"));
const String StringKeyWord_null = KeyWordPool.Add(TEXT("null"));
const String StringKeyWord_var = KeyWordPool.Add(TEXT("var"));
const String StringKeyWord_bool = KeyWordPool.Add(TEXT("bool"));
const String StringKeyWord_byte = KeyWordPool.Add(TEXT("byte"));
const String StringKeyWord_char = KeyWordPool.Add(TEXT("char"));
const String StringKeyWord_integer = KeyWordPool.Add(TEXT("integer"));
const String StringKeyWord_real = KeyWordPool.Add(TEXT("real"));
const String StringKeyWord_real2 = KeyWordPool.Add(TEXT("real2"));
const String StringKeyWord_real3 = KeyWordPool.Add(TEXT("real3"));
const String StringKeyWord_real4 = KeyWordPool.Add(TEXT("real4"));
const String StringKeyWord_type = KeyWordPool.Add(TEXT("type"));
const String StringKeyWord_string = KeyWordPool.Add(TEXT("string"));
const String StringKeyWord_handle = KeyWordPool.Add(TEXT("handle"));
const String StringKeyWord_entity = KeyWordPool.Add(TEXT("entity"));
const String StringKeyWord_delegate = KeyWordPool.Add(TEXT("delegate"));
const String StringKeyWord_coroutine = KeyWordPool.Add(TEXT("coroutine"));
const String StringKeyWord_array = KeyWordPool.Add(TEXT("array"));

const String StringKeyWord_if = KeyWordPool.Add(TEXT("if"));
const String StringKeyWord_elif = KeyWordPool.Add(TEXT("elif"));
const String StringKeyWord_else = KeyWordPool.Add(TEXT("else"));
const String StringKeyWord_while = KeyWordPool.Add(TEXT("while"));
const String StringKeyWord_for = KeyWordPool.Add(TEXT("for"));
const String StringKeyWord_break = KeyWordPool.Add(TEXT("break"));
const String StringKeyWord_continue = KeyWordPool.Add(TEXT("continue"));
const String StringKeyWord_return = KeyWordPool.Add(TEXT("return"));
const String StringKeyWord_is = KeyWordPool.Add(TEXT("is"));
const String StringKeyWord_as = KeyWordPool.Add(TEXT("as"));
const String StringKeyWord_start = KeyWordPool.Add(TEXT("start"));
const String StringKeyWord_new = KeyWordPool.Add(TEXT("new"));
const String StringKeyWord_wait = KeyWordPool.Add(TEXT("wait"));
const String StringKeyWord_exit = KeyWordPool.Add(TEXT("exit"));
const String StringKeyWord_try = KeyWordPool.Add(TEXT("try"));
const String StringKeyWord_catch = KeyWordPool.Add(TEXT("catch"));
const String StringKeyWord_finally = KeyWordPool.Add(TEXT("finally"));

bool IsKeyWord(const String& value)
{
	if (value == StringKeyWord_namespace)return true;
	if (value == StringKeyWord_import)return true;
	if (value == StringKeyWord_native)return true;
	if (value == StringKeyWord_public)return true;
	if (value == StringKeyWord_internal)return true;
	if (value == StringKeyWord_space)return true;
	if (value == StringKeyWord_protected)return true;
	if (value == StringKeyWord_private)return true;
	if (value == StringKeyWord_enum)return true;
	if (value == StringKeyWord_struct)return true;
	if (value == StringKeyWord_class)return true;
	if (value == StringKeyWord_interface)return true;
	if (value == StringKeyWord_const)return true;

	if (value == StringKeyWord_kernel)return true;
	if (value == StringKeyWord_global)return true;
	if (value == StringKeyWord_base)return true;
	if (value == StringKeyWord_this)return true;
	if (value == StringKeyWord_true)return true;
	if (value == StringKeyWord_false)return true;
	if (value == StringKeyWord_null)return true;
	if (value == StringKeyWord_var)return true;
	if (value == StringKeyWord_bool)return true;
	if (value == StringKeyWord_byte)return true;
	if (value == StringKeyWord_char)return true;
	if (value == StringKeyWord_integer)return true;
	if (value == StringKeyWord_real)return true;
	if (value == StringKeyWord_real2)return true;
	if (value == StringKeyWord_real3)return true;
	if (value == StringKeyWord_real4)return true;
	if (value == StringKeyWord_type)return true;
	if (value == StringKeyWord_string)return true;
	if (value == StringKeyWord_handle)return true;
	if (value == StringKeyWord_entity)return true;
	if (value == StringKeyWord_delegate)return true;
	if (value == StringKeyWord_coroutine)return true;
	if (value == StringKeyWord_array)return true;

	if (value == StringKeyWord_if)return true;
	if (value == StringKeyWord_elif)return true;
	if (value == StringKeyWord_else)return true;
	if (value == StringKeyWord_while)return true;
	if (value == StringKeyWord_for)return true;
	if (value == StringKeyWord_break)return true;
	if (value == StringKeyWord_continue)return true;
	if (value == StringKeyWord_return)return true;
	if (value == StringKeyWord_is)return true;
	if (value == StringKeyWord_as)return true;
	if (value == StringKeyWord_start)return true;
	if (value == StringKeyWord_new)return true;
	if (value == StringKeyWord_wait)return true;
	if (value == StringKeyWord_exit)return true;
	if (value == StringKeyWord_try)return true;
	if (value == StringKeyWord_catch)return true;
	if (value == StringKeyWord_finally)return true;
	return false;
}

String KeyWord_namespace() { return StringKeyWord_namespace; }
String KeyWord_import() { return StringKeyWord_import; }
String KeyWord_native() { return StringKeyWord_native; }
String KeyWord_public() { return StringKeyWord_public; }
String KeyWord_internal() { return StringKeyWord_internal; }
String KeyWord_space() { return StringKeyWord_space; }
String KeyWord_protected() { return StringKeyWord_protected; }
String KeyWord_private() { return StringKeyWord_private; }
String KeyWord_enum() { return StringKeyWord_enum; }
String KeyWord_struct() { return StringKeyWord_struct; }
String KeyWord_class() { return StringKeyWord_class; }
String KeyWord_interface() { return StringKeyWord_interface; }
String KeyWord_const() { return StringKeyWord_const; }
String KeyWord_kernel() { return StringKeyWord_kernel; }
String KeyWord_global() { return StringKeyWord_global; }
String KeyWord_base() { return StringKeyWord_base; }
String KeyWord_this() { return StringKeyWord_this; }
String KeyWord_true() { return StringKeyWord_true; }
String KeyWord_false() { return StringKeyWord_false; }
String KeyWord_null() { return StringKeyWord_null; }
String KeyWord_var() { return StringKeyWord_var; }
String KeyWord_bool() { return StringKeyWord_bool; }
String KeyWord_byte() { return StringKeyWord_byte; }
String KeyWord_char() { return StringKeyWord_char; }
String KeyWord_integer() { return StringKeyWord_integer; }
String KeyWord_real() { return StringKeyWord_real; }
String KeyWord_real2() { return StringKeyWord_real2; }
String KeyWord_real3() { return StringKeyWord_real3; }
String KeyWord_real4() { return StringKeyWord_real4; }
String KeyWord_type() { return StringKeyWord_type; }
String KeyWord_string() { return StringKeyWord_string; }
String KeyWord_handle() { return StringKeyWord_handle; }
String KeyWord_entity() { return StringKeyWord_entity; }
String KeyWord_delegate() { return StringKeyWord_delegate; }
String KeyWord_coroutine() { return StringKeyWord_coroutine; }
String KeyWord_array() { return StringKeyWord_array; }
String KeyWord_if() { return StringKeyWord_if; }
String KeyWord_elif() { return StringKeyWord_elif; }
String KeyWord_else() { return StringKeyWord_else; }
String KeyWord_while() { return StringKeyWord_while; }
String KeyWord_for() { return StringKeyWord_for; }
String KeyWord_break() { return StringKeyWord_break; }
String KeyWord_continue() { return StringKeyWord_continue; }
String KeyWord_return() { return StringKeyWord_return; }
String KeyWord_is() { return StringKeyWord_is; }
String KeyWord_as() { return StringKeyWord_as; }
String KeyWord_start() { return StringKeyWord_start; }
String KeyWord_new() { return StringKeyWord_new; }
String KeyWord_wait() { return StringKeyWord_wait; }
String KeyWord_exit() { return StringKeyWord_exit; }
String KeyWord_try() { return StringKeyWord_try; }
String KeyWord_catch() { return StringKeyWord_catch; }
String KeyWord_finally() { return StringKeyWord_finally; }
