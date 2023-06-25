#include "KeyWords.h"

StringAgency* KeyWordPool = NULL;
void InitKeyWord()
{
	if (KeyWordPool)return;
	KeyWordPool = new StringAgency(512);
	KeyWord_namespace = KeyWordPool->Add(TEXT("namespace"));
	KeyWord_import = KeyWordPool->Add(TEXT("import"));
	KeyWord_native = KeyWordPool->Add(TEXT("native"));
	KeyWord_public = KeyWordPool->Add(TEXT("public"));
	KeyWord_internal = KeyWordPool->Add(TEXT("internal"));
	KeyWord_space = KeyWordPool->Add(TEXT("space"));
	KeyWord_protected = KeyWordPool->Add(TEXT("protected"));
	KeyWord_private = KeyWordPool->Add(TEXT("private"));
	KeyWord_enum = KeyWordPool->Add(TEXT("enum"));
	KeyWord_struct = KeyWordPool->Add(TEXT("struct"));
	KeyWord_class = KeyWordPool->Add(TEXT("class"));
	KeyWord_interface = KeyWordPool->Add(TEXT("interface"));
	KeyWord_const = KeyWordPool->Add(TEXT("const"));

	KeyWord_kernel = KeyWordPool->Add(TEXT("kernel"));
	KeyWord_global = KeyWordPool->Add(TEXT("global"));
	KeyWord_base = KeyWordPool->Add(TEXT("base"));
	KeyWord_this = KeyWordPool->Add(TEXT("this"));
	KeyWord_true = KeyWordPool->Add(TEXT("true"));
	KeyWord_false = KeyWordPool->Add(TEXT("false"));
	KeyWord_null = KeyWordPool->Add(TEXT("null"));
	KeyWord_var = KeyWordPool->Add(TEXT("var"));
	KeyWord_bool = KeyWordPool->Add(TEXT("bool"));
	KeyWord_byte = KeyWordPool->Add(TEXT("byte"));
	KeyWord_char = KeyWordPool->Add(TEXT("char"));
	KeyWord_integer = KeyWordPool->Add(TEXT("int"));
	KeyWord_real = KeyWordPool->Add(TEXT("real"));
	KeyWord_real2 = KeyWordPool->Add(TEXT("real2"));
	KeyWord_real3 = KeyWordPool->Add(TEXT("real3"));
	KeyWord_real4 = KeyWordPool->Add(TEXT("real4"));
	KeyWord_type = KeyWordPool->Add(TEXT("type"));
	KeyWord_string = KeyWordPool->Add(TEXT("string"));
	KeyWord_handle = KeyWordPool->Add(TEXT("handle"));
	KeyWord_entity = KeyWordPool->Add(TEXT("entity"));
	KeyWord_delegate = KeyWordPool->Add(TEXT("delegate"));
	KeyWord_coroutine = KeyWordPool->Add(TEXT("coroutine"));
	KeyWord_array = KeyWordPool->Add(TEXT("array"));

	KeyWord_if = KeyWordPool->Add(TEXT("if"));
	KeyWord_elif = KeyWordPool->Add(TEXT("elif"));
	KeyWord_else = KeyWordPool->Add(TEXT("else"));
	KeyWord_while = KeyWordPool->Add(TEXT("while"));
	KeyWord_for = KeyWordPool->Add(TEXT("for"));
	KeyWord_break = KeyWordPool->Add(TEXT("break"));
	KeyWord_continue = KeyWordPool->Add(TEXT("continue"));
	KeyWord_return = KeyWordPool->Add(TEXT("return"));
	KeyWord_is = KeyWordPool->Add(TEXT("is"));
	KeyWord_as = KeyWordPool->Add(TEXT("as"));
	KeyWord_start = KeyWordPool->Add(TEXT("start"));
	KeyWord_new = KeyWordPool->Add(TEXT("new"));
	KeyWord_wait = KeyWordPool->Add(TEXT("wait"));
	KeyWord_exit = KeyWordPool->Add(TEXT("exit"));
	KeyWord_try = KeyWordPool->Add(TEXT("try"));
	KeyWord_catch = KeyWordPool->Add(TEXT("catch"));
	KeyWord_finally = KeyWordPool->Add(TEXT("finally"));
}

bool IsKeyWord(const String& value)
{
	if (value == KeyWord_namespace)return true;
	if (value == KeyWord_import)return true;
	if (value == KeyWord_native)return true;
	if (value == KeyWord_public)return true;
	if (value == KeyWord_internal)return true;
	if (value == KeyWord_space)return true;
	if (value == KeyWord_protected)return true;
	if (value == KeyWord_private)return true;
	if (value == KeyWord_enum)return true;
	if (value == KeyWord_struct)return true;
	if (value == KeyWord_class)return true;
	if (value == KeyWord_interface)return true;
	if (value == KeyWord_const)return true;

	if (value == KeyWord_kernel)return true;
	if (value == KeyWord_global)return true;
	if (value == KeyWord_base)return true;
	if (value == KeyWord_this)return true;
	if (value == KeyWord_true)return true;
	if (value == KeyWord_false)return true;
	if (value == KeyWord_null)return true;
	if (value == KeyWord_var)return true;
	if (value == KeyWord_bool)return true;
	if (value == KeyWord_byte)return true;
	if (value == KeyWord_char)return true;
	if (value == KeyWord_integer)return true;
	if (value == KeyWord_real)return true;
	if (value == KeyWord_real2)return true;
	if (value == KeyWord_real3)return true;
	if (value == KeyWord_real4)return true;
	if (value == KeyWord_type)return true;
	if (value == KeyWord_string)return true;
	if (value == KeyWord_handle)return true;
	if (value == KeyWord_entity)return true;
	if (value == KeyWord_delegate)return true;
	if (value == KeyWord_coroutine)return true;
	if (value == KeyWord_array)return true;

	if (value == KeyWord_if)return true;
	if (value == KeyWord_elif)return true;
	if (value == KeyWord_else)return true;
	if (value == KeyWord_while)return true;
	if (value == KeyWord_for)return true;
	if (value == KeyWord_break)return true;
	if (value == KeyWord_continue)return true;
	if (value == KeyWord_return)return true;
	if (value == KeyWord_is)return true;
	if (value == KeyWord_as)return true;
	if (value == KeyWord_start)return true;
	if (value == KeyWord_new)return true;
	if (value == KeyWord_wait)return true;
	if (value == KeyWord_exit)return true;
	if (value == KeyWord_try)return true;
	if (value == KeyWord_catch)return true;
	if (value == KeyWord_finally)return true;
	return false;
}
