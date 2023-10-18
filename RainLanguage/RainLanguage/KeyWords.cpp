#include "KeyWords.h"
#include "ClearStaticCache.h"

static StringAgency* KeyWordPool = NULL;

//KernelLibraryInfo.cpp 中不少地方是直接写死的字符串，如果要改的话注意两边同步
#define KEY_WORLD(name,text) String KeyWord_##name() { if (!KeyWordPool)KeyWordPool = new StringAgency(0xff); return KeyWordPool->Add(TEXT(text)); }
KEY_WORLD(namespace, "namespace")
KEY_WORLD(import, "import")
KEY_WORLD(native, "native")
KEY_WORLD(public, "public")
KEY_WORLD(internal, "internal")
KEY_WORLD(space, "space")
KEY_WORLD(protected, "protected")
KEY_WORLD(private, "private")
KEY_WORLD(enum, "enum")
KEY_WORLD(struct, "struct")
KEY_WORLD(class, "class")
KEY_WORLD(interface, "interface")
KEY_WORLD(const, "const")

KEY_WORLD(global, "global")
KEY_WORLD(base, "base")
KEY_WORLD(this, "this")
KEY_WORLD(true, "true")
KEY_WORLD(false, "false")
KEY_WORLD(null, "null")
KEY_WORLD(var, "var")
KEY_WORLD(bool, "bool")
KEY_WORLD(byte, "byte")
KEY_WORLD(char, "char")
KEY_WORLD(integer, "integer")
KEY_WORLD(real, "real")
KEY_WORLD(real2, "real2")
KEY_WORLD(real3, "real3")
KEY_WORLD(real4, "real4")
KEY_WORLD(type, "type")
KEY_WORLD(string, "string")
KEY_WORLD(handle, "handle")
KEY_WORLD(entity, "entity")
KEY_WORLD(delegate, "delegate")
KEY_WORLD(task, "task")
KEY_WORLD(array, "array")

KEY_WORLD(if, "if")
KEY_WORLD(elseif, "elseif")
KEY_WORLD(else, "else")
KEY_WORLD(while, "while")
KEY_WORLD(for, "for")
KEY_WORLD(break, "break")
KEY_WORLD(continue, "continue")
KEY_WORLD(return, "return")
KEY_WORLD(is, "is")
KEY_WORLD(as, "as")
KEY_WORLD(start, "start")
KEY_WORLD(new, "new")
KEY_WORLD(wait, "wait")
KEY_WORLD(exit, "exit")
KEY_WORLD(try, "try")
KEY_WORLD(catch, "catch")
KEY_WORLD(finally, "finally")

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