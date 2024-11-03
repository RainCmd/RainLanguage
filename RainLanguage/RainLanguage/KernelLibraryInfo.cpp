#include "KernelLibraryInfo.h"
#include "KernelFunctions.h"
#include "VirtualMachine.h"
#include "Real/MathReal.h"
#include "VirtualMachine/Exceptions.h"
#include "ClearStaticCache.h"
#include "KeyWords.h"

#define KERNEL_STRING(text) stringAgency.Add(text)
#define REGISIER_ENUM_ELEMENT(name,index,value)\
		ASSERT_DEBUG(elements.Count() == index, "枚举元素索引错误");\
		new (elements.Add())KernelLibraryInfo::Enum::Element(KERNEL_STRING(name), (integer)value);

#define REGISIER_ENUM(isPublic,space,name,index,elements)\
		ASSERT_DEBUG(enums.Count() == index, "枚举索引错误");\
		space->enums.Add(enums.Count());\
		new (enums.Add())KernelLibraryInfo::Enum(isPublic, KERNEL_STRING(name), elements);

#define REGISTER_MEMBER_VARIABLES(isPublic,name,type,address) new (memberVariables.Add())KernelLibraryInfo::Variable(isPublic, KERNEL_STRING(name), type, address);

#define REGISTER_MEMBER_FUNCTIONS(isPublic,name,returns,parameters,invoker)\
		memberFunctions.Add(functions.Count());\
		new (functions.Add())KernelLibraryInfo::Function(isPublic, KERNEL_STRING(name), returns, parameters, invoker);

#define REGISTER_STRUCT(isPublic,space,name,index,size,alignment,variables,functions)\
		ASSERT_DEBUG(structs.Count() == index, "结构体索引错误");\
		space->structs.Add(structs.Count());\
		new (structs.Add())KernelLibraryInfo::Struct(isPublic, KERNEL_STRING(name), size, alignment, variables, functions);

#define REGISTER_CLASS(isPublic,space,name,index,parent,inherits,size,alignment,constructors,variables,functions)\
		ASSERT_DEBUG(classes.Count() == index, "托管类型索引错误");\
		space->classes.Add(classes.Count());\
		new (classes.Add())KernelLibraryInfo::Class(isPublic, parent, inherits, KERNEL_STRING(name), size, alignment, constructors, variables, functions);

#define REGISTER_INTERFACE_FUNCTIONS(name,returns,parameters)\
		new (memberFunctions.Add())Interface::Function(KERNEL_STRING(name), returns, parameters);

#define REGISTER_INTERFACE(isPublic,space,name,index,inherits)\
		ASSERT_DEBUG(interfaces.Count() == index, "接口类型索引错误");\
		space->interfaces.Add(interfaces.Count());\
		new (interfaces.Add())KernelLibraryInfo::Interface(isPublic, inherits, KERNEL_STRING(name), memberFunctions);

#define REGISTER_FUNCTIONS(isPublic,space,name,returns,parameters,invoker)\
		space->functions.Add(functions.Count());\
		new (functions.Add())KernelLibraryInfo::Function(isPublic, KERNEL_STRING(name), returns, parameters, invoker);

#define REGISTER_SPECIAL_FUNCTIONS(isPublic,space,name,index,returns,parameters,invoker)\
		ASSERT_DEBUG(functions.Count() == index, "特殊函数索引错误");\
		REGISTER_FUNCTIONS(isPublic,space,name,returns,parameters,invoker)

#define REGISTER_VARIABLE(isPublic,space,name,type,value)\
		space->variables.Add(variables.Count());\
		new (variables.Add())KernelLibraryInfo::GlobalVariable(isPublic, KERNEL_STRING(name), type, AddData(value));

#define EMPTY_INDICES (List<uint32, true>(0))
#define EMPTY_TYPES (List<Type, true>(0))
#define EMPTY_DECLARATIONS (List<Declaration, true>(0))
#define EMPTY_VARIABLES (List<KernelLibraryInfo::Variable>(0))

KernelLibraryInfo::Space::~Space()
{
	for(uint32 i = 0; i < children.Count(); i++) delete children[i];
	children.Clear();
}
KernelLibraryInfo::~KernelLibraryInfo()
{
	delete root; root = NULL;
}
static KernelLibraryInfo* kernelLibraryInfo = NULL;
const KernelLibraryInfo* KernelLibraryInfo::GetKernelLibraryInfo()
{
	if(kernelLibraryInfo == NULL) kernelLibraryInfo = new KernelLibraryInfo();
	return kernelLibraryInfo;
}

void ClearKernelLibraryInfo()
{
	delete kernelLibraryInfo;
	kernelLibraryInfo = NULL;
}

inline TupleInfo CreateTypeList(const Type& type)
{
	TupleInfo result(1);
	result.AddElement(type, 0);
	return result;
}
inline TupleInfo CreateTypeList(const Type& type1, const Type& type2)
{
	TupleInfo result(2);
	result.AddElement(type1, 0);
	result.AddElement(type2, 0);
	return result;
}
inline TupleInfo CreateTypeList(const Type& type1, const Type& type2, const Type& type3)
{
	TupleInfo result(3);
	result.AddElement(type1, 0);
	result.AddElement(type2, 0);
	result.AddElement(type3, 0);
	return result;
}
inline TupleInfo CreateTypeList(const Type& type1, const Type& type2, const Type& type3, const Type& type4)
{
	TupleInfo result(4);
	result.AddElement(type1, 0);
	result.AddElement(type2, 0);
	result.AddElement(type3, 0);
	result.AddElement(type4, 0);
	return result;
}

inline List<Declaration, true> CreateDeclarationList(const Declaration& declaration)
{
	List<Declaration, true> result(1);
	result.Add(declaration);
	return result;
}

inline void GetStackSize(KernelLibraryInfo& library, Type type, uint32& size, uint8& alignment)
{
	if(type.dimension) type = TYPE_Handle;
	switch(type.code)
	{
		case TypeCode::Invalid:
			break;
		case TypeCode::Struct:
			size = library.structs[type.index].size;
			alignment = library.structs[type.index].alignment;
			return;
		case TypeCode::Enum:
			size = library.structs[TYPE_Enum.index].size;
			alignment = library.structs[TYPE_Enum.index].alignment;
			return;
		case TypeCode::Handle:
		case TypeCode::Interface:
		case TypeCode::Delegate:
		case TypeCode::Task:
			size = SIZE(Handle);
			alignment = MEMORY_ALIGNMENT_4;
			return;
		default:
			break;
	}
	EXCEPTION("无效的类型");
}

inline void CalculateTupleInfo(KernelLibraryInfo& library, TupleInfo& info)
{
	uint32 size = 0; uint8 alignment = 0;
	for(uint32 i = 0; i < info.Count(); i++)
	{
		GetStackSize(library, info.GetType(i), size, alignment);
		MemoryAlignment(info.size, info.GetOffset(i), alignment, size);
	}
}

inline void CalculateMemberFunctionParameterTupleInfo(KernelLibraryInfo& library, TupleInfo& info)
{
	uint32 size = 0; uint8 alignment = 0;
	GetStackSize(library, info.GetType(0), size, alignment);
	info.size += MemoryAlignment(size, MEMORY_ALIGNMENT_MAX);
	for(uint32 i = 1; i < info.Count(); i++)
	{
		GetStackSize(library, info.GetType(i), size, alignment);
		MemoryAlignment(info.size, info.GetOffset(i), alignment, size);
	}
}

inline void CalculateTupleInfo(KernelLibraryInfo& kernel)
{
	for(uint32 x = 0; x < kernel.structs.Count(); x++)
		for(uint32 y = 0; y < kernel.structs[x].functions.Count(); y++)
			CalculateMemberFunctionParameterTupleInfo(kernel, kernel.functions[kernel.structs[x].functions[y]].parameters);
	for(uint32 x = 0; x < kernel.classes.Count(); x++)
	{
		for(uint32 y = 0; y < kernel.classes[x].constructors.Count(); y++)
			CalculateMemberFunctionParameterTupleInfo(kernel, kernel.functions[kernel.classes[x].constructors[y]].parameters);
		for(uint32 y = 0; y < kernel.classes[x].functions.Count(); y++)
			CalculateMemberFunctionParameterTupleInfo(kernel, kernel.functions[kernel.classes[x].functions[y]].parameters);
	}
	for(uint32 x = 0; x < kernel.interfaces.Count(); x++)
		for(uint32 y = 0; y < kernel.interfaces[x].functions.Count(); y++)
		{
			CalculateMemberFunctionParameterTupleInfo(kernel, kernel.interfaces[x].functions[y].parameters);
			CalculateTupleInfo(kernel, kernel.interfaces[x].functions[y].returns);
		}
	for(uint32 x = 0; x < kernel.delegates.Count(); x++)
	{
		CalculateTupleInfo(kernel, kernel.delegates[x].parameters);
		CalculateTupleInfo(kernel, kernel.delegates[x].returns);
	}
	for(uint32 x = 0; x < kernel.tasks.Count(); x++)
		CalculateTupleInfo(kernel, kernel.tasks[x].returns);
	for(uint32 x = 0; x < kernel.functions.Count(); x++)
	{
		if(!kernel.functions[x].parameters.size) CalculateTupleInfo(kernel, kernel.functions[x].parameters);
		CalculateTupleInfo(kernel, kernel.functions[x].returns);
	}
}

KernelLibraryInfo::KernelLibraryInfo() :root(NULL), data(64), variables(0), enums(KERNEL_TYPE_ENUM_COUNT), structs(KERNEL_TYPE_STRUCT_COUNT), classes(KERNEL_TYPE_CLASS_COUNT), interfaces(0), delegates(0), tasks(0), functions(0), dataStrings(0), stringAgency(1024)
{
	root = new KernelLibraryInfo::Space(KERNEL_STRING(TEXT("kernel")));

	REGISTER_VARIABLE(true, root, TEXT("InvalidType"), TYPE_Type, Type(NULL, TypeCode::Invalid, NULL, NULL));

	//Operation
	{
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("<"), KERNEL_SPECIAL_FUNCTION_Less_integer_integer, CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Integer, TYPE_Integer), Operation_Less_integer_integer);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("<"), KERNEL_SPECIAL_FUNCTION_Less_real_real, CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Real, TYPE_Real), Operation_Less_real_real);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("<="), KERNEL_SPECIAL_FUNCTION_Less_Equals_integer_integer, CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Integer, TYPE_Integer), Operation_Less_Equals_integer_integer);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("<="), KERNEL_SPECIAL_FUNCTION_Less_Equals_real_real, CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Real, TYPE_Real), Operation_Less_Equals_real_real);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT(">"), KERNEL_SPECIAL_FUNCTION_Greater_integer_integer, CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Integer, TYPE_Integer), Operation_Greater_integer_integer);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT(">"), KERNEL_SPECIAL_FUNCTION_Greater_real_real, CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Real, TYPE_Real), Operation_Greater_real_real);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT(">="), KERNEL_SPECIAL_FUNCTION_Greater_Equals_integer_integer, CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Integer, TYPE_Integer), Operation_Greater_Equals_integer_integer);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT(">="), KERNEL_SPECIAL_FUNCTION_Greater_Equals_real_real, CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Real, TYPE_Real), Operation_Greater_Equals_real_real);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("=="), KERNEL_SPECIAL_FUNCTION_Equals_bool_bool, CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Bool, TYPE_Bool), Operation_Equals_bool_bool);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("=="), KERNEL_SPECIAL_FUNCTION_Equals_integer_integer, CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Integer, TYPE_Integer), Operation_Equals_integer_integer);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("=="), KERNEL_SPECIAL_FUNCTION_Equals_real_real, CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Real, TYPE_Real), Operation_Equals_real_real);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("=="), KERNEL_SPECIAL_FUNCTION_Equals_real2_real2, CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Real2, TYPE_Real2), Operation_Equals_real2_real2);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("=="), KERNEL_SPECIAL_FUNCTION_Equals_real3_real3, CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Real3, TYPE_Real3), Operation_Equals_real3_real3);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("=="), KERNEL_SPECIAL_FUNCTION_Equals_real4_real4, CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Real4, TYPE_Real4), Operation_Equals_real4_real4);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("=="), KERNEL_SPECIAL_FUNCTION_Equals_string_string, CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_String, TYPE_String), Operation_Equals_string_string);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("=="), KERNEL_SPECIAL_FUNCTION_Equals_handle_handle, CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Handle, TYPE_Handle), Operation_Equals_handle_handle);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("=="), KERNEL_SPECIAL_FUNCTION_Equals_entity_entity, CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Entity, TYPE_Entity), Operation_Equals_entity_entity);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("=="), KERNEL_SPECIAL_FUNCTION_Equals_delegate_delegate, CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Delegate, TYPE_Delegate), Operation_Equals_delegate_delegate);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("=="), KERNEL_SPECIAL_FUNCTION_Equals_type_type, CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Type, TYPE_Type), Operation_Equals_type_type);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("!="), KERNEL_SPECIAL_FUNCTION_Not_Equals_bool_bool, CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Bool, TYPE_Bool), Operation_Not_Equals_bool_bool);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("!="), KERNEL_SPECIAL_FUNCTION_Not_Equals_integer_integer, CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Integer, TYPE_Integer), Operation_Not_Equals_integer_integer);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("!="), KERNEL_SPECIAL_FUNCTION_Not_Equals_real_real, CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Real, TYPE_Real), Operation_Not_Equals_real_real);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("!="), KERNEL_SPECIAL_FUNCTION_Not_Equals_real2_real2, CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Real2, TYPE_Real2), Operation_Not_Equals_real2_real2);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("!="), KERNEL_SPECIAL_FUNCTION_Not_Equals_real3_real3, CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Real3, TYPE_Real3), Operation_Not_Equals_real3_real3);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("!="), KERNEL_SPECIAL_FUNCTION_Not_Equals_real4_real4, CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Real4, TYPE_Real4), Operation_Not_Equals_real4_real4);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("!="), KERNEL_SPECIAL_FUNCTION_Not_Equals_string_string, CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_String, TYPE_String), Operation_Not_Equals_string_string);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("!="), KERNEL_SPECIAL_FUNCTION_Not_Equals_handle_handle, CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Handle, TYPE_Handle), Operation_Not_Equals_handle_handle);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("!="), KERNEL_SPECIAL_FUNCTION_Not_Equals_entity_entity, CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Entity, TYPE_Entity), Operation_Not_Equals_entity_entity);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("!="), KERNEL_SPECIAL_FUNCTION_Not_Equals_delegate_delegate, CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Delegate, TYPE_Delegate), Operation_Not_Equals_delegate_delegate);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("!="), KERNEL_SPECIAL_FUNCTION_Not_Equals_type_type, CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Type, TYPE_Type), Operation_Not_Equals_type_type);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("&"), KERNEL_SPECIAL_FUNCTION_And_bool_bool, CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Bool, TYPE_Bool), Operation_And_bool_bool);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("&"), KERNEL_SPECIAL_FUNCTION_And_integer_integer, CreateTypeList(TYPE_Integer), CreateTypeList(TYPE_Integer, TYPE_Integer), Operation_And_integer_integer);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("|"), KERNEL_SPECIAL_FUNCTION_Or_bool_bool, CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Bool, TYPE_Bool), Operation_Or_bool_bool);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("|"), KERNEL_SPECIAL_FUNCTION_Or_integer_integer, CreateTypeList(TYPE_Integer), CreateTypeList(TYPE_Integer, TYPE_Integer), Operation_Or_integer_integer);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("^"), KERNEL_SPECIAL_FUNCTION_Xor_bool_bool, CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Bool, TYPE_Bool), Operation_Xor_bool_bool);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("^"), KERNEL_SPECIAL_FUNCTION_Xor_integer_integer, CreateTypeList(TYPE_Integer), CreateTypeList(TYPE_Integer, TYPE_Integer), Operation_Xor_integer_integer);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("<<"), KERNEL_SPECIAL_FUNCTION_Left_Shift_integer_integer, CreateTypeList(TYPE_Integer), CreateTypeList(TYPE_Integer, TYPE_Integer), Operation_Left_Shift_integer_integer);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT(">>"), KERNEL_SPECIAL_FUNCTION_Right_Shift_integer_integer, CreateTypeList(TYPE_Integer), CreateTypeList(TYPE_Integer, TYPE_Integer), Operation_Right_Shift_integer_integer);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("+"), KERNEL_SPECIAL_FUNCTION_Plus_integer_integer, CreateTypeList(TYPE_Integer), CreateTypeList(TYPE_Integer, TYPE_Integer), Operation_Plus_integer_integer);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("+"), KERNEL_SPECIAL_FUNCTION_Plus_real_real, CreateTypeList(TYPE_Real), CreateTypeList(TYPE_Real, TYPE_Real), Operation_Plus_real_real);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("+"), KERNEL_SPECIAL_FUNCTION_Plus_real2_real2, CreateTypeList(TYPE_Real2), CreateTypeList(TYPE_Real2, TYPE_Real2), Operation_Plus_real2_real2);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("+"), KERNEL_SPECIAL_FUNCTION_Plus_real3_real3, CreateTypeList(TYPE_Real3), CreateTypeList(TYPE_Real3, TYPE_Real3), Operation_Plus_real3_real3);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("+"), KERNEL_SPECIAL_FUNCTION_Plus_real4_real4, CreateTypeList(TYPE_Real4), CreateTypeList(TYPE_Real4, TYPE_Real4), Operation_Plus_real4_real4);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("+"), KERNEL_SPECIAL_FUNCTION_Plus_string_string, CreateTypeList(TYPE_String), CreateTypeList(TYPE_String, TYPE_String), Operation_Plus_string_string);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("+"), KERNEL_SPECIAL_FUNCTION_Plus_string_bool, CreateTypeList(TYPE_String), CreateTypeList(TYPE_String, TYPE_Bool), Operation_Plus_string_bool);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("+"), KERNEL_SPECIAL_FUNCTION_Plus_string_char, CreateTypeList(TYPE_String), CreateTypeList(TYPE_String, TYPE_Char), Operation_Plus_string_char);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("+"), KERNEL_SPECIAL_FUNCTION_Plus_string_integer, CreateTypeList(TYPE_String), CreateTypeList(TYPE_String, TYPE_Integer), Operation_Plus_string_integer);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("+"), KERNEL_SPECIAL_FUNCTION_Plus_string_real, CreateTypeList(TYPE_String), CreateTypeList(TYPE_String, TYPE_Real), Operation_Plus_string_real);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("+"), KERNEL_SPECIAL_FUNCTION_Plus_string_handle, CreateTypeList(TYPE_String), CreateTypeList(TYPE_String, TYPE_Handle), Operation_Plus_string_handle);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("+"), KERNEL_SPECIAL_FUNCTION_Plus_string_type, CreateTypeList(TYPE_String), CreateTypeList(TYPE_String, TYPE_Type), Operation_Plus_string_type);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("+"), KERNEL_SPECIAL_FUNCTION_Plus_bool_string, CreateTypeList(TYPE_String), CreateTypeList(TYPE_Bool, TYPE_String), Operation_Plus_bool_string);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("+"), KERNEL_SPECIAL_FUNCTION_Plus_char_string, CreateTypeList(TYPE_String), CreateTypeList(TYPE_Char, TYPE_String), Operation_Plus_char_string);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("+"), KERNEL_SPECIAL_FUNCTION_Plus_integer_string, CreateTypeList(TYPE_String), CreateTypeList(TYPE_Integer, TYPE_String), Operation_Plus_integer_string);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("+"), KERNEL_SPECIAL_FUNCTION_Plus_real_string, CreateTypeList(TYPE_String), CreateTypeList(TYPE_Real, TYPE_String), Operation_Plus_real_string);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("+"), KERNEL_SPECIAL_FUNCTION_Plus_handle_string, CreateTypeList(TYPE_String), CreateTypeList(TYPE_Handle, TYPE_String), Operation_Plus_handle_string);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("+"), KERNEL_SPECIAL_FUNCTION_Plus_type_string, CreateTypeList(TYPE_String), CreateTypeList(TYPE_Type, TYPE_String), Operation_Plus_type_string);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("-"), KERNEL_SPECIAL_FUNCTION_Minus_integer_integer, CreateTypeList(TYPE_Integer), CreateTypeList(TYPE_Integer, TYPE_Integer), Operation_Minus_integer_integer);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("-"), KERNEL_SPECIAL_FUNCTION_Minus_real_real, CreateTypeList(TYPE_Real), CreateTypeList(TYPE_Real, TYPE_Real), Operation_Minus_real_real);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("-"), KERNEL_SPECIAL_FUNCTION_Minus_real2_real2, CreateTypeList(TYPE_Real2), CreateTypeList(TYPE_Real2, TYPE_Real2), Operation_Minus_real2_real2);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("-"), KERNEL_SPECIAL_FUNCTION_Minus_real3_real3, CreateTypeList(TYPE_Real3), CreateTypeList(TYPE_Real3, TYPE_Real3), Operation_Minus_real3_real3);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("-"), KERNEL_SPECIAL_FUNCTION_Minus_real4_real4, CreateTypeList(TYPE_Real4), CreateTypeList(TYPE_Real4, TYPE_Real4), Operation_Minus_real4_real4);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("*"), KERNEL_SPECIAL_FUNCTION_Mul_integer_integer, CreateTypeList(TYPE_Integer), CreateTypeList(TYPE_Integer, TYPE_Integer), Operation_Mul_integer_integer);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("*"), KERNEL_SPECIAL_FUNCTION_Mul_real_real, CreateTypeList(TYPE_Real), CreateTypeList(TYPE_Real, TYPE_Real), Operation_Mul_real_real);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("*"), KERNEL_SPECIAL_FUNCTION_Mul_real2_real, CreateTypeList(TYPE_Real2), CreateTypeList(TYPE_Real2, TYPE_Real), Operation_Mul_real2_real);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("*"), KERNEL_SPECIAL_FUNCTION_Mul_real3_real, CreateTypeList(TYPE_Real3), CreateTypeList(TYPE_Real3, TYPE_Real), Operation_Mul_real3_real);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("*"), KERNEL_SPECIAL_FUNCTION_Mul_real4_real, CreateTypeList(TYPE_Real4), CreateTypeList(TYPE_Real4, TYPE_Real), Operation_Mul_real4_real);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("*"), KERNEL_SPECIAL_FUNCTION_Mul_real_real2, CreateTypeList(TYPE_Real2), CreateTypeList(TYPE_Real, TYPE_Real2), Operation_Mul_real_real2);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("*"), KERNEL_SPECIAL_FUNCTION_Mul_real_real3, CreateTypeList(TYPE_Real3), CreateTypeList(TYPE_Real, TYPE_Real3), Operation_Mul_real_real3);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("*"), KERNEL_SPECIAL_FUNCTION_Mul_real_real4, CreateTypeList(TYPE_Real4), CreateTypeList(TYPE_Real, TYPE_Real4), Operation_Mul_real_real4);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("*"), KERNEL_SPECIAL_FUNCTION_Mul_real2_real2, CreateTypeList(TYPE_Real2), CreateTypeList(TYPE_Real2, TYPE_Real2), Operation_Mul_real2_real2);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("*"), KERNEL_SPECIAL_FUNCTION_Mul_real3_real3, CreateTypeList(TYPE_Real3), CreateTypeList(TYPE_Real3, TYPE_Real3), Operation_Mul_real3_real3);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("*"), KERNEL_SPECIAL_FUNCTION_Mul_real4_real4, CreateTypeList(TYPE_Real4), CreateTypeList(TYPE_Real4, TYPE_Real4), Operation_Mul_real4_real4);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("/"), KERNEL_SPECIAL_FUNCTION_Div_integer_integer, CreateTypeList(TYPE_Integer), CreateTypeList(TYPE_Integer, TYPE_Integer), Operation_Div_integer_integer);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("/"), KERNEL_SPECIAL_FUNCTION_Div_real_real, CreateTypeList(TYPE_Real), CreateTypeList(TYPE_Real, TYPE_Real), Operation_Div_real_real);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("/"), KERNEL_SPECIAL_FUNCTION_Div_real2_real, CreateTypeList(TYPE_Real2), CreateTypeList(TYPE_Real2, TYPE_Real), Operation_Div_real2_real);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("/"), KERNEL_SPECIAL_FUNCTION_Div_real3_real, CreateTypeList(TYPE_Real3), CreateTypeList(TYPE_Real3, TYPE_Real), Operation_Div_real3_real);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("/"), KERNEL_SPECIAL_FUNCTION_Div_real4_real, CreateTypeList(TYPE_Real4), CreateTypeList(TYPE_Real4, TYPE_Real), Operation_Div_real4_real);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("/"), KERNEL_SPECIAL_FUNCTION_Div_real_real2, CreateTypeList(TYPE_Real2), CreateTypeList(TYPE_Real, TYPE_Real2), Operation_Div_real_real2);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("/"), KERNEL_SPECIAL_FUNCTION_Div_real_real3, CreateTypeList(TYPE_Real3), CreateTypeList(TYPE_Real, TYPE_Real3), Operation_Div_real_real3);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("/"), KERNEL_SPECIAL_FUNCTION_Div_real_real4, CreateTypeList(TYPE_Real4), CreateTypeList(TYPE_Real, TYPE_Real4), Operation_Div_real_real4);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("/"), KERNEL_SPECIAL_FUNCTION_Div_real2_real2, CreateTypeList(TYPE_Real2), CreateTypeList(TYPE_Real2, TYPE_Real2), Operation_Div_real2_real2);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("/"), KERNEL_SPECIAL_FUNCTION_Div_real3_real3, CreateTypeList(TYPE_Real3), CreateTypeList(TYPE_Real3, TYPE_Real3), Operation_Div_real3_real3);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("/"), KERNEL_SPECIAL_FUNCTION_Div_real4_real4, CreateTypeList(TYPE_Real4), CreateTypeList(TYPE_Real4, TYPE_Real4), Operation_Div_real4_real4);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("%"), KERNEL_SPECIAL_FUNCTION_Mod_integer_integer, CreateTypeList(TYPE_Integer), CreateTypeList(TYPE_Integer, TYPE_Integer), Operation_Mod_integer_integer);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("!"), KERNEL_SPECIAL_FUNCTION_Not_bool, CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Bool), Operation_Not_bool);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("~"), KERNEL_SPECIAL_FUNCTION_Inverse_integer, CreateTypeList(TYPE_Integer), CreateTypeList(TYPE_Integer), Operation_Inverse_integer);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("+"), KERNEL_SPECIAL_FUNCTION_Positive_integer, CreateTypeList(TYPE_Integer), CreateTypeList(TYPE_Integer), Operation_Positive_integer);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("+"), KERNEL_SPECIAL_FUNCTION_Positive_real, CreateTypeList(TYPE_Real), CreateTypeList(TYPE_Real), Operation_Positive_real);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("+"), KERNEL_SPECIAL_FUNCTION_Positive_real2, CreateTypeList(TYPE_Real2), CreateTypeList(TYPE_Real2), Operation_Positive_real2);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("+"), KERNEL_SPECIAL_FUNCTION_Positive_real3, CreateTypeList(TYPE_Real3), CreateTypeList(TYPE_Real3), Operation_Positive_real3);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("+"), KERNEL_SPECIAL_FUNCTION_Positive_real4, CreateTypeList(TYPE_Real4), CreateTypeList(TYPE_Real4), Operation_Positive_real4);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("-"), KERNEL_SPECIAL_FUNCTION_Negative_integer, CreateTypeList(TYPE_Integer), CreateTypeList(TYPE_Integer), Operation_Negative_integer);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("-"), KERNEL_SPECIAL_FUNCTION_Negative_real, CreateTypeList(TYPE_Real), CreateTypeList(TYPE_Real), Operation_Negative_real);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("-"), KERNEL_SPECIAL_FUNCTION_Negative_real2, CreateTypeList(TYPE_Real2), CreateTypeList(TYPE_Real2), Operation_Negative_real2);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("-"), KERNEL_SPECIAL_FUNCTION_Negative_real3, CreateTypeList(TYPE_Real3), CreateTypeList(TYPE_Real3), Operation_Negative_real3);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("-"), KERNEL_SPECIAL_FUNCTION_Negative_real4, CreateTypeList(TYPE_Real4), CreateTypeList(TYPE_Real4), Operation_Negative_real4);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("++"), KERNEL_SPECIAL_FUNCTION_Increment_integer, TupleInfo_EMPTY, CreateTypeList(TYPE_Integer), Operation_Increment_integer);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("++"), KERNEL_SPECIAL_FUNCTION_Increment_real, TupleInfo_EMPTY, CreateTypeList(TYPE_Real), Operation_Increment_real);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("--"), KERNEL_SPECIAL_FUNCTION_Decrement_integer, TupleInfo_EMPTY, CreateTypeList(TYPE_Integer), Operation_Decrement_integer);
		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("--"), KERNEL_SPECIAL_FUNCTION_Decrement_real, TupleInfo_EMPTY, CreateTypeList(TYPE_Real), Operation_Decrement_real);

		REGISTER_SPECIAL_FUNCTIONS(true, root, TEXT("GetTypeName"), KERNEL_SPECIAL_FUNCTION_GetTypeName, CreateTypeList(TYPE_String), CreateTypeList(TYPE_Type), GetTypeName);
	}

	//enum TaskState
	{
		List<KernelLibraryInfo::Enum::Element> elements(6);
		REGISIER_ENUM_ELEMENT(TEXT("Unstart"), KERNEL_TASK_STATE_INDEX_Unstart, InvokerState::Unstart);
		REGISIER_ENUM_ELEMENT(TEXT("Running"), KERNEL_TASK_STATE_INDEX_Running, InvokerState::Running);
		REGISIER_ENUM_ELEMENT(TEXT("Completed"), KERNEL_TASK_STATE_INDEX_Completed, InvokerState::Completed);
		REGISIER_ENUM_ELEMENT(TEXT("Exceptional"), KERNEL_TASK_STATE_INDEX_Exceptional, InvokerState::Exceptional);
		REGISIER_ENUM_ELEMENT(TEXT("Aborted"), KERNEL_TASK_STATE_INDEX_Aborted, InvokerState::Aborted);
		REGISIER_ENUM_ELEMENT(TEXT("Invalid"), KERNEL_TASK_STATE_INDEX_Invalid, InvokerState::Invalid);
		REGISIER_ENUM(true, root, TEXT("TaskState"), KERNEL_TYPE_ENUM_INDEX_TaskState, elements);
	}

	//struct bool
	{
		List<uint32, true> memberFunctions = List<uint32, true>(1);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("ToString"), CreateTypeList(TYPE_String), CreateTypeList(TYPE_Bool), bool_ToString);
		REGISTER_STRUCT(true, root, KeyWord_bool(), KERNEL_TYPE_STRUCT_INDEX_Bool, SIZE(bool), MEMORY_ALIGNMENT_1, EMPTY_VARIABLES, memberFunctions);
	}
	//struct byte
	{
		List<uint32, true> memberFunctions = List<uint32, true>(1);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("ToString"), CreateTypeList(TYPE_String), CreateTypeList(TYPE_Byte), byte_ToString);
		REGISTER_STRUCT(true, root, KeyWord_byte(), KERNEL_TYPE_STRUCT_INDEX_Byte, SIZE(uint8), MEMORY_ALIGNMENT_1, EMPTY_VARIABLES, memberFunctions);
	}
	//struct char
	{
		List<uint32, true> memberFunctions = List<uint32, true>(1);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("ToString"), CreateTypeList(TYPE_String), CreateTypeList(TYPE_Char), char_ToString);
		REGISTER_STRUCT(true, root, KeyWord_char(), KERNEL_TYPE_STRUCT_INDEX_Char, SIZE(character), MEMORY_ALIGNMENT_2, EMPTY_VARIABLES, memberFunctions);
	}
	//struct integer
	{
		List<uint32, true> memberFunctions = List<uint32, true>(1);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("ToString"), CreateTypeList(TYPE_String), CreateTypeList(TYPE_Integer), integer_ToString);
		REGISTER_STRUCT(true, root, KeyWord_integer(), KERNEL_TYPE_STRUCT_INDEX_Integer, SIZE(integer), MEMORY_ALIGNMENT_8, EMPTY_VARIABLES, memberFunctions);
	}
	//struct real
	{
		List<uint32, true> memberFunctions = List<uint32, true>(1);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("ToString"), CreateTypeList(TYPE_String), CreateTypeList(TYPE_Real), real_ToString);
		REGISTER_STRUCT(true, root, KeyWord_real(), KERNEL_TYPE_STRUCT_INDEX_Real, SIZE(real), MEMORY_ALIGNMENT_REAL, EMPTY_VARIABLES, memberFunctions);
	}
	//struct real2
	{
		List<KernelLibraryInfo::Variable> memberVariables = List<KernelLibraryInfo::Variable>(2);
		REGISTER_MEMBER_VARIABLES(true, TEXT("x"), TYPE_Real, GET_FIELD_OFFSET(Real2, x));
		REGISTER_MEMBER_VARIABLES(true, TEXT("y"), TYPE_Real, GET_FIELD_OFFSET(Real2, y));
		List<uint32, true> memberFunctions = List<uint32, true>(3);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("Normalized"), CreateTypeList(TYPE_Real2), CreateTypeList(TYPE_Real2), real2_Normalized);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("Magnitude"), CreateTypeList(TYPE_Real), CreateTypeList(TYPE_Real2), real2_Magnitude);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("SqrMagnitude"), CreateTypeList(TYPE_Real), CreateTypeList(TYPE_Real2), real2_SqrMagnitude);
		REGISTER_STRUCT(true, root, KeyWord_real2(), KERNEL_TYPE_STRUCT_INDEX_Real2, SIZE(Real2), MEMORY_ALIGNMENT_REAL, memberVariables, memberFunctions);
	}
	//struct real3
	{
		List<KernelLibraryInfo::Variable> memberVariables = List<KernelLibraryInfo::Variable>(3);
		REGISTER_MEMBER_VARIABLES(true, TEXT("x"), TYPE_Real, GET_FIELD_OFFSET(Real3, x));
		REGISTER_MEMBER_VARIABLES(true, TEXT("y"), TYPE_Real, GET_FIELD_OFFSET(Real3, y));
		REGISTER_MEMBER_VARIABLES(true, TEXT("z"), TYPE_Real, GET_FIELD_OFFSET(Real3, z));
		List<uint32, true> memberFunctions = List<uint32, true>(3);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("Normalized"), CreateTypeList(TYPE_Real3), CreateTypeList(TYPE_Real3), real3_Normalized);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("Magnitude"), CreateTypeList(TYPE_Real), CreateTypeList(TYPE_Real3), real3_Magnitude);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("SqrMagnitude"), CreateTypeList(TYPE_Real), CreateTypeList(TYPE_Real3), real3_SqrMagnitude);
		REGISTER_STRUCT(true, root, KeyWord_real3(), KERNEL_TYPE_STRUCT_INDEX_Real3, SIZE(Real3), MEMORY_ALIGNMENT_REAL, memberVariables, memberFunctions);
	}
	//struct real4
	{
		List<KernelLibraryInfo::Variable> memberVariables = List<KernelLibraryInfo::Variable>(4);
		REGISTER_MEMBER_VARIABLES(true, TEXT("x"), TYPE_Real, GET_FIELD_OFFSET(Real4, x));
		REGISTER_MEMBER_VARIABLES(true, TEXT("y"), TYPE_Real, GET_FIELD_OFFSET(Real4, y));
		REGISTER_MEMBER_VARIABLES(true, TEXT("z"), TYPE_Real, GET_FIELD_OFFSET(Real4, z));
		REGISTER_MEMBER_VARIABLES(true, TEXT("w"), TYPE_Real, GET_FIELD_OFFSET(Real4, w));
		List<uint32, true> memberFunctions = List<uint32, true>(3);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("Normalized"), CreateTypeList(TYPE_Real4), CreateTypeList(TYPE_Real4), real4_Normalized);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("Magnitude"), CreateTypeList(TYPE_Real), CreateTypeList(TYPE_Real4), real4_Magnitude);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("SqrMagnitude"), CreateTypeList(TYPE_Real), CreateTypeList(TYPE_Real4), real4_SqrMagnitude);
		REGISTER_STRUCT(true, root, KeyWord_real4(), KERNEL_TYPE_STRUCT_INDEX_Real4, SIZE(Real4), MEMORY_ALIGNMENT_REAL, memberVariables, memberFunctions);
	}
	//struct enum
	{
		List<uint32, true> memberFunctions = List<uint32, true>(1);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("ToString"), CreateTypeList(TYPE_String), CreateTypeList(TYPE_Enum), enum_ToString);
		REGISTER_STRUCT(true, root, KeyWord_enum(), KERNEL_TYPE_STRUCT_INDEX_Enum, SIZE(integer), MEMORY_ALIGNMENT_8, EMPTY_VARIABLES, memberFunctions);
	}
	//struct type
	{
		List<uint32, true> memberFunctions = List<uint32, true>(25);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("IsPublic"), CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Type), type_IsPublic);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetAttributes"), CreateTypeList(Type(TYPE_String, 1)), CreateTypeList(TYPE_Type), type_GetAttributes);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetName"), CreateTypeList(TYPE_String), CreateTypeList(TYPE_Type), type_GetName);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetParent"), CreateTypeList(TYPE_Type), CreateTypeList(TYPE_Type), type_GetParent);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetInherits"), CreateTypeList(Type(TYPE_Type, 1)), CreateTypeList(TYPE_Type), type_GetInherits);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetConstructors"), CreateTypeList(Type(TYPE_Reflection_MemberConstructor, 1)), CreateTypeList(TYPE_Type), type_GetConstructors);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetVariables"), CreateTypeList(Type(TYPE_Reflection_MemberVariable, 1)), CreateTypeList(TYPE_Type), type_GetVariables);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetFunctions"), CreateTypeList(Type(TYPE_Reflection_MemberFunction, 1)), CreateTypeList(TYPE_Type), type_GetFunctions);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetSpace"), CreateTypeList(TYPE_Reflection_Space), CreateTypeList(TYPE_Type), type_GetSpace);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetTypeCode"), CreateTypeList(TYPE_Reflection_TypeCode), CreateTypeList(TYPE_Type), type_GetTypeCode);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("IsAssignable"), CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Type, TYPE_Type), type_IsAssignable);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("IsValid"), CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Type), type_IsValid);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetEnumElements"), CreateTypeList(Type(TYPE_Integer, 1)), CreateTypeList(TYPE_Type), type_GetEnumElements);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetEnumElementNames"), CreateTypeList(Type(TYPE_String, 1)), CreateTypeList(TYPE_Type), type_GetEnumElementNames);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetParameters"), CreateTypeList(Type(TYPE_Type, 1)), CreateTypeList(TYPE_Type), type_GetParameters);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetReturns"), CreateTypeList(Type(TYPE_Type, 1)), CreateTypeList(TYPE_Type), type_GetReturns);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("CreateUninitialized"), CreateTypeList(TYPE_Handle), CreateTypeList(TYPE_Type), type_CreateUninitialized);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("CreateDelegate"), CreateTypeList(TYPE_Delegate), CreateTypeList(TYPE_Type, TYPE_Reflection_Native), type_CreateDelegate2);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("CreateDelegate"), CreateTypeList(TYPE_Delegate), CreateTypeList(TYPE_Type, TYPE_Reflection_MemberFunction, TYPE_Handle), type_CreateDelegate3);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("CreateDelegate"), CreateTypeList(TYPE_Delegate), CreateTypeList(TYPE_Type, TYPE_Reflection_Function), type_CreateDelegate);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("CreateTask"), CreateTypeList(TYPE_Task), CreateTypeList(TYPE_Type, TYPE_Reflection_Function, Type(TYPE_Handle, 1)), type_CreateTask);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("CreateTask"), CreateTypeList(TYPE_Task), CreateTypeList(TYPE_Type, TYPE_Reflection_MemberFunction, TYPE_Handle, Type(TYPE_Handle, 1)), type_CreateTask2);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("CreateArray"), CreateTypeList(TYPE_Array), CreateTypeList(TYPE_Type, TYPE_Integer), type_CreateArray);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetArrayRank"), CreateTypeList(TYPE_Integer), CreateTypeList(TYPE_Type), type_GetArrayRank);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetArrayElementType"), CreateTypeList(TYPE_Type), CreateTypeList(TYPE_Type), type_GetArrayElementType);
		REGISTER_STRUCT(true, root, KeyWord_type(), KERNEL_TYPE_STRUCT_INDEX_Type, SIZE(Type), MEMORY_ALIGNMENT_4, EMPTY_VARIABLES, memberFunctions);
	}
	//struct string
	{
		List<uint32, true> memberFunctions = List<uint32, true>(6);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetLength"), CreateTypeList(TYPE_Integer), CreateTypeList(TYPE_String), string_GetLength);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetStringID"), CreateTypeList(TYPE_Integer), CreateTypeList(TYPE_String), string_GetStringID);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("ToBool"), CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_String), string_ToBool);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("ToInteger"), CreateTypeList(TYPE_Integer), CreateTypeList(TYPE_String), string_ToInteger);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("ToReal"), CreateTypeList(TYPE_Real), CreateTypeList(TYPE_String), string_ToReal);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("ToChars"), CreateTypeList(Type(TYPE_Char, 1)), CreateTypeList(TYPE_String), string_ToChars);
		REGISTER_STRUCT(true, root, KeyWord_string(), KERNEL_TYPE_STRUCT_INDEX_String, SIZE(string), MEMORY_ALIGNMENT_4, EMPTY_VARIABLES, memberFunctions);
	}
	//struct entity
	{
		List<uint32, true> memberFunctions = List<uint32, true>(1);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetEntityID"), CreateTypeList(TYPE_Integer), CreateTypeList(TYPE_Entity), entity_GetEntityID);
		REGISTER_STRUCT(true, root, KeyWord_entity(), KERNEL_TYPE_STRUCT_INDEX_Entity, SIZE(Entity), MEMORY_ALIGNMENT_4, EMPTY_VARIABLES, memberFunctions);
	}
	//class handle
	{
		List<uint32, true> memberFunctions = List<uint32, true>(3);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetHandleID"), CreateTypeList(TYPE_Integer), CreateTypeList(TYPE_Handle), handle_GetHandleID);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("ToString"), CreateTypeList(TYPE_String), CreateTypeList(TYPE_Handle), handle_ToString);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetType"), CreateTypeList(TYPE_Type), CreateTypeList(TYPE_Handle), handle_GetType);
		REGISTER_CLASS(true, root, KeyWord_handle(), KERNEL_TYPE_CLASS_INDEX_Handle, Declaration(), EMPTY_DECLARATIONS, 0, MEMORY_ALIGNMENT_0, EMPTY_INDICES, EMPTY_VARIABLES, memberFunctions);
	}
	//class Delegate
	{
		List<uint32, true> memberFunctions = List<uint32, true>(1);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("Invoke"), CreateTypeList(Type(TYPE_Handle, 1)), CreateTypeList(TYPE_Delegate, Type(TYPE_Handle, 1)), delegate_Invoke);
		REGISTER_CLASS(true, root, TEXT("Delegate"), KERNEL_TYPE_CLASS_INDEX_Delegate, TYPE_Handle, EMPTY_DECLARATIONS, SIZE(::Delegate), MEMORY_ALIGNMENT_4, EMPTY_INDICES, EMPTY_VARIABLES, memberFunctions);
	}
	//class Task
	{
		List<uint32, true> memberFunctions = List<uint32, true>(11);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("Start"), TupleInfo_EMPTY, CreateTypeList(TYPE_Task, TYPE_Bool, TYPE_Bool), task_Start);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("Abort"), TupleInfo_EMPTY, CreateTypeList(TYPE_Task), task_Abort);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("SetName"), TupleInfo_EMPTY, CreateTypeList(TYPE_Task, TYPE_String), task_SetName);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetName"), CreateTypeList(TYPE_String), CreateTypeList(TYPE_Task), task_GetName);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("InstantID"), CreateTypeList(TYPE_Integer), CreateTypeList(TYPE_Task), task_GetState);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetState"), CreateTypeList(TYPE_TaskState), CreateTypeList(TYPE_Task), task_GetState);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetExitCode"), CreateTypeList(TYPE_String), CreateTypeList(TYPE_Task), task_GetExitCode);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("IsPause"), CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Task), task_IsPause);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("Pause"), TupleInfo_EMPTY, CreateTypeList(TYPE_Task), task_Pause);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("Resume"), TupleInfo_EMPTY, CreateTypeList(TYPE_Task), task_Resume);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetResults"), CreateTypeList(Type(TYPE_Handle, 1)), CreateTypeList(TYPE_Task), task_GetResults);
		REGISTER_CLASS(true, root, TEXT("Task"), KERNEL_TYPE_CLASS_INDEX_Task, TYPE_Handle, EMPTY_DECLARATIONS, 8, MEMORY_ALIGNMENT_8, EMPTY_INDICES, EMPTY_VARIABLES, memberFunctions);
	}
	//class array
	{
		List<uint32, true> memberFunctions = List<uint32, true>(4);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetLength"), CreateTypeList(TYPE_Integer), CreateTypeList(TYPE_Array), array_GetLength);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetElement"), CreateTypeList(TYPE_Handle), CreateTypeList(TYPE_Array, TYPE_Integer), array_GetElement);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("SetElement"), TupleInfo_EMPTY, CreateTypeList(TYPE_Array, TYPE_Integer, TYPE_Handle), array_SetElement);
		REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetEnumerator"), CreateTypeList(TYPE_Collections_Enumerator), CreateTypeList(TYPE_Array), array_GetEnumerator);
		REGISTER_CLASS(true, root, KeyWord_array(), KERNEL_TYPE_CLASS_INDEX_Array, TYPE_Handle, CreateDeclarationList(TYPE_Collections_Enumerable), 4, MEMORY_ALIGNMENT_4, EMPTY_INDICES, EMPTY_VARIABLES, memberFunctions);
	}
	//space BitConvert
	{
		Space* space = new KernelLibraryInfo::Space(KERNEL_STRING(TEXT("BitConvert")), root);
		TupleInfo byte_x8(8, 8);
		for(uint32 i = 0; i < 8; i++)
			byte_x8.AddElement(TYPE_Byte, i);

		REGISTER_FUNCTIONS(true, space, TEXT("BytesConvertInteger"), CreateTypeList(TYPE_Integer), byte_x8, BytesConvertInteger);
		REGISTER_FUNCTIONS(true, space, TEXT("BytesConvertReal"), CreateTypeList(TYPE_Real), byte_x8, BytesConvertReal);
		REGISTER_FUNCTIONS(true, space, TEXT("BytesConvertString"), CreateTypeList(TYPE_String), CreateTypeList(Type(TYPE_Byte, 1)), BytesConvertString);
		REGISTER_FUNCTIONS(true, space, TEXT("IntegerConvertBytes"), byte_x8, CreateTypeList(TYPE_Integer), IntegerConvertBytes);
		REGISTER_FUNCTIONS(true, space, TEXT("RealConvertBytes"), byte_x8, CreateTypeList(TYPE_Real), RealConvertBytes);
		REGISTER_FUNCTIONS(true, space, TEXT("StringConvertBytes"), CreateTypeList(Type(TYPE_Byte, 1)), CreateTypeList(TYPE_String), StringConvertBytes);
	}
	//space Math
	{
		Space* space = new KernelLibraryInfo::Space(KERNEL_STRING(TEXT("Math")), root);

		REGISTER_VARIABLE(true, space, TEXT("PI"), TYPE_Real, PI);
		REGISTER_VARIABLE(true, space, TEXT("E"), TYPE_Real, E);
		REGISTER_VARIABLE(true, space, TEXT("Deg2Rad"), TYPE_Real, (PI / (real)180));
		REGISTER_VARIABLE(true, space, TEXT("Rad2Deg"), TYPE_Real, ((real)180 / PI));

		REGISTER_FUNCTIONS(true, space, TEXT("Abs"), CreateTypeList(TYPE_Integer), CreateTypeList(TYPE_Integer), integer_Abs);
		REGISTER_FUNCTIONS(true, space, TEXT("Clamp"), CreateTypeList(TYPE_Integer), CreateTypeList(TYPE_Integer, TYPE_Integer, TYPE_Integer), integer_Clamp);
		REGISTER_FUNCTIONS(true, space, TEXT("GetRandomInt"), CreateTypeList(TYPE_Integer), TupleInfo_EMPTY, integer_GetRandomInt);
		REGISTER_FUNCTIONS(true, space, TEXT("Max"), CreateTypeList(TYPE_Integer), CreateTypeList(TYPE_Integer, TYPE_Integer), integer_Max);
		REGISTER_FUNCTIONS(true, space, TEXT("Min"), CreateTypeList(TYPE_Integer), CreateTypeList(TYPE_Integer, TYPE_Integer), integer_Min);
		REGISTER_FUNCTIONS(true, space, TEXT("Abs"), CreateTypeList(TYPE_Real), CreateTypeList(TYPE_Real), integer_Abs);
		REGISTER_FUNCTIONS(true, space, TEXT("Acos"), CreateTypeList(TYPE_Real), CreateTypeList(TYPE_Real), real_Acos);
		REGISTER_FUNCTIONS(true, space, TEXT("Asin"), CreateTypeList(TYPE_Real), CreateTypeList(TYPE_Real), real_Asin);
		REGISTER_FUNCTIONS(true, space, TEXT("Atan"), CreateTypeList(TYPE_Real), CreateTypeList(TYPE_Real), real_Atan);
		REGISTER_FUNCTIONS(true, space, TEXT("Atan2"), CreateTypeList(TYPE_Real), CreateTypeList(TYPE_Real, TYPE_Real), real_Atan2);
		REGISTER_FUNCTIONS(true, space, TEXT("Ceil"), CreateTypeList(TYPE_Integer), CreateTypeList(TYPE_Real), real_Ceil);
		REGISTER_FUNCTIONS(true, space, TEXT("Clamp"), CreateTypeList(TYPE_Real), CreateTypeList(TYPE_Real, TYPE_Real, TYPE_Real), real_Clamp);
		REGISTER_FUNCTIONS(true, space, TEXT("Clamp01"), CreateTypeList(TYPE_Real), CreateTypeList(TYPE_Real), real_Clamp01);
		REGISTER_FUNCTIONS(true, space, TEXT("Cos"), CreateTypeList(TYPE_Real), CreateTypeList(TYPE_Real), real_Cos);
		REGISTER_FUNCTIONS(true, space, TEXT("Floor"), CreateTypeList(TYPE_Integer), CreateTypeList(TYPE_Real), real_Floor);
		REGISTER_FUNCTIONS(true, space, TEXT("GetRandomReal"), CreateTypeList(TYPE_Real), TupleInfo_EMPTY, real_GetRandomReal);
		REGISTER_FUNCTIONS(true, space, TEXT("Lerp"), CreateTypeList(TYPE_Real), CreateTypeList(TYPE_Real, TYPE_Real, TYPE_Real), real_Lerp);
		REGISTER_FUNCTIONS(true, space, TEXT("Max"), CreateTypeList(TYPE_Real), CreateTypeList(TYPE_Real, TYPE_Real), real_Max);
		REGISTER_FUNCTIONS(true, space, TEXT("Min"), CreateTypeList(TYPE_Real), CreateTypeList(TYPE_Real, TYPE_Real), real_Min);
		REGISTER_FUNCTIONS(true, space, TEXT("Round"), CreateTypeList(TYPE_Integer), CreateTypeList(TYPE_Real), real_Round);
		REGISTER_FUNCTIONS(true, space, TEXT("Sign"), CreateTypeList(TYPE_Integer), CreateTypeList(TYPE_Real), real_Sign);
		REGISTER_FUNCTIONS(true, space, TEXT("Sin"), CreateTypeList(TYPE_Real), CreateTypeList(TYPE_Real), real_Sin);
		REGISTER_FUNCTIONS(true, space, TEXT("SinCos"), CreateTypeList(TYPE_Real, TYPE_Real), CreateTypeList(TYPE_Real), real_SinCos);
		REGISTER_FUNCTIONS(true, space, TEXT("Sqrt"), CreateTypeList(TYPE_Real), CreateTypeList(TYPE_Real), real_Sqrt);
		REGISTER_FUNCTIONS(true, space, TEXT("Tan"), CreateTypeList(TYPE_Real), CreateTypeList(TYPE_Real), real_Tan);
		REGISTER_FUNCTIONS(true, space, TEXT("Angle"), CreateTypeList(TYPE_Real), CreateTypeList(TYPE_Real2, TYPE_Real2), real2_Angle);
		REGISTER_FUNCTIONS(true, space, TEXT("Cross"), CreateTypeList(TYPE_Real), CreateTypeList(TYPE_Real2, TYPE_Real2), real2_Cross);
		REGISTER_FUNCTIONS(true, space, TEXT("Dot"), CreateTypeList(TYPE_Real), CreateTypeList(TYPE_Real2, TYPE_Real2), real2_Dot);
		REGISTER_FUNCTIONS(true, space, TEXT("Lerp"), CreateTypeList(TYPE_Real2), CreateTypeList(TYPE_Real2, TYPE_Real2, TYPE_Real), real2_Lerp);
		REGISTER_FUNCTIONS(true, space, TEXT("Max"), CreateTypeList(TYPE_Real2), CreateTypeList(TYPE_Real2, TYPE_Real2), real2_Max);
		REGISTER_FUNCTIONS(true, space, TEXT("Min"), CreateTypeList(TYPE_Real2), CreateTypeList(TYPE_Real2, TYPE_Real2), real2_Min);
		REGISTER_FUNCTIONS(true, space, TEXT("Angle"), CreateTypeList(TYPE_Real), CreateTypeList(TYPE_Real3, TYPE_Real3), real3_Angle);
		REGISTER_FUNCTIONS(true, space, TEXT("Cross"), CreateTypeList(TYPE_Real3), CreateTypeList(TYPE_Real3, TYPE_Real3), real3_Cross);
		REGISTER_FUNCTIONS(true, space, TEXT("Dot"), CreateTypeList(TYPE_Real), CreateTypeList(TYPE_Real3, TYPE_Real3), real3_Dot);
		REGISTER_FUNCTIONS(true, space, TEXT("Lerp"), CreateTypeList(TYPE_Real3), CreateTypeList(TYPE_Real3, TYPE_Real3, TYPE_Real), real3_Lerp);
		REGISTER_FUNCTIONS(true, space, TEXT("Max"), CreateTypeList(TYPE_Real3), CreateTypeList(TYPE_Real3, TYPE_Real3), real3_Max);
		REGISTER_FUNCTIONS(true, space, TEXT("Min"), CreateTypeList(TYPE_Real3), CreateTypeList(TYPE_Real3, TYPE_Real3), real3_Min);
		REGISTER_FUNCTIONS(true, space, TEXT("Angle"), CreateTypeList(TYPE_Real), CreateTypeList(TYPE_Real4, TYPE_Real4), real4_Angle);
		REGISTER_FUNCTIONS(true, space, TEXT("Dot"), CreateTypeList(TYPE_Real), CreateTypeList(TYPE_Real4, TYPE_Real4), real4_Dot);
		REGISTER_FUNCTIONS(true, space, TEXT("Lerp"), CreateTypeList(TYPE_Real4), CreateTypeList(TYPE_Real4, TYPE_Real4, TYPE_Real), real4_Lerp);
		REGISTER_FUNCTIONS(true, space, TEXT("Max"), CreateTypeList(TYPE_Real4), CreateTypeList(TYPE_Real4, TYPE_Real4), real4_Max);
		REGISTER_FUNCTIONS(true, space, TEXT("Min"), CreateTypeList(TYPE_Real4), CreateTypeList(TYPE_Real4, TYPE_Real4), real4_Min);
	}
	//space System
	{
		Space* space = new KernelLibraryInfo::Space(KERNEL_STRING(TEXT("System")), root);
		//space Exceptions
		{
			Space* exceptionSpace = new KernelLibraryInfo::Space(KERNEL_STRING(TEXT("Exceptions")), space);
			REGISTER_VARIABLE(true, exceptionSpace, TEXT("NullReference"), TYPE_String, AddData(EXCEPTION_NULL_REFERENCE));
			REGISTER_VARIABLE(true, exceptionSpace, TEXT("InvalidTask"), TYPE_String, AddData(EXCEPTION_INVALID_TASK));
			REGISTER_VARIABLE(true, exceptionSpace, TEXT("OutOfRange"), TYPE_String, AddData(EXCEPTION_OUT_OF_RANGE));
			REGISTER_VARIABLE(true, exceptionSpace, TEXT("InvalidType"), TYPE_String, AddData(EXCEPTION_INVALID_TYPE));
			REGISTER_VARIABLE(true, exceptionSpace, TEXT("NotEnum"), TYPE_String, AddData(EXCEPTION_NOT_ENUM));
			REGISTER_VARIABLE(true, exceptionSpace, TEXT("NotArray"), TYPE_String, AddData(EXCEPTION_NOT_ARRAY));
			REGISTER_VARIABLE(true, exceptionSpace, TEXT("NotDelegate"), TYPE_String, AddData(EXCEPTION_NOT_DELEGATE));
			REGISTER_VARIABLE(true, exceptionSpace, TEXT("NotTask"), TYPE_String, AddData(EXCEPTION_NOT_TASK));
			REGISTER_VARIABLE(true, exceptionSpace, TEXT("NotDelegateOrTask"), TYPE_String, AddData(EXCEPTION_NOT_DELEGATE_OR_TASK));
			REGISTER_VARIABLE(true, exceptionSpace, TEXT("CantCreateOfThisType"), TYPE_String, AddData(EXCEPTION_CANT_CREATE_OF_THIS_TYPE));
			REGISTER_VARIABLE(true, exceptionSpace, TEXT("TaskNotUnstart"), TYPE_String, AddData(EXCEPTION_TASK_NOT_UNSTART));
			REGISTER_VARIABLE(true, exceptionSpace, TEXT("TaskNotRunning"), TYPE_String, AddData(EXCEPTION_TASK_NOT_RUNNING));
			REGISTER_VARIABLE(true, exceptionSpace, TEXT("TaskNotCompleted"), TYPE_String, AddData(EXCEPTION_TASK_NOT_COMPLETED));
			REGISTER_VARIABLE(true, exceptionSpace, TEXT("DivideByZero"), TYPE_String, AddData(EXCEPTION_DIVIDE_BY_ZERO));
			REGISTER_VARIABLE(true, exceptionSpace, TEXT("InvalidCast"), TYPE_String, AddData(EXCEPTION_INVALID_CAST));
			REGISTER_VARIABLE(true, exceptionSpace, TEXT("StackOverflow"), TYPE_String, AddData(EXCEPTION_STACK_OVERFLOW));
			REGISTER_VARIABLE(true, exceptionSpace, TEXT("AssignmentReadonlyVariable"), TYPE_String, AddData(EXCEPTION_ASSIGNMENT_READONLY_VARIABLE));
			REGISTER_VARIABLE(true, exceptionSpace, TEXT("ParameterListDoesNotMatch"), TYPE_String, AddData(EXCEPTION_PARAMETER_LIST_DOES_NOT_MATCH));
			REGISTER_VARIABLE(true, exceptionSpace, TEXT("ReturnListDoesNotMatch"), TYPE_String, AddData(EXCEPTION_RETURN_LIST_DOES_NOT_MATCH));
			REGISTER_VARIABLE(true, exceptionSpace, TEXT("IgnoreWaitButConditionNotValid"), TYPE_String, AddData(EXCEPTION_IGNORE_WAIT_BUT_CONDITION_NOT_VAILD));
			REGISTER_VARIABLE(true, exceptionSpace, TEXT("IgnoreWaitButTaskNotCompleted"), TYPE_String, AddData(EXCEPTION_IGNORE_WAIT_BUT_TASK_NOT_COMPLETED));
			REGISTER_VARIABLE(true, exceptionSpace, TEXT("AllocHeapMemoryOnGC"), TYPE_String, AddData(EXCEPTION_ALLOC_HEAP_MEMORY_ON_GC));
			REGISTER_VARIABLE(true, exceptionSpace, TEXT("HeapOverflow"), TYPE_String, AddData(EXCEPTION_HEAP_OVERFLOW));
		}

		REGISTER_FUNCTIONS(true, space, TEXT("Collect"), CreateTypeList(TYPE_Integer), CreateTypeList(TYPE_Bool), Collect);
		REGISTER_FUNCTIONS(true, space, TEXT("HeapTotalMemory"), CreateTypeList(TYPE_Integer), TupleInfo_EMPTY, HeapTotalMemory);
		REGISTER_FUNCTIONS(true, space, TEXT("CountHandle"), CreateTypeList(TYPE_Integer), TupleInfo_EMPTY, CountHandle);
		REGISTER_FUNCTIONS(true, space, TEXT("CountTask"), CreateTypeList(TYPE_Integer), TupleInfo_EMPTY, CountTask);
		REGISTER_FUNCTIONS(true, space, TEXT("EntityCount"), CreateTypeList(TYPE_Integer), TupleInfo_EMPTY, EntityCount);
		REGISTER_FUNCTIONS(true, space, TEXT("StringCount"), CreateTypeList(TYPE_Integer), TupleInfo_EMPTY, StringCount);
		REGISTER_FUNCTIONS(true, space, TEXT("SetRandomSeed"), TupleInfo_EMPTY, CreateTypeList(TYPE_Integer), SetRandomSeed);
		REGISTER_FUNCTIONS(true, space, TEXT("LoadAssembly"), CreateTypeList(TYPE_Reflection_Assembly), CreateTypeList(TYPE_String), LoadAssembly);
		REGISTER_FUNCTIONS(true, space, TEXT("GetAssembles"), CreateTypeList(Type(TYPE_Reflection_Assembly, 1)), TupleInfo_EMPTY, GetAssembles);
		REGISTER_FUNCTIONS(true, space, TEXT("GetCurrentTaskInstantID"), CreateTypeList(TYPE_Integer), TupleInfo_EMPTY, GetCurrentTaskInstantID);
		REGISTER_FUNCTIONS(true, space, TEXT("CreateString"), CreateTypeList(TYPE_String), CreateTypeList(Type(TYPE_Char, 1), TYPE_Integer, TYPE_Integer), CreateString);
	}
	//space Collections
	{
		Space* space = new KernelLibraryInfo::Space(KERNEL_STRING(TEXT("Collections")), root);
		//interface Enumerable
		{
			List<Interface::Function, true> memberFunctions = List<Interface::Function, true>(1);
			REGISTER_INTERFACE_FUNCTIONS(TEXT("GetEnumerator"), CreateTypeList(TYPE_Collections_Enumerator), CreateTypeList(TYPE_Collections_Enumerable));
			REGISTER_INTERFACE(true, space, TEXT("Enumerable"), KERNEL_TYPE_INTERFACE_INDEX_Collections_Enumerable, EMPTY_DECLARATIONS);
		}
		//interface Enumerator
		{
			List<Interface::Function, true> memberFunctions = List<Interface::Function, true>(1);
			REGISTER_INTERFACE_FUNCTIONS(TEXT("Next"), CreateTypeList(TYPE_Bool, TYPE_Handle), CreateTypeList(TYPE_Collections_Enumerator));
			REGISTER_INTERFACE(true, space, TEXT("Enumerator"), KERNEL_TYPE_INTERFACE_INDEX_Collections_Enumerator, EMPTY_DECLARATIONS);
		}
		//class ArrayEnumerator
		{
			List<KernelLibraryInfo::Variable> memberVariables = List<KernelLibraryInfo::Variable>(2);
			REGISTER_MEMBER_VARIABLES(false, TEXT("source"), TYPE_Handle, GET_FIELD_OFFSET(CollectionsArrayEnumerator, source));
			REGISTER_MEMBER_VARIABLES(false, TEXT("index"), TYPE_Integer, GET_FIELD_OFFSET(CollectionsArrayEnumerator, index));
			List<uint32, true> memberFunctions = List<uint32, true>(1);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("Next"), CreateTypeList(TYPE_Bool, TYPE_Handle), CreateTypeList(TYPE_Collections_ArrayEnumerator), Collections_ArrayEnumerator_Next);
			REGISTER_CLASS(true, root, TEXT("ArrayEnumerator"), KERNEL_TYPE_CLASS_INDEX_Collections_ArrayEnumerator, TYPE_Handle, CreateDeclarationList(TYPE_Collections_Enumerator), SIZE(CollectionsArrayEnumerator), MEMORY_ALIGNMENT_4, EMPTY_INDICES, memberVariables, memberFunctions);
		}
	}
	//space Reflection
	{
		Space* space = new KernelLibraryInfo::Space(KERNEL_STRING(TEXT("Reflection")), root);
		//enum TypeCode
		{
			List<KernelLibraryInfo::Enum::Element> elements(5);
			REGISIER_ENUM_ELEMENT(TEXT("Invalid"), KERNEL_TYPE_CODE_Invalid, KernelReflectionTypeCode::Invalid);
			REGISIER_ENUM_ELEMENT(TEXT("Struct"), KERNEL_TYPE_CODE_Struct, KernelReflectionTypeCode::Struct);
			REGISIER_ENUM_ELEMENT(TEXT("Bool"), KERNEL_TYPE_CODE_Bool, KernelReflectionTypeCode::Bool);
			REGISIER_ENUM_ELEMENT(TEXT("Byte"), KERNEL_TYPE_CODE_Byte, KernelReflectionTypeCode::Byte);
			REGISIER_ENUM_ELEMENT(TEXT("Char"), KERNEL_TYPE_CODE_Char, KernelReflectionTypeCode::Char);
			REGISIER_ENUM_ELEMENT(TEXT("Integer"), KERNEL_TYPE_CODE_Integer, KernelReflectionTypeCode::Integer);
			REGISIER_ENUM_ELEMENT(TEXT("Real"), KERNEL_TYPE_CODE_Real, KernelReflectionTypeCode::Real);
			REGISIER_ENUM_ELEMENT(TEXT("Real2"), KERNEL_TYPE_CODE_Real2, KernelReflectionTypeCode::Real2);
			REGISIER_ENUM_ELEMENT(TEXT("Real3"), KERNEL_TYPE_CODE_Real3, KernelReflectionTypeCode::Real3);
			REGISIER_ENUM_ELEMENT(TEXT("Real4"), KERNEL_TYPE_CODE_Real4, KernelReflectionTypeCode::Real4);
			REGISIER_ENUM_ELEMENT(TEXT("Enum"), KERNEL_TYPE_CODE_Enum, KernelReflectionTypeCode::Enum);
			REGISIER_ENUM_ELEMENT(TEXT("Type"), KERNEL_TYPE_CODE_Type, KernelReflectionTypeCode::Type);
			REGISIER_ENUM_ELEMENT(TEXT("String"), KERNEL_TYPE_CODE_String, KernelReflectionTypeCode::String);
			REGISIER_ENUM_ELEMENT(TEXT("Entity"), KERNEL_TYPE_CODE_Entity, KernelReflectionTypeCode::Entity);
			REGISIER_ENUM_ELEMENT(TEXT("Handle"), KERNEL_TYPE_CODE_Handle, KernelReflectionTypeCode::Handle);
			REGISIER_ENUM_ELEMENT(TEXT("Interface"), KERNEL_TYPE_CODE_Interface, KernelReflectionTypeCode::Interface);
			REGISIER_ENUM_ELEMENT(TEXT("Delegate"), KERNEL_TYPE_CODE_Delegate, KernelReflectionTypeCode::Delegate);
			REGISIER_ENUM_ELEMENT(TEXT("Task"), KERNEL_TYPE_CODE_Task, KernelReflectionTypeCode::Task);
			REGISIER_ENUM_ELEMENT(TEXT("Array"), KERNEL_TYPE_CODE_Array, KernelReflectionTypeCode::Array);
			REGISIER_ENUM(true, space, TEXT("TypeCode"), KERNEL_TYPE_ENUM_INDEX_TypeCode, elements);
		}
		//class Variable
		{
			List<uint32, true> memberFunctions = List<uint32, true>(7);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("IsPublic"), CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Reflection_Variable), Reflection_Variable_IsPublic);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetAttributes"), CreateTypeList(Type(TYPE_String, 1)), CreateTypeList(TYPE_Reflection_Variable), Reflection_Variable_GetAttributes);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetSpace"), CreateTypeList(TYPE_Reflection_Space), CreateTypeList(TYPE_Reflection_Variable), Reflection_Variable_GetSpace);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetName"), CreateTypeList(TYPE_String), CreateTypeList(TYPE_Reflection_Variable), Reflection_Variable_GetName);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetVariableType"), CreateTypeList(TYPE_Type), CreateTypeList(TYPE_Reflection_Variable), Reflection_Variable_GetVariableType);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetValue"), CreateTypeList(TYPE_Handle), CreateTypeList(TYPE_Reflection_Variable), Reflection_Variable_GetValue);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("SetValue"), TupleInfo_EMPTY, CreateTypeList(TYPE_Reflection_Variable, TYPE_Handle), Reflection_Variable_SetValue);
			REGISTER_CLASS(true, space, TEXT("Variable"), KERNEL_TYPE_CLASS_INDEX_Reflection_Variable, TYPE_Handle, EMPTY_DECLARATIONS, SIZE(::Variable), MEMORY_ALIGNMENT_4, EMPTY_INDICES, EMPTY_VARIABLES, memberFunctions);
		}
		//class MemberConstructor
		{
			List<uint32, true> memberFunctions = List<uint32, true>(5);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("IsPublic"), CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Reflection_MemberConstructor), Reflection_MemberConstructor_IsPublic);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetAttributes"), CreateTypeList(Type(TYPE_String, 1)), CreateTypeList(TYPE_Reflection_MemberConstructor), Reflection_MemberConstructor_GetAttributes);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetDeclaringType"), CreateTypeList(TYPE_Type), CreateTypeList(TYPE_Reflection_MemberConstructor), Reflection_MemberConstructor_GetDeclaringType);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetParameters"), CreateTypeList(Type(TYPE_Type, 1)), CreateTypeList(TYPE_Reflection_MemberConstructor), Reflection_MemberConstructor_GetParameters);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("Invoke"), CreateTypeList(TYPE_Handle), CreateTypeList(TYPE_Reflection_MemberConstructor, Type(TYPE_Handle, 1)), Reflection_MemberConstructor_Invoke);
			REGISTER_CLASS(true, space, TEXT("MemberConstructor"), KERNEL_TYPE_CLASS_INDEX_Reflection_MemberConstructor, TYPE_Handle, EMPTY_DECLARATIONS, SIZE(MemberFunction), MEMORY_ALIGNMENT_4, EMPTY_INDICES, EMPTY_VARIABLES, memberFunctions);
		}
		//class MemberVariable
		{
			List<uint32, true> memberFunctions = List<uint32, true>(7);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("IsPublic"), CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Reflection_MemberVariable), Reflection_MemberVariable_IsPublic);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetAttributes"), CreateTypeList(Type(TYPE_String, 1)), CreateTypeList(TYPE_Reflection_MemberVariable), Reflection_MemberVariable_GetAttributes);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetDeclaringType"), CreateTypeList(TYPE_Type), CreateTypeList(TYPE_Reflection_MemberVariable), Reflection_MemberVariable_GetDeclaringType);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetName"), CreateTypeList(TYPE_String), CreateTypeList(TYPE_Reflection_MemberVariable), Reflection_MemberVariable_GetName);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetVariableType"), CreateTypeList(TYPE_Type), CreateTypeList(TYPE_Reflection_MemberVariable), Reflection_MemberVariable_GetVariableType);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetValue"), CreateTypeList(TYPE_Handle), CreateTypeList(TYPE_Reflection_MemberVariable, TYPE_Handle), Reflection_MemberVariable_GetValue);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("SetValue"), TupleInfo_EMPTY, CreateTypeList(TYPE_Reflection_MemberVariable, TYPE_Handle, TYPE_Handle), Reflection_MemberVariable_SetValue);
			REGISTER_CLASS(true, space, TEXT("MemberVariable"), KERNEL_TYPE_CLASS_INDEX_Reflection_MemberVariable, TYPE_Handle, EMPTY_DECLARATIONS, SIZE(MemberVariable), MEMORY_ALIGNMENT_4, EMPTY_INDICES, EMPTY_VARIABLES, memberFunctions);
		}
		//class MemberFunction
		{
			List<uint32, true> memberFunctions = List<uint32, true>(7);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("IsPublic"), CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Reflection_MemberFunction), Reflection_MemberFunction_IsPublic);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetAttributes"), CreateTypeList(Type(TYPE_String, 1)), CreateTypeList(TYPE_Reflection_MemberFunction), Reflection_MemberFunction_GetAttributes);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetDeclaringType"), CreateTypeList(TYPE_Type), CreateTypeList(TYPE_Reflection_MemberFunction), Reflection_MemberFunction_GetDeclaringType);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetName"), CreateTypeList(TYPE_String), CreateTypeList(TYPE_Reflection_MemberFunction), Reflection_MemberFunction_GetName);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetParameters"), CreateTypeList(Type(TYPE_Type, 1)), CreateTypeList(TYPE_Reflection_MemberFunction), Reflection_MemberFunction_GetParameters);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetReturns"), CreateTypeList(Type(TYPE_Type, 1)), CreateTypeList(TYPE_Reflection_MemberFunction), Reflection_MemberFunction_GetReturns);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("Invoke"), CreateTypeList(Type(TYPE_Handle, 1)), CreateTypeList(TYPE_Reflection_MemberFunction, TYPE_Handle, Type(TYPE_Handle, 1)), Reflection_MemberFunction_Invoke);
			REGISTER_CLASS(true, space, TEXT("MemberFunction"), KERNEL_TYPE_CLASS_INDEX_Reflection_MemberFunction, TYPE_Handle, EMPTY_DECLARATIONS, SIZE(MemberFunction), MEMORY_ALIGNMENT_4, EMPTY_INDICES, EMPTY_VARIABLES, memberFunctions);
		}
		//class Function
		{
			List<uint32, true> memberFunctions = List<uint32, true>(7);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("IsPublic"), CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Reflection_Function), Reflection_Function_IsPublic);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetAttributes"), CreateTypeList(Type(TYPE_String, 1)), CreateTypeList(TYPE_Reflection_Function), Reflection_Function_GetAttributes);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetSpace"), CreateTypeList(TYPE_Reflection_Space), CreateTypeList(TYPE_Reflection_Function), Reflection_Function_GetSpace);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetName"), CreateTypeList(TYPE_String), CreateTypeList(TYPE_Reflection_Function), Reflection_Function_GetName);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetParameters"), CreateTypeList(Type(TYPE_Type, 1)), CreateTypeList(TYPE_Reflection_Function), Reflection_Function_GetParameters);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetReturns"), CreateTypeList(Type(TYPE_Type, 1)), CreateTypeList(TYPE_Reflection_Function), Reflection_Function_GetReturns);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("Invoke"), CreateTypeList(Type(TYPE_Handle, 1)), CreateTypeList(TYPE_Reflection_Function, Type(TYPE_Handle, 1)), Reflection_Function_Invoke);
			REGISTER_CLASS(true, space, TEXT("Function"), KERNEL_TYPE_CLASS_INDEX_Reflection_Function, TYPE_Handle, EMPTY_DECLARATIONS, SIZE(Function), MEMORY_ALIGNMENT_4, EMPTY_INDICES, EMPTY_VARIABLES, memberFunctions);
		}
		//class Native
		{
			List<uint32, true> memberFunctions = List<uint32, true>(7);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("IsPublic"), CreateTypeList(TYPE_Bool), CreateTypeList(TYPE_Reflection_Native), Reflection_Native_IsPublic);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetAttributes"), CreateTypeList(Type(TYPE_String, 1)), CreateTypeList(TYPE_Reflection_Native), Reflection_Native_GetAttributes);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetSpace"), CreateTypeList(TYPE_Reflection_Space), CreateTypeList(TYPE_Reflection_Native), Reflection_Native_GetSpace);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetName"), CreateTypeList(TYPE_String), CreateTypeList(TYPE_Reflection_Native), Reflection_Native_GetName);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetParameters"), CreateTypeList(Type(TYPE_Type, 1)), CreateTypeList(TYPE_Reflection_Native), Reflection_Native_GetParameters);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetReturns"), CreateTypeList(Type(TYPE_Type, 1)), CreateTypeList(TYPE_Reflection_Native), Reflection_Native_GetReturns);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("Invoke"), CreateTypeList(Type(TYPE_Handle, 1)), CreateTypeList(TYPE_Reflection_Native, Type(TYPE_Handle, 1)), Reflection_Native_Invoke);
			REGISTER_CLASS(true, space, TEXT("Native"), KERNEL_TYPE_CLASS_INDEX_Reflection_Native, TYPE_Handle, EMPTY_DECLARATIONS, SIZE(Native), MEMORY_ALIGNMENT_4, EMPTY_INDICES, EMPTY_VARIABLES, memberFunctions);
		}
		//class Space
		{
			List<uint32, true> memberFunctions = List<uint32, true>(9);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetAttributes"), CreateTypeList(Type(TYPE_String, 1)), CreateTypeList(TYPE_Reflection_Space), Reflection_Space_GetAttributes);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetParent"), CreateTypeList(TYPE_Reflection_Space), CreateTypeList(TYPE_Reflection_Space), Reflection_Space_GetParent);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetChildren"), CreateTypeList(Type(TYPE_Reflection_Space, 1)), CreateTypeList(TYPE_Reflection_Space), Reflection_Space_GetChildren);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetAssembly"), CreateTypeList(TYPE_Reflection_Assembly), CreateTypeList(TYPE_Reflection_Space), Reflection_Space_GetAssembly);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetName"), CreateTypeList(TYPE_String), CreateTypeList(TYPE_Reflection_Space), Reflection_Space_GetName);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetVariables"), CreateTypeList(Type(TYPE_Reflection_Variable, 1)), CreateTypeList(TYPE_Reflection_Space), Reflection_Space_GetVariables);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetFunctions"), CreateTypeList(Type(TYPE_Reflection_Function, 1)), CreateTypeList(TYPE_Reflection_Space), Reflection_Space_GetFunctions);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetNatives"), CreateTypeList(Type(TYPE_Reflection_Native, 1)), CreateTypeList(TYPE_Reflection_Space), Reflection_Space_GetNatives);
			REGISTER_MEMBER_FUNCTIONS(true, TEXT("GetTypes"), CreateTypeList(Type(TYPE_Type, 1)), CreateTypeList(TYPE_Reflection_Space), Reflection_Space_GetTypes);
			REGISTER_CLASS(true, space, TEXT("Space"), KERNEL_TYPE_CLASS_INDEX_Reflection_Space, TYPE_Handle, EMPTY_DECLARATIONS, SIZE(ReflectionSpace), MEMORY_ALIGNMENT_4, EMPTY_INDICES, EMPTY_VARIABLES, memberFunctions);
		}
		//class Assembly
		REGISTER_CLASS(true, space, TEXT("Assembly"), KERNEL_TYPE_CLASS_INDEX_Reflection_Assembly, TYPE_Reflection_Space, EMPTY_DECLARATIONS, SIZE(ReflectionSpace), MEMORY_ALIGNMENT_4, EMPTY_INDICES, EMPTY_VARIABLES, EMPTY_INDICES);
	}

	CalculateTupleInfo(*this);
}

uint32 KernelLibraryInfo::AddData(const character* value)
{
	uint32 address = data.Count();
	string result = stringAgency.AddAndRef(value);
	for(uint32 i = 0; i < dataStrings.Count(); i++)
		if(dataStrings[i].value == result)
		{
			dataStrings[i].addresses.Add(address);
			return address;
		}
	dataStrings.Add(StringAddresses(result, address));
	return address;
}
