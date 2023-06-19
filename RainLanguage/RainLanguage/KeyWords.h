#pragma once
#include "RainLanguage.h"
#include "String.h"
//KernelLibraryInfo.cpp 中不少地方是直接写死的字符串，如果要改的话注意两边同步
static StringAgency KeyWordPool = StringAgency(512);
const String KeyWord_namespace = KeyWordPool.Add(TEXT("namespace"));
const String KeyWord_import = KeyWordPool.Add(TEXT("import"));
const String KeyWord_native = KeyWordPool.Add(TEXT("native"));
const String KeyWord_public = KeyWordPool.Add(TEXT("public"));
const String KeyWord_internal = KeyWordPool.Add(TEXT("internal"));
const String KeyWord_space = KeyWordPool.Add(TEXT("space"));
const String KeyWord_protected = KeyWordPool.Add(TEXT("protected"));
const String KeyWord_private = KeyWordPool.Add(TEXT("private"));
const String KeyWord_enum = KeyWordPool.Add(TEXT("enum"));
const String KeyWord_struct = KeyWordPool.Add(TEXT("struct"));
const String KeyWord_class = KeyWordPool.Add(TEXT("class"));
const String KeyWord_interface = KeyWordPool.Add(TEXT("interface"));
const String KeyWord_const = KeyWordPool.Add(TEXT("const"));

const String KeyWord_kernel = KeyWordPool.Add(TEXT("kernel"));
const String KeyWord_global = KeyWordPool.Add(TEXT("global"));
const String KeyWord_base = KeyWordPool.Add(TEXT("base"));
const String KeyWord_this = KeyWordPool.Add(TEXT("this"));
const String KeyWord_true = KeyWordPool.Add(TEXT("true"));
const String KeyWord_false = KeyWordPool.Add(TEXT("false"));
const String KeyWord_null = KeyWordPool.Add(TEXT("null"));
const String KeyWord_var = KeyWordPool.Add(TEXT("var"));
const String KeyWord_bool = KeyWordPool.Add(TEXT("bool"));
const String KeyWord_byte = KeyWordPool.Add(TEXT("byte"));
const String KeyWord_char = KeyWordPool.Add(TEXT("char"));
const String KeyWord_integer = KeyWordPool.Add(TEXT("int"));
const String KeyWord_real = KeyWordPool.Add(TEXT("real"));
const String KeyWord_real2 = KeyWordPool.Add(TEXT("real2"));
const String KeyWord_real3 = KeyWordPool.Add(TEXT("real3"));
const String KeyWord_real4 = KeyWordPool.Add(TEXT("real4"));
const String KeyWord_type = KeyWordPool.Add(TEXT("type"));
const String KeyWord_string = KeyWordPool.Add(TEXT("string"));
const String KeyWord_handle = KeyWordPool.Add(TEXT("handle"));
const String KeyWord_entity = KeyWordPool.Add(TEXT("entity"));
const String KeyWord_delegate = KeyWordPool.Add(TEXT("delegate"));
const String KeyWord_coroutine = KeyWordPool.Add(TEXT("coroutine"));
const String KeyWord_array = KeyWordPool.Add(TEXT("array"));

const String KeyWord_if = KeyWordPool.Add(TEXT("if"));
const String KeyWord_elif = KeyWordPool.Add(TEXT("elif"));
const String KeyWord_else = KeyWordPool.Add(TEXT("else"));
const String KeyWord_while = KeyWordPool.Add(TEXT("while"));
const String KeyWord_for = KeyWordPool.Add(TEXT("for"));
const String KeyWord_break = KeyWordPool.Add(TEXT("break"));
const String KeyWord_continue = KeyWordPool.Add(TEXT("continue"));
const String KeyWord_return = KeyWordPool.Add(TEXT("return"));
const String KeyWord_is = KeyWordPool.Add(TEXT("is"));
const String KeyWord_as = KeyWordPool.Add(TEXT("as"));
const String KeyWord_start = KeyWordPool.Add(TEXT("start"));
const String KeyWord_new = KeyWordPool.Add(TEXT("new"));
const String KeyWord_wait = KeyWordPool.Add(TEXT("wait"));
const String KeyWord_exit = KeyWordPool.Add(TEXT("exit"));
const String KeyWord_try = KeyWordPool.Add(TEXT("try"));
const String KeyWord_catch = KeyWordPool.Add(TEXT("catch"));
const String KeyWord_finally = KeyWordPool.Add(TEXT("finally"));

bool IsKeyWord(const String& value);