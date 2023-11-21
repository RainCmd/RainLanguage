#pragma once
#include "Rain.h"

/// <summary>
/// ����ȼ�
/// </summary>
enum class  ErrorLevel :uint32
{
	Error,
	WarringLevel1,
	WarringLevel2,
	WarringLevel3,
	WarringLevel4,
	LoggerLevel1,
	LoggerLevel2,
	LoggerLevel3,
	LoggerLevel4,
};

/// <summary>
/// ������Ϣ
/// </summary>
enum class MessageType :uint32
{
	ERROR = ((uint32)ErrorLevel::Error) << 24,
	ERROR_MISSING_PAIRED_SYMBOL,			//ȱ����Եķ���
	ERROR_UNKNOWN_SYMBOL,					//δ֪�ķ���
	ERROR_INDENT,							//��������
	ERROR_INPUT_STRINGL,					//�����ַ���
	ERROR_INPUT_COMMA_OR_SEMICOLON,			//���붺�Ż�ֺ�
	ERROR_UNEXPECTED_LEXCAL,				//����Ĵ���
	ERROR_ATTRIBUTE_INVALID,				//������Ч
	ERROR_MISSING_NAME,						//ȱ������
	ERROR_MISSING_TYPE,						//ȱ������
	ERROR_UNEXPECTED_LINE_END,				//�������β
	ERROR_CONSTANT_NOT_ASSIGNMENT,			//����δ��ֵ
	ERROR_MISSING_ASSIGNMENT_EXPRESSION,	//ȱ�ٸ�ֵ���ʽ
	ERROR_MISSING_IDENTIFIER,				//ȱ�ٱ�ʶ��
	ERROR_MISSING_EXPRESSION,				//ȱ�ٱ��ʽ
	ERROR_INVALID_IDENTIFIER,				//��Ч�ı�ʶ��
	ERROR_INVALID_VISIBILITY,				//��Ч�Ŀɷ��������η�
	ERROR_NOT_VARIABLE_DECLARATION,			//���Ǳ�������
	ERROR_INVALID_INITIALIZER,				//��Ч�ĳ�ʼ��
	ERROR_LIBRARY_LOAD_FAIL,				//���򼯼���ʧ��
	ERROR_RELY_DECLARATION_LOAD_FAIL,		//��������������ʧ��
	ERROR_RELY_DECLARATION_AMBIGUITY,		//��������������ȷ
	ERROR_RELY_DECLARATION_MISMATCHING,		//������������ʵ�����Ͳ�ƥ��
	ERROR_IMPORT_SELF,						//���ܵ������ڱ���Ŀ�
	ERROR_IMPORT_NAMESPACE_NOT_FOUND,		//����������ռ�δ�ҵ�
	ERROR_DUPLICATE_DECLARATION,			//�ظ�������
	ERROR_RELY_SPACE_EQUIVOCAL,				//�����ռ䲻��ȷ
	ERROR_DECLARATION_EQUIVOCAL,			//������������ȷ
	ERROR_EXPRESSION_EQUIVOCAL,				//���ʽ���岻��ȷ
	ERROR_TYPE_EQUIVOCAL,					//���Ͳ���ȷ
	ERROR_DECLARATION_NOT_FOUND,			//����δ�ҵ�
	ERROR_ENUM_ELEMENT_NOT_FOUND,			//ö��Ԫ��δ�ҵ�
	ERROR_NOT_TYPE_DECLARATION,				//������������
	ERROR_NAME_IS_KEY_WORD,					//�����ǹؼ���
	ERROR_DUPLICATION_NAME,					//����
	ERROR_NAME_SAME_AS_NAMESPACE,			//�������ռ�ͬ��
	ERROR_INVALID_OVERLOAD,					//��Ч������
	ERROR_INVALID_OVERRIDE,					//��Ч����д
	ERROR_STRUCT_NO_CONSTRUCTOR,			//�ṹ�岻�����й��캯��
	ERROR_STRUCT_CYCLIC_INCLUSION,			//�ṹ��ѭ������
	ERROR_CIRCULAR_INHERITANCE,				//ѭ���̳�
	ERROR_DUPLICATE_INHERITANCE,			//�ظ��̳�
	ERROR_INTERFACE_NOT_IMPLEMENTED,		//�ӿ�δʵ��
	ERROR_IMPLEMENTED_FUNCTION_RETURN_TYPES_INCONSISTENT,		//ʵ�ֵĽӿں�������ֵ���Ͳ�һ��
	ERROR_OVERRIDE_FUNCTION_RETURN_TYPES_INCONSISTENT,			//��д�ĸ��ຯ������ֵ���Ͳ�һ��
	ERROR_TASK_RETURN_TYPES_INCONSISTENT,						//����������ֵ���Ͳ�һ��
	ERROR_DELEGATE_PARAMETER_TYPES_INCONSISTENT,				//ί�в������Ͳ�һ��
	ERROR_DELEGATE_RETURN_TYPES_INCONSISTENT,					//ί�з���ֵ���Ͳ�һ��
	ERROR_INVALID_OPERATOR,					//��Ч�Ĳ���
	ERROR_TYPE_MISMATCH,					//���Ͳ�ƥ��
	ERROR_NOT_HANDLE_TYPE,					//���Ǿ������
	ERROR_NOT_DELEGATE_TYPE,				//����ί������
	ERROR_WRONG_NUMBER_OF_INDICES,			//����������
	ERROR_NOT_MEMBER_METHOD,				//���ڳ�Ա������
	ERROR_METHOD_NOT_FOUND,					//����δ�ҵ�
	ERROR_OPERATOR_NOT_FOUND,				//����δ�ҵ�
	ERROR_CONSTRUCTOR_NOT_FOUND,			//���캯��δ�ҵ�
	ERROR_NUMBER_OF_PARAMETERS,				//������������
	ERROR_DESTRUCTOR_ALLOC,					//���������������й��ڴ�
	ERROR_TUPLE_INDEX_NOT_CONSTANT,			//Ԫ���������ǳ���
	ERROR_INDEX_OUT_OF_RANGE,				//����Խ��
	ERROR_EXPRESSION_NOT_VALUE,				//���ʽ���Ǹ�ֵ
	ERROR_TYPE_CANNOT_BE_NULL,				//���Ͳ���Ϊ��
	ERROR_EXPRESSION_UNASSIGNABLE,			//���ʽ���ɸ�ֵ
	ERROR_DIVISION_BY_ZERO,					//����
	ERROR_GENERATOR_CONSTANT_EVALUATION_FAIL,					//����ֵ����ʧ��
	ERROR_UNSUPPORTED_CONSTANT_TYPES,		//��֧�ֵĳ�������
	ERROR_CONSTRUCTOR_CALL_ITSELF,			//���캯����������
	ERROR_MISSING_RETURN,					//ȱ�ٷ���ֵ
	ERROR_TYPE_NUMBER_ERROR,				//������������
	ERROR_ONLY_BE_USED_IN_LOOP,				//ֻ����ѭ����ʹ��
	ERROR_CANNOT_USE_RETURN_IN_CATCH_AND_FINALLY,				//����ѭ����
	ERROR_NOT_SUPPORTED_CREATION_NATIVE_TASK,					//��֧���ñ��غ�����������
	ERROR_NOT_SUPPORTED_SPECIAL_FUNCTION,						//��֧�ֵ����⺯��

	WARRING_LEVEL1 = ((uint32)ErrorLevel::WarringLevel1) << 24,
	WARRING_LEVEL1_REPEATED_VISIBILITY,		//�ظ��Ŀɷ���������
	WARRING_LEVEL1_DESTRUCTOR_ATTRIBUTES,	//�����������Խ�������
	WARRING_LEVEL1_DESTRUCTOR_VISIBILITY,	//���������Ŀɷ��������λᱻ����

	WARRING_LEVEL2 = ((uint32)ErrorLevel::WarringLevel2) << 24,

	WARRING_LEVEL3 = ((uint32)ErrorLevel::WarringLevel3) << 24,

	WARRING_LEVEL4 = ((uint32)ErrorLevel::WarringLevel4) << 24,

	LOGGER_LEVEL1 = ((uint32)ErrorLevel::LoggerLevel1) << 24,
	LOGGER_LEVEL1_DISCARD_ATTRIBUTE,		//������������
	LOGGER_LEVEL1_REPEATED_ATTRIBUTE,		//�ظ�������

	LOGGER_LEVEL2 = ((uint32)ErrorLevel::LoggerLevel2) << 24,

	LOGGER_LEVEL3 = ((uint32)ErrorLevel::LoggerLevel3) << 24,

	LOGGER_LEVEL4 = ((uint32)ErrorLevel::LoggerLevel4) << 24,
	LOGGER_LEVEL4_MISSING_VISIBILITY,		//�ɷ���������ȱʧ����ʹ��Ĭ�Ͽɷ���������
	LOGGER_LEVEL4_UNTREATED_KERNEL_SPECIAL_FUNCTION,			//δ����ĺ������⺯��
	LOGGER_LEVEL4_DISCARDED_EXPRESSION,		//�����ı��ʽ
	LOGGER_LEVEL4_INACCESSIBLE_STATEMENT,	//�޷����ʵ����

	INVALID = 0xFFFFFFFF
};