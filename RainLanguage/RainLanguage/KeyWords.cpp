#include "KeyWords.h"
#include "ClearStaticCache.h"

static StringAgency* KeyWordPool = NULL;

//KernelLibraryInfo.cpp 中不少地方是直接写死的字符串，如果要改的话注意两边同步
#define KEY_WORLD(name) String KeyWord_##name() { if (!KeyWordPool)KeyWordPool = new StringAgency(0xff); return KeyWordPool->Add(TEXT(#name)); }
KEY_WORLD(namespace)
KEY_WORLD(import)
KEY_WORLD(native)
KEY_WORLD(public)
KEY_WORLD(internal)
KEY_WORLD(space)
KEY_WORLD(protected)
KEY_WORLD(private)
KEY_WORLD(enum)
KEY_WORLD(struct)
KEY_WORLD(class)
KEY_WORLD(interface)
KEY_WORLD(const)

KEY_WORLD(global)
KEY_WORLD(base)
KEY_WORLD(this)
KEY_WORLD(true)
KEY_WORLD(false)
KEY_WORLD(null)
KEY_WORLD(var)
KEY_WORLD(bool)
KEY_WORLD(byte)
KEY_WORLD(char)
KEY_WORLD(integer)
KEY_WORLD(real)
KEY_WORLD(real2)
KEY_WORLD(real3)
KEY_WORLD(real4)
KEY_WORLD(type)
KEY_WORLD(string)
KEY_WORLD(handle)
KEY_WORLD(entity)
KEY_WORLD(delegate)
KEY_WORLD(task)
KEY_WORLD(array)

KEY_WORLD(if)
KEY_WORLD(elseif)
KEY_WORLD(else)
KEY_WORLD(while)
KEY_WORLD(for)
KEY_WORLD(break)
KEY_WORLD(continue)
KEY_WORLD(return)
KEY_WORLD(is)
KEY_WORLD(as)
KEY_WORLD(and)
KEY_WORLD(or)
KEY_WORLD(start)
KEY_WORLD(new)
KEY_WORLD(wait)
KEY_WORLD(exit)
KEY_WORLD(try)
KEY_WORLD(catch)
KEY_WORLD(finally)

bool IsKeyWord(const String & value)
{
	if (value == KeyWord_namespace()) return true;
	if (value == KeyWord_import()) return true;
	if (value == KeyWord_native()) return true;
	if (value == KeyWord_public()) return true;
	if (value == KeyWord_internal()) return true;
	if (value == KeyWord_space()) return true;
	if (value == KeyWord_protected()) return true;
	if (value == KeyWord_private()) return true;
	if (value == KeyWord_enum()) return true;
	if (value == KeyWord_struct()) return true;
	if (value == KeyWord_class()) return true;
	if (value == KeyWord_interface()) return true;
	if (value == KeyWord_const()) return true;

	if (value == KeyWord_global()) return true;
	if (value == KeyWord_base()) return true;
	if (value == KeyWord_this()) return true;
	if (value == KeyWord_true()) return true;
	if (value == KeyWord_false()) return true;
	if (value == KeyWord_null()) return true;
	if (value == KeyWord_var()) return true;
	if (value == KeyWord_bool()) return true;
	if (value == KeyWord_byte()) return true;
	if (value == KeyWord_char()) return true;
	if (value == KeyWord_integer()) return true;
	if (value == KeyWord_real()) return true;
	if (value == KeyWord_real2()) return true;
	if (value == KeyWord_real3()) return true;
	if (value == KeyWord_real4()) return true;
	if (value == KeyWord_type()) return true;
	if (value == KeyWord_string()) return true;
	if (value == KeyWord_handle()) return true;
	if (value == KeyWord_entity()) return true;
	if (value == KeyWord_delegate()) return true;
	if (value == KeyWord_task()) return true;
	if (value == KeyWord_array()) return true;

	if (value == KeyWord_if()) return true;
	if (value == KeyWord_elseif()) return true;
	if (value == KeyWord_else()) return true;
	if (value == KeyWord_while()) return true;
	if (value == KeyWord_for()) return true;
	if (value == KeyWord_break()) return true;
	if (value == KeyWord_continue()) return true;
	if (value == KeyWord_return()) return true;
	if (value == KeyWord_is()) return true;
	if (value == KeyWord_as()) return true;
	if (value == KeyWord_and()) return true;
	if (value == KeyWord_or()) return true;
	if (value == KeyWord_start()) return true;
	if (value == KeyWord_new()) return true;
	if (value == KeyWord_wait()) return true;
	if (value == KeyWord_exit()) return true;
	if (value == KeyWord_try()) return true;
	if (value == KeyWord_catch()) return true;
	if (value == KeyWord_finally()) return true;
	return false;
}

void ClearKeyWorlds()
{
	delete KeyWordPool;
	KeyWordPool = NULL;
}

String DiscardVariable()
{
	if(!KeyWordPool) KeyWordPool = new StringAgency(0xff); 
	return KeyWordPool->Add(TEXT("_"));
}