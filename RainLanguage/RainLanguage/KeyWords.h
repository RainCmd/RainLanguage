#pragma once
#include "RainLanguage.h"
#include "String.h"
//KernelLibraryInfo.cpp 中不少地方是直接写死的字符串，如果要改的话注意两边同步
static String KeyWord_namespace;
static String KeyWord_import;
static String KeyWord_native;
static String KeyWord_public;
static String KeyWord_internal;
static String KeyWord_space;
static String KeyWord_protected;
static String KeyWord_private;
static String KeyWord_enum;
static String KeyWord_struct;
static String KeyWord_class;
static String KeyWord_interface;
static String KeyWord_const;

static String KeyWord_kernel;
static String KeyWord_global;
static String KeyWord_base;
static String KeyWord_this;
static String KeyWord_true;
static String KeyWord_false;
static String KeyWord_null;
static String KeyWord_var;
static String KeyWord_bool;
static String KeyWord_byte;
static String KeyWord_char;
static String KeyWord_integer;
static String KeyWord_real;
static String KeyWord_real2;
static String KeyWord_real3;
static String KeyWord_real4;
static String KeyWord_type;
static String KeyWord_string;
static String KeyWord_handle;
static String KeyWord_entity;
static String KeyWord_delegate;
static String KeyWord_coroutine;
static String KeyWord_array;

static String KeyWord_if;
static String KeyWord_elif;
static String KeyWord_else;
static String KeyWord_while;
static String KeyWord_for;
static String KeyWord_break;
static String KeyWord_continue;
static String KeyWord_return;
static String KeyWord_is;
static String KeyWord_as;
static String KeyWord_start;
static String KeyWord_new;
static String KeyWord_wait;
static String KeyWord_exit;
static String KeyWord_try;
static String KeyWord_catch;
static String KeyWord_finally;
void InitKeyWord();
bool IsKeyWord(const String& value);