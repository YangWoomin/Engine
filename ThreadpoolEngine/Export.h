#pragma once


#ifdef THREADPOOLENGINE_EXPORTS
#define THREADPOOLENGINE_API __declspec(dllexport)
#else
#define THREADPOOLENGINE_API __declspec(dllimport)
#endif

// ������Ʈ ���ο��� ����ϴ� ����/���� ���� �ڵ�
// ������Ʈ ���ο��� ������ ���� �ڵ�� ������ ����ϸ� ����� DWORD GetLastError �ڵ�� ���
enum ERROR_CODE : __int64
{
	// ���� �ƴ� (����)
	ERROR_CODE_NONE
	= 0,

	// ���� ���� //
	// �̹� ����
	ERROR_CODE_ALREADY_EXISTS
	= -1,

	// �������� ����
	ERROR_CODE_DOES_NOT_EXIST
	= -2,

	// ��ȿ���� ���� ����
	ERROR_CODE_INVALID_ACCESS
	= -3,

	// CThreadpoolCallbackWork ���� ���� �ڵ� //
	// �ݹ� ��ü�� �̹� ���ε� �Ǿ���
	ERROR_CODE_THREADPOOL_CALLBACK_WORK_ALREADY_BOUND
	= -101,

	// �ݹ� ��ü�� �ݹ� �Լ��� ������
	ERROR_CODE_THREADPOOL_CALLBACK_WORK_CALLBACK_FUNCTION_RUNNING
	= -102,

	// �ݹ� ��ü�� �ݹ� �Լ��� �Ѱ��� �ݹ� �����Ͱ� ���õ��� ����
	ERROR_CODE_THREADPOOL_CALLBACK_WORK_CALLBACK_DATA_NOT_SET
	= -103,

	// �ݹ� ��ü�� ���ε��� ���� ����
	ERROR_CODE_THREADPOOL_CALLBACK_WORK_NOT_BOUND
	= -104,

	// �ݹ� ��ü�� NULL�� ����
	ERROR_CODE_THREADPOOL_CALLBACK_WORK_PTP_WORK_IS_NULL
	= -105,

	// �ݹ� �Լ��� ȣ��Ǳ� ���� �ݹ� ��ü�� ������
	ERROR_CODE_THREADPOOL_CALLBACK_WORK_RELEASE_CALLBACK_OBJECT
	= -106,

	// �ݹ� ��ü ������ ������
	ERROR_CODE_THREADPOOL_CALLBACK_WORK_CREATE_CALLBACK_OBJECT_FAILURE
	= -107,

	// CThreadpoolCallbackIo ���� ���� �ڵ� //
	// �ݹ� ��ü�� �̹� ���ε� �Ǿ���
	ERROR_CODE_THREADPOOL_CALLBACK_IO_ALREADY_BOUND
	= -401,

	// �ݹ� ��ü�� �ݹ� �Լ��� ������
	ERROR_CODE_THREADPOOL_CALLBACK_IO_CALLBACK_FUNCTION_RUNNING
	= -402,

	// �ݹ� ��ü�� �ݹ� �Լ��� �Ѱ��� �ݹ� �����Ͱ� ���õ��� ����
	ERROR_CODE_THREADPOOL_CALLBACK_IO_CALLBACK_DATA_NOT_SET
	= -403,

	// �ݹ� ��ü�� ���ε��� ���� ����
	ERROR_CODE_THREADPOOL_CALLBACK_IO_NOT_BOUND
	= -404,

	// �ݹ� ��ü�� NULL�� ����
	ERROR_CODE_THREADPOOL_CALLBACK_IO_PTP_IO_IS_NULL
	= -405,

	// IOCP�� ���ε��� ��ġ�� ��ȿ���� ����
	ERROR_CODE_THREADPOOL_CALLBACK_IO_INVALID_DEVICE
	= -410,

	// �ݹ� �����Ͱ� �̹� ���õǾ���
	ERROR_CODE_THREADPOOL_CALLBACK_IO_CALLBACK_DATA_ALREADY_SET
	= -411,

	// ������Ǯ ���� ���� �ڵ� //
	// ����Ʈ ������Ǯ�� �ƴ� ������Ǯ�� �ּ� ������ ����(THREADPOOL_THREAD_MIN_COUNT)�� ��ȿ���� ����
	ERROR_CODE_THREADPOOL_INVALID_THREAD_MIN_COUNT
	= -1001,

	// ����Ʈ ������Ǯ�� �ƴ� ������Ǯ�� �ִ� ������ ����(THREADPOOL_THREAD_MAX_COUNT)�� ��ȿ���� ����
	ERROR_CODE_THREADPOOL_INVALID_THREAD_MAX_COUNT
	= -1002,

	// ����Ʈ ������Ǯ�� ��������� ������ �� ����
	ERROR_CODE_THREADPOOL_CANNOT_DESTROY_DEFAULT_THREADPOOL_EXPLICITLY
	= -1003,

	// Ư�� �׷��� ������Ǯ�� �̹� ������ (������ �Ⱦ��� �� ����)
	ERROR_CODE_THREADPOOL_THREADPOOL_GROUP_ALREADY_EXISTS
	= -1004,

	// �ݹ� ��ü�� Ư�� �׷��� ������Ǯ�� �̹� ������
	ERROR_CODE_THREADPOOL_CALLBACK_OBJECT_ALREADY_EXISTS_IN_THREADPOOL_GROUP
	= -1005,
};

struct CALLBACK_DATA_PARAMETER
{
	// ����
	ERROR_CODE							_errorCode;

	// TP_WAIT ����
	TP_WAIT_RESULT						_tpWaitResult;

	// TP_IO ����
	LPOVERLAPPED						_pOverlapped;
	ULONG								_ulIoResult;
	ULONG_PTR							_ulpNumberOfBytesTransferred;

	CALLBACK_DATA_PARAMETER(ERROR_CODE errorCode, TP_WAIT_RESULT tpWaitResult, LPOVERLAPPED pOverlapped, ULONG ulIoResult, ULONG_PTR ulpNumberOfBytesTransferred)
	{
		_errorCode = errorCode;

		_tpWaitResult = tpWaitResult;

		_pOverlapped = pOverlapped;
		_ulIoResult = ulIoResult;
		_ulpNumberOfBytesTransferred = ulpNumberOfBytesTransferred;
	}
};

// �ݹ� ��ü�� �ݹ� �Լ��� ���޵� �ݹ� ������
interface ICallbackData
{
	virtual ~ICallbackData() { }

	// ȣ��� �ݹ� �Լ�
	virtual void CallbackFunction(CALLBACK_DATA_PARAMETER callbackDataParameter) = 0;

	// �ݹ� �Լ� ȣ�� �� �� ��ü�� �ڵ� ���� ����, ü�̴׿� �����ϰ� ����� �� ����
	virtual BOOL DeleteCallbackDataAutomatically() = 0;

};

#define DEFAULT_THREAD_POOL 0
typedef DWORD ThreadpoolGroup; // DEFAULT_THREAD_POOL�� �ƴ� 0���� ū ��
#define THREADPOOL_THREAD_MIN_COUNT 1 // ����Ʈ ������Ǯ�� �ƴ� ������Ǯ�� �ּ� ������ ����
#define THREADPOOL_THREAD_MAX_COUNT 500 // ����Ʈ ������Ǯ�� �ƴ� ������Ǯ�� �ִ� ������ ����

// CreateThreadpool �Ű� ���� ���� ����ü
struct THREADPOOL_GROUP_PARAMETER
{
	ThreadpoolGroup				_dwThreadpoolGroup;
	int							_iMinThreadCount;
	int							_iMaxThreadCount;

	THREADPOOL_GROUP_PARAMETER(ThreadpoolGroup dwThreadpoolGroup, int iMinThreadCount, int iMaxThreadCount)
	{
		_dwThreadpoolGroup = dwThreadpoolGroup;
		_iMinThreadCount = iMinThreadCount;
		_iMaxThreadCount = iMaxThreadCount;
	}
};

class CThreadpoolCallbackWork;
// Work �ݹ� ��ü ���� Ŭ����
class THREADPOOLENGINE_API CThreadpoolCallbackWorkWrapper
{
protected:
	// Work �ݹ� ��ü
	CThreadpoolCallbackWork* _pThreadpoolCallbackWork;

public:
	CThreadpoolCallbackWorkWrapper(THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode);

	~CThreadpoolCallbackWorkWrapper();

	BOOL ExecuteThreadpoolCallbackWorkWrapper(ICallbackData* pCallbackData, ERROR_CODE& errorCode);
};

class CThreadpoolCallbackIo;

template <class C>
class THREADPOOLENGINE_API CThreadpoolCallbackWrapper
{
protected:
	C* _pThreadpoolCallbackObject;
};


// THREADPOOLENGINE_API CThreadpoolCallbackIo* CreateThreadpoolCallbackIo(HANDLE hDevice, THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter);

