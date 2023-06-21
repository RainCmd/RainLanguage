#pragma once
#include "VirtualMachineDefinitions.h"
#include "Vector.h"
#include "RainLibrary.h"

/// <summary>
/// ��ջ����
/// </summary>
struct RAINLANGUAGE RainStackFrame
{
	/// <summary>
	/// ����
	/// </summary>
	const character* libraryName;
	/// <summary>
	/// ��������
	/// </summary>
	uint32 libraryNameLength;
	/// <summary>
	/// �Ȿ�ش���ε�ַ
	/// </summary>
	uint32 address;

	RainStackFrame(const character* libraryName, const uint32& libraryNameLength, const uint32& address)
		: libraryName(libraryName), libraryNameLength(libraryNameLength), address(address) {}
};

class Invoker;
/// <summary>
/// c++��������
/// </summary>
class RAINLANGUAGE InvokerWrapper
{
private:
	uint64 instanceID;
	Invoker* invoker;
	void ValidAssert() const;
public:
	InvokerWrapper();
	InvokerWrapper(Invoker* invoker);
	InvokerWrapper(const InvokerWrapper& other);
	InvokerWrapper(InvokerWrapper&& other) noexcept;
	~InvokerWrapper();

	/// <summary>
	/// ��ȡ����ʵ��ID
	/// </summary>
	/// <returns>���õ�ʵ��ID</returns>
	uint64 GetInstanceID() const;
	/// <summary>
	/// �ж��Ƿ���һ����Ч�ĵ���
	/// </summary>
	/// <returns>����Ч�ĵ���</returns>
	bool IsValid() const;
	/// <summary>
	/// ��ȡ��ǰ����״̬
	/// </summary>
	/// <returns>��ǰ����״̬</returns>
	InvokerState GetState() const;
	/// <summary>
	/// ��ȡ�쳣�˳�ʱ���˳���Ϣ
	/// </summary>
	/// <param name="length">��Ϣ����</param>
	/// <returns>��Ϣ</returns>
	/// <exception>�����������Ч״̬�����쳣</exception>
	const character* GetExitMessage(uint32& length) const;
	/// <summary>
	/// ��ʼִ��Э��
	/// </summary>
	/// <param name="immediately">true:����ִ�� false:�´�Updateִ��</param>
	/// <param name="ignoreWait">����������wait�ؼ���</param>
	void Start(bool immediately, bool ignoreWait) const;
	/// <summary>
	/// �жϵ�ǰ�����Ƿ�����ͣ״̬
	/// </summary>
	/// <returns>����ͣ״̬</returns>
	/// <exception>������ò�������״̬�����쳣</exception>
	bool IsPause() const;
	/// <summary>
	/// ��ͣ��ǰ����
	/// </summary>
	/// <exception>������ò�������״̬�����쳣</exception>
	void Pause() const;
	/// <summary>
	/// �ָ���ǰ����
	/// </summary>
	/// <exception>������ò�������״̬�����쳣</exception>
	void Resume() const;
	/// <summary>
	/// �����쳣
	/// </summary>
	/// <param name="chars">�쳣��Ϣ</param>
	/// <param name="length">�쳣��Ϣ����</param>
	void Abort(const character* chars, uint32 length) const;

	/// <summary>
	/// ��ȡ�������͵ķ���ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <returns>����ֵ</returns>
	/// <exception>������ò��������״̬�򷵻�ֵ���Ͳ���ȷ�����쳣</exception>
	bool GetBoolReturnValue(uint32 index) const;
	/// <summary>
	/// ��ȡ�ֽ����͵ķ���ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <returns>����ֵ</returns>
	/// <exception>������ò��������״̬�򷵻�ֵ���Ͳ���ȷ�����쳣</exception>
	uint8 GetByteReturnValue(uint32 index) const;
	/// <summary>
	/// ��ȡ�ַ����͵ķ���ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <returns>����ֵ</returns>
	/// <exception>������ò��������״̬�򷵻�ֵ���Ͳ���ȷ�����쳣</exception>
	character GetCharReturnValue(uint32 index) const;
	/// <summary>
	/// ��ȡ�������͵ķ���ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <returns>����ֵ</returns>
	/// <exception>������ò��������״̬�򷵻�ֵ���Ͳ���ȷ�����쳣</exception>
	integer GetIntegerReturnValue(uint32 index) const;
	/// <summary>
	/// ��ȡʵ�����͵ķ���ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <returns>����ֵ</returns>
	/// <exception>������ò��������״̬�򷵻�ֵ���Ͳ���ȷ�����쳣</exception>
	real GetRealReturnValue(uint32 index) const;
	/// <summary>
	/// ��ȡ��ά�������͵ķ���ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <returns>����ֵ</returns>
	/// <exception>������ò��������״̬�򷵻�ֵ���Ͳ���ȷ�����쳣</exception>
	Real2 GetReal2ReturnValue(uint32 index) const;
	/// <summary>
	/// ��ȡ��ά�������͵ķ���ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <returns>����ֵ</returns>
	/// <exception>������ò��������״̬�򷵻�ֵ���Ͳ���ȷ�����쳣</exception>
	Real3 GetReal3ReturnValue(uint32 index) const;
	/// <summary>
	/// ��ȡ��ά�������͵ķ���ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <returns>����ֵ</returns>
	/// <exception>������ò��������״̬�򷵻�ֵ���Ͳ���ȷ�����쳣</exception>
	Real4 GetReal4ReturnValue(uint32 index) const;
	/// <summary>
	/// ��ȡö�����͵�����ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <returns>ö�����͵�����ֵ</returns>
	/// <exception>������ò��������״̬�򷵻�ֵ���Ͳ���ȷ�����쳣</exception>
	integer GetEnumValueReturnValue(uint32 index) const;
	/// <summary>
	/// ��ȡö�����Ͷ�Ӧ��Ԫ������
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <param name="length">ö�����Ͷ�Ӧ��Ԫ�����Ƴ���</param>
	/// <returns>ö�����Ͷ�Ӧ��Ԫ������</returns>
	/// <exception>������ò��������״̬�򷵻�ֵ���Ͳ���ȷ�����쳣</exception>
	const character* GetEnumNameReturnValue(uint32 index, uint32& length) const;
	/// <summary>
	/// ��ȡ�ַ������͵ķ���ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <param name="length">�ַ�������</param>
	/// <returns>�ַ���</returns>
	/// <exception>������ò��������״̬�򷵻�ֵ���Ͳ���ȷ�����쳣</exception>
	const character* GetStringReturnValue(uint32 index, uint32& length) const;
	/// <summary>
	/// ��ȡʵ�����͵ķ���ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <returns>����ֵ</returns>
	/// <exception>������ò��������״̬�򷵻�ֵ���Ͳ���ȷ�����쳣</exception>
	uint64 GetEntityReturnValue(uint32 index) const;

	/// <summary>
	/// ���ò������Ͳ���
	/// </summary>
	/// <param name="index">��������</param>
	/// <param name="value">����ֵ</param>
	/// <exception>������ò���δ����״̬��������Ͳ���ȷ�����쳣</exception>
	void SetParameter(uint32 index, bool value) const;
	/// <summary>
	/// �����ֽ����Ͳ���
	/// </summary>
	/// <param name="index">��������</param>
	/// <param name="value">����ֵ</param>
	/// <exception>������ò���δ����״̬��������Ͳ���ȷ�����쳣</exception>
	void SetParameter(uint32 index, uint8 value) const;
	/// <summary>
	/// �����ַ����Ͳ���
	/// </summary>
	/// <param name="index">��������</param>
	/// <param name="value">����ֵ</param>
	/// <exception>������ò���δ����״̬��������Ͳ���ȷ�����쳣</exception>
	void SetParameter(uint32 index, character value) const;
	/// <summary>
	/// �����������Ͳ���
	/// </summary>
	/// <param name="index">��������</param>
	/// <param name="value">����ֵ</param>
	/// <exception>������ò���δ����״̬��������Ͳ���ȷ�����쳣</exception>
	void SetParameter(uint32 index, integer value) const;
	/// <summary>
	/// ����ʵ�����Ͳ���
	/// </summary>
	/// <param name="index">��������</param>
	/// <param name="value">����ֵ</param>
	/// <exception>������ò���δ����״̬��������Ͳ���ȷ�����쳣</exception>
	void SetParameter(uint32 index, real value) const;
	/// <summary>
	/// ���ö�ά�������Ͳ���
	/// </summary>
	/// <param name="index">��������</param>
	/// <param name="value">����ֵ</param>
	/// <exception>������ò���δ����״̬��������Ͳ���ȷ�����쳣</exception>
	void SetParameter(uint32 index, Real2 value) const;
	/// <summary>
	/// ������ά�������Ͳ���
	/// </summary>
	/// <param name="index">��������</param>
	/// <param name="value">����ֵ</param>
	/// <exception>������ò���δ����״̬��������Ͳ���ȷ�����쳣</exception>
	void SetParameter(uint32 index, Real3 value) const;
	/// <summary>
	/// ������ά�������Ͳ���
	/// </summary>
	/// <param name="index">��������</param>
	/// <param name="value">����ֵ</param>
	/// <exception>������ò���δ����״̬��������Ͳ���ȷ�����쳣</exception>
	void SetParameter(uint32 index, Real4 value) const;
	/// <summary>
	/// ���ð�ö��Ԫ��������ö��ֵ
	/// </summary>
	/// <param name="index">��������</param>
	/// <param name="chars">�����ַ���</param>
	/// <param name="length">�����ַ�������</param>
	/// <exception>������ò���δ����״̬��������Ͳ���ȷ������δ�ҵ������쳣</exception>
	void SetEnumNameParameter(uint32 index, const character* chars, uint32 length) const;
	/// <summary>
	/// ���ð�ö��Ԫ��������ö��ֵ��������Ҫ��\0��β
	/// </summary>
	/// <param name="index">��������</param>
	/// <param name="chars">�����ַ���</param>
	/// <exception>������ò���δ����״̬��������Ͳ���ȷ������δ�ҵ������쳣</exception>
	void SetEnumNameParameter(uint32 index, const character* chars) const;
	/// <summary>
	/// ��ö������ֵ���ò���
	/// </summary>
	/// <param name="index">��������</param>
	/// <param name="value">����ֵ</param>
	/// <exception>������ò���δ����״̬��������Ͳ���ȷ�����쳣</exception>
	void SetEnumValueParameter(uint32 index, integer value) const;
	/// <summary>
	/// �����ַ������Ͳ���
	/// </summary>
	/// <param name="index">��������</param>
	/// <param name="chars">�ַ���</param>
	/// <param name="length">�ַ�������</param>
	/// <exception>������ò���δ����״̬��������Ͳ���ȷ�����쳣</exception>
	void SetParameter(uint32 index, const character* chars, uint32 length) const;
	/// <summary>
	/// �����ַ������Ͳ������ַ�����Ҫ��\0��β
	/// </summary>
	/// <param name="index">��������</param>
	/// <param name="chars">�ַ���</param>
	/// <exception>������ò���δ����״̬��������Ͳ���ȷ�����쳣</exception>
	void SetParameter(uint32 index, const character* chars) const;
	/// <summary>
	/// ����ʵ�����Ͳ���
	/// </summary>
	/// <param name="index">��������</param>
	/// <param name="value">����ֵ</param>
	/// <exception>������ò���δ����״̬��������Ͳ���ȷ�����쳣</exception>
	void SetEntityParameter(uint32 index, uint64 value) const;
};

/// <summary>
/// ���Զ�c++�ĵ���
/// </summary>
class RAINLANGUAGE CallerWrapper
{
public:
	CallerWrapper() = default;
	virtual ~CallerWrapper() {}

	/// <summary>
	/// ��ȡ��������ֵ
	/// </summary>
	/// <param name="index">��������</param>
	/// <returns>����ֵ</returns>
	/// <exception>����������Ͳ���ȷ�����쳣</exception>
	virtual bool GetBoolParameter(uint32 index) const = 0;
	/// <summary>
	/// ��ȡ�ֽڲ���ֵ
	/// </summary>
	/// <param name="index">��������</param>
	/// <returns>����ֵ</returns>
	/// <exception>����������Ͳ���ȷ�����쳣</exception>
	virtual uint8 GetByteParameter(uint32 index) const = 0;
	/// <summary>
	/// ��ȡ�ַ�����ֵ
	/// </summary>
	/// <param name="index">��������</param>
	/// <returns>����ֵ</returns>
	/// <exception>����������Ͳ���ȷ�����쳣</exception>
	virtual character GetCharParameter(uint32 index) const = 0;
	/// <summary>
	/// ��ȡ��������ֵ
	/// </summary>
	/// <param name="index">��������</param>
	/// <returns>����ֵ</returns>
	/// <exception>����������Ͳ���ȷ�����쳣</exception>
	virtual integer GetIntegerParameter(uint32 index) const = 0;
	/// <summary>
	/// ��ȡʵ������ֵ
	/// </summary>
	/// <param name="index">��������</param>
	/// <returns>����ֵ</returns>
	/// <exception>����������Ͳ���ȷ�����쳣</exception>
	virtual real GetRealParameter(uint32 index) const = 0;
	/// <summary>
	/// ��ȡ��ά��������ֵ
	/// </summary>
	/// <param name="index">��������</param>
	/// <returns>����ֵ</returns>
	/// <exception>����������Ͳ���ȷ�����쳣</exception>
	virtual Real2 GetReal2Parameter(uint32 index) const = 0;
	/// <summary>
	/// ��ȡ��ά��������ֵ
	/// </summary>
	/// <param name="index">��������</param>
	/// <returns>����ֵ</returns>
	/// <exception>����������Ͳ���ȷ�����쳣</exception>
	virtual Real3 GetReal3Parameter(uint32 index) const = 0;
	/// <summary>
	/// ��ȡ��ά��������ֵ
	/// </summary>
	/// <param name="index">��������</param>
	/// <returns>����ֵ</returns>
	/// <exception>����������Ͳ���ȷ�����쳣</exception>
	virtual Real4 GetReal4Parameter(uint32 index) const = 0;
	/// <summary>
	/// ��ȡö�ٲ�����
	/// </summary>
	/// <param name="index">��������</param>
	/// <param name="length">ö�ٲ���������</param>
	/// <returns>ö�ٲ�����</returns>
	/// <exception>����������Ͳ���ȷ�����쳣</exception>
	virtual const character* GetEnumNameParameter(uint32 index, uint32& length) const = 0;
	/// <summary>
	/// ��ȡö�ٲ�������ֵ
	/// </summary>
	/// <param name="index">��������</param>
	/// <returns>����ֵ</returns>
	/// <exception>����������Ͳ���ȷ�����쳣</exception>
	virtual integer GetEnumValueParameter(uint32 index) const = 0;
	/// <summary>
	/// ��ȡ�ַ�������ֵ
	/// </summary>
	/// <param name="index">��������</param>
	/// <param name="length">�ַ�������</param>
	/// <returns>�ַ���</returns>
	/// <exception>����������Ͳ���ȷ�����쳣</exception>
	virtual const character* GetStringParameter(uint32 index, uint32& length) const = 0;
	/// <summary>
	/// ��ȡʵ�����
	/// </summary>
	/// <param name="index">��������</param>
	/// <returns>����ֵ</returns>
	/// <exception>����������Ͳ���ȷ�����쳣</exception>
	virtual uint64 GetEntityParameter(uint32 index) const = 0;

	/// <summary>
	/// ���ò�������ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <param name="value">����ֵ</param>
	/// <exception>�������ֵ���Ͳ���ȷ�����쳣</exception>
	virtual void SetReturnValue(uint32 index, bool value) = 0;
	/// <summary>
	/// �����ֽڷ���ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <param name="value">����ֵ</param>
	/// <exception>�������ֵ���Ͳ���ȷ�����쳣</exception>
	virtual void SetReturnValue(uint32 index, uint8 value) = 0;
	/// <summary>
	/// �����ַ�����ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <param name="value">����ֵ</param>
	/// <exception>�������ֵ���Ͳ���ȷ�����쳣</exception>
	virtual void SetReturnValue(uint32 index, character value) = 0;
	/// <summary>
	/// ������������ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <param name="value">����ֵ</param>
	/// <exception>�������ֵ���Ͳ���ȷ�����쳣</exception>
	virtual void SetReturnValue(uint32 index, integer value) = 0;
	/// <summary>
	/// ����ʵ������ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <param name="value">����ֵ</param>
	/// <exception>�������ֵ���Ͳ���ȷ�����쳣</exception>
	virtual void SetReturnValue(uint32 index, real value) = 0;
	/// <summary>
	/// ���ö�ά��������ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <param name="value">����ֵ</param>
	/// <exception>�������ֵ���Ͳ���ȷ�����쳣</exception>
	virtual void SetReturnValue(uint32 index, Real2 value) = 0;
	/// <summary>
	/// ������ά��������ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <param name="value">����ֵ</param>
	/// <exception>�������ֵ���Ͳ���ȷ�����쳣</exception>
	virtual void SetReturnValue(uint32 index, Real3 value) = 0;
	/// <summary>
	/// ������ά��������ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <param name="value">����ֵ</param>
	/// <exception>�������ֵ���Ͳ���ȷ�����쳣</exception>
	virtual void SetReturnValue(uint32 index, Real4 value) = 0;
	/// <summary>
	/// ��ö��Ԫ�������÷���ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <param name="chars">ö��Ԫ�����ַ���</param>
	/// <param name="length">ö��Ԫ�����ַ�������</param>
	/// <exception>�������ֵ���Ͳ���ȷ��Ԫ����δ�ҵ������쳣</exception>
	virtual void SetEnumNameReturnValue(uint32 index, const character* chars, uint32 length) = 0;
	/// <summary>
	/// ��ö��Ԫ�������÷���ֵ��������Ҫ��\0��β
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <param name="chars">ö��Ԫ�����ַ���</param>
	/// <exception>�������ֵ���Ͳ���ȷ��Ԫ����δ�ҵ������쳣</exception>
	virtual void SetEnumNameReturnValue(uint32 index, const character* chars) = 0;
	/// <summary>
	/// ����ö�ٷ���ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <param name="value">����ֵ</param>
	/// <exception>�������ֵ���Ͳ���ȷ�����쳣</exception>
	virtual void SetEnumValueReturnValue(uint32 index, integer value) = 0;
	/// <summary>
	/// �����ַ�������ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <param name="chars">�ַ���</param>
	/// <param name="length">�ַ�������</param>
	/// <exception>�������ֵ���Ͳ���ȷ�����쳣</exception>
	virtual void SetReturnValue(uint32 index, const character* chars, uint32 length) = 0;
	/// <summary>
	/// �����ַ�������ֵ���ַ�����Ҫ��\0��β
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <param name="chars">�ַ���</param>
	/// <exception>�������ֵ���Ͳ���ȷ�����쳣</exception>
	virtual void SetReturnValue(uint32 index, const character* chars) = 0;
	/// <summary>
	/// ����ʵ�巵��ֵ
	/// </summary>
	/// <param name="index">����ֵ����</param>
	/// <param name="value">����ֵ</param>
	/// <exception>�������ֵ���Ͳ���ȷ�����쳣</exception>
	virtual void SetEntityReturnValue(uint32 index, uint64 value) = 0;

	/// <summary>
	/// �����쳣��Ϣ
	/// </summary>
	/// <param name="chars">�쳣��Ϣ�ַ��� </param>
	/// <param name="length">�쳣��Ϣ�ַ�������</param>
	virtual void SetException(const character* chars, uint32 length) = 0;
};

/// <summary>
/// �������������
/// </summary>
struct RAINLANGUAGE StartupParameter
{
	/// <summary>
	/// �������صĿ��б�
	/// </summary>
	RainLibrary* libraries;
	/// <summary>
	/// �������صĿ�����
	/// </summary>
	uint32 libraryCount;
	/// <summary>
	/// �������
	/// </summary>
	integer seed;
	/// <summary>
	/// �ַ��������ʼ�ݻ�
	/// </summary>
	uint32 stringCapacity;
	/// <summary>
	/// ʵ������ʼ�ݻ�
	/// </summary>
	uint32 entityCapacity;
	/// <summary>
	/// ��������ú��ͷ�ʵ��ʱ�Ļص�����
	/// </summary>
	EntityAction onReferenceEntity, onReleaseEntity;
	/// <summary>
	/// �������
	/// </summary>
	LibraryLoader libraryLoader;
	/// <summary>
	/// ���ص��õļ�����
	/// </summary>
	NativeCallerLoader nativeCallerLoader;
	/// <summary>
	/// �йܶѳ�ʼ�ݻ���������������
	/// </summary>
	uint32 heapCapacity, heapGeneration;
	/// <summary>
	/// Э�̳�ʼ�ݻ�
	/// </summary>
	uint32 coroutineCapacity;
	/// <summary>
	/// ִ��ջ��ʼ�ݻ�
	/// </summary>
	uint32 executeStackCapacity;
	/// <summary>
	/// Э���쳣�Ļص�����
	/// </summary>
	OnExceptionExit onExceptionExit;

	StartupParameter(RainLibrary* libraries, const uint32& libraryCount, const integer& seed, const uint32& stringCapacity, const uint32& entityCapacity, const EntityAction& onReferenceEntity, const EntityAction& onReleaseEntity, const LibraryLoader& libraryLoader, const NativeCallerLoader& nativeCallerLoader, const uint32& heapCapacity, const uint32& heapGeneration, const uint32& coroutineCapacity, const uint32& executeStackCapacity, const OnExceptionExit& onExceptionExit)
		: libraries(libraries), libraryCount(libraryCount), seed(seed), stringCapacity(stringCapacity), entityCapacity(entityCapacity), onReferenceEntity(onReferenceEntity), onReleaseEntity(onReleaseEntity), libraryLoader(libraryLoader), nativeCallerLoader(nativeCallerLoader), heapCapacity(heapCapacity), heapGeneration(heapGeneration), coroutineCapacity(coroutineCapacity), executeStackCapacity(executeStackCapacity), onExceptionExit(onExceptionExit) {}
};

/// <summary>
/// ���Եĺ������
/// </summary>
struct RAINLANGUAGE RainFunction
{
private:
	uint32 library;
	uint32 function;
public:
	/// <summary>
	/// �ж��Ƿ��Ǹ���Ч�ľ��
	/// </summary>
	/// <returns>�Ǹ���Ч�ľ��</returns>
	inline bool IsValid() { return library != 0xFFFFFFFF; }
};

/// <summary>
/// ���������
/// </summary>
class RAINLANGUAGE RainKernel
{
public:
	RainKernel() = default;
	virtual ~RainKernel() {};

	/// <summary>
	/// ����һ������
	/// </summary>
	/// <param name="function">�������</param>
	/// <returns>����</returns>
	virtual InvokerWrapper CreateInvoker(const RainFunction& function) = 0;
	/// <summary>
	/// ���Һ�����ֻ�ܲ���ȫ�ֵĹ���������ʹ��'.'�ָ��������ռ����ͺ�������û��'.'�ָ������������Ѽ��ؿ�Ĺ���ȫ�ֺ�����ƥ���һ��������ȵĺ���
	/// ����ʧ�ܻ᷵��һ����Ч�ľ��
	/// </summary>
	/// <param name="name">������</param>
	/// <param name="nameLength">����������</param>
	/// <returns>�������</returns>
	virtual const RainFunction FindFunction(const character* name, uint32 nameLength) = 0;
	/// <summary>
	/// ���Һ�����ֻ�ܲ���ȫ�ֵĹ���������ʹ��'.'�ָ��������ռ����ͺ�������û��'.'�ָ������������Ѽ��ؿ�Ĺ���ȫ�ֺ�����ƥ���һ��������ȵĺ���
	/// ����ʧ�ܻ᷵��һ����Ч�ľ��
	/// </summary>
	/// <param name="name">������</param>
	/// <returns>�������</returns>
	virtual const RainFunction FindFunction(const character* name) = 0;
	/// <summary>
	/// ���Һ�����ֻ�ܲ���ȫ�ֵĹ���������ʹ��'.'�ָ��������ռ����ͺ�������û��'.'�ָ������������Ѽ��ؿ�Ĺ���ȫ�ֺ�����ƥ������������ȵĺ���
	/// ����ʧ�ܻ᷵��һ����Ч�ľ��
	/// </summary>
	/// <param name="name">������</param>
	/// <param name="nameLength">����������</param>
	/// <returns>�������</returns>
	virtual const RainFunction* FindFunctions(const character* name, uint32 nameLength, uint32& count) = 0;
	/// <summary>
	/// ���Һ�����ֻ�ܲ���ȫ�ֵĹ���������ʹ��'.'�ָ��������ռ����ͺ�������û��'.'�ָ������������Ѽ��ؿ�Ĺ���ȫ�ֺ�����ƥ������������ȵĺ���
	/// ����ʧ�ܻ᷵��һ����Ч�ľ��
	/// </summary>
	/// <param name="name">������</param>
	/// <returns>�������</returns>
	virtual const RainFunction* FindFunctions(const character* name, uint32& count) = 0;

	/// <summary>
	/// ���������
	/// </summary>
	virtual void Update() = 0;
};


RAINLANGUAGE RainKernel* CreateKernel(const StartupParameter& parameter);
