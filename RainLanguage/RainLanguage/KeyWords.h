﻿#pragma once
#include "Language.h"
#include "String.h"

String KeyWord_namespace();
String KeyWord_import();
String KeyWord_native();
String KeyWord_public();
String KeyWord_internal();
String KeyWord_space();
String KeyWord_protected();
String KeyWord_private();
String KeyWord_enum();
String KeyWord_struct();
String KeyWord_class();
String KeyWord_interface();
String KeyWord_const();

String KeyWord_global();
String KeyWord_base();
String KeyWord_this();
String KeyWord_true();
String KeyWord_false();
String KeyWord_null();
String KeyWord_var();
String KeyWord_bool();
String KeyWord_byte();
String KeyWord_char();
String KeyWord_integer();
String KeyWord_real();
String KeyWord_real2();
String KeyWord_real3();
String KeyWord_real4();
String KeyWord_type();
String KeyWord_string();
String KeyWord_handle();
String KeyWord_entity();
String KeyWord_delegate();
String KeyWord_task();
String KeyWord_array();

String KeyWord_if();
String KeyWord_elseif();
String KeyWord_else();
String KeyWord_while();
String KeyWord_for();
String KeyWord_break();
String KeyWord_continue();
String KeyWord_return();
String KeyWord_is();
String KeyWord_as();
String KeyWord_and();
String KeyWord_or();
String KeyWord_start();
String KeyWord_new();
String KeyWord_wait();
String KeyWord_exit();
String KeyWord_try();
String KeyWord_catch();
String KeyWord_finally();

bool IsKeyWord(const String& value);

String DiscardVariable();
String ClosureName();
