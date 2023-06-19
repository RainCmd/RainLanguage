#include "KeyWords.h"

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
