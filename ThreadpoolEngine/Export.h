#pragma once


#ifdef THREADPOOLENGINE_EXPORTS
#define THREADPOOLENGINE_API __declspec(dllexport)
#else
#define THREADPOOLENGINE_API __declspec(dllimport)
#endif

// ������Ʈ ���ο��� ����ϴ� ����/���� ���� �ڵ�
// ������Ʈ ���ο��� ������ ���� �ڵ�� INT_MIN �̸��� ������ ���
// ����� DWORD GetLastError, int WSAGetLastError,  LONG NTSTATUS �ڵ�� ���
enum ERROR_CODE : __int64
{
	// ���� �ƴ� (����)
	ERROR_CODE_NONE
	= 0,

	// ������Ʈ ������ ���� �ڵ� ���ذ�
	ERROR_CODE_BASE
	= INT_MIN, // -2147483647 - 1

	// ���� ���� //
	// �̹� ����
	ERROR_CODE_ALREADY_EXISTS
	= ERROR_CODE_BASE - 1,

	// �������� ����
	ERROR_CODE_DOES_NOT_EXIST
	= ERROR_CODE_BASE - 2,

	// ��ȿ���� ���� ����
	ERROR_CODE_INVALID_ACCESS
	= ERROR_CODE_BASE - 3,

	// CThreadpoolCallbackWork ���� ���� �ڵ� //
	// �ݹ� ��ü�� �̹� ���ε� �Ǿ���
	ERROR_CODE_THREADPOOL_CALLBACK_WORK_ALREADY_BOUND
	= ERROR_CODE_BASE - 101,

	// �ݹ� ��ü�� �ݹ� �Լ��� ������
	ERROR_CODE_THREADPOOL_CALLBACK_WORK_CALLBACK_FUNCTION_RUNNING
	= ERROR_CODE_BASE - 102,

	// �ݹ� ��ü�� �ݹ� �Լ��� �Ѱ��� �ݹ� �����Ͱ� ���õ��� ����
	ERROR_CODE_THREADPOOL_CALLBACK_WORK_CALLBACK_DATA_NOT_SET
	= ERROR_CODE_BASE - 103,

	// �ݹ� ��ü�� ���ε��� ���� ����
	ERROR_CODE_THREADPOOL_CALLBACK_WORK_NOT_BOUND
	= ERROR_CODE_BASE - 104,

	// �ݹ� ��ü�� NULL�� ����
	ERROR_CODE_THREADPOOL_CALLBACK_WORK_PTP_WORK_IS_NULL
	= ERROR_CODE_BASE - 105,

	// �ݹ� �Լ��� ȣ��Ǳ� ���� �ݹ� ��ü�� ������
	ERROR_CODE_THREADPOOL_CALLBACK_WORK_RELEASE_CALLBACK_OBJECT
	= ERROR_CODE_BASE - 106,

	// �ݹ� ��ü ������ ������
	ERROR_CODE_THREADPOOL_CALLBACK_WORK_CREATE_CALLBACK_OBJECT_FAILURE
	= ERROR_CODE_BASE - 107,

	// CThreadpoolCallbackIo ���� ���� �ڵ� //
	// �ݹ� ��ü�� �̹� ���ε� �Ǿ���
	ERROR_CODE_THREADPOOL_CALLBACK_IO_ALREADY_BOUND
	= ERROR_CODE_BASE - 401,

	// �ݹ� ��ü�� �ݹ� �Լ��� ������
	ERROR_CODE_THREADPOOL_CALLBACK_IO_CALLBACK_FUNCTION_RUNNING
	= ERROR_CODE_BASE - 402,

	// �ݹ� ��ü�� �ݹ� �Լ��� �Ѱ��� �ݹ� �����Ͱ� ���õ��� ����
	ERROR_CODE_THREADPOOL_CALLBACK_IO_CALLBACK_DATA_NOT_SET
	= ERROR_CODE_BASE - 403,

	// �ݹ� ��ü�� ���ε��� ���� ����
	ERROR_CODE_THREADPOOL_CALLBACK_IO_NOT_BOUND
	= ERROR_CODE_BASE - 404,

	// �ݹ� ��ü�� NULL�� ����
	ERROR_CODE_THREADPOOL_CALLBACK_IO_PTP_IO_IS_NULL
	= ERROR_CODE_BASE - 405,

	// �ݹ� ��ü ������ ������
	ERROR_CODE_THREADPOOL_CALLBACK_IO_CREATE_CALLBACK_OBJECT_FAILURE
	= ERROR_CODE_BASE - 407,

	// IOCP�� ���ε��� ��ġ�� ��ȿ���� ����
	ERROR_CODE_THREADPOOL_CALLBACK_IO_INVALID_DEVICE
	= ERROR_CODE_BASE - 410,

	// �ݹ� �����Ͱ� �̹� ���õǾ���
	ERROR_CODE_THREADPOOL_CALLBACK_IO_CALLBACK_DATA_ALREADY_SET
	= ERROR_CODE_BASE - 411,

	// ������Ǯ ���� ���� �ڵ� //
	// ����Ʈ ������Ǯ�� �ƴ� ������Ǯ�� �ּ� ������ ����(THREADPOOL_THREAD_MIN_COUNT)�� ��ȿ���� ����
	ERROR_CODE_THREADPOOL_INVALID_THREAD_MIN_COUNT
	= ERROR_CODE_BASE - 1001,

	// ����Ʈ ������Ǯ�� �ƴ� ������Ǯ�� �ִ� ������ ����(THREADPOOL_THREAD_MAX_COUNT)�� ��ȿ���� ����
	ERROR_CODE_THREADPOOL_INVALID_THREAD_MAX_COUNT
	= ERROR_CODE_BASE - 1002,

	// ����Ʈ ������Ǯ�� ��������� ������ �� ����
	ERROR_CODE_THREADPOOL_CANNOT_DESTROY_DEFAULT_THREADPOOL_EXPLICITLY
	= ERROR_CODE_BASE - 1003,

	// Ư�� �׷��� ������Ǯ�� �̹� ������ (������ �Ⱦ��� �� ����)
	ERROR_CODE_THREADPOOL_THREADPOOL_GROUP_ALREADY_EXISTS
	= ERROR_CODE_BASE - 1004,

	// �ݹ� ��ü�� Ư�� �׷��� ������Ǯ�� �̹� ������
	ERROR_CODE_THREADPOOL_CALLBACK_OBJECT_ALREADY_EXISTS_IN_THREADPOOL_GROUP
	= ERROR_CODE_BASE - 1005,
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
#define THREADPOOL_THREAD_MIN_COUNT 1 // ����Ʈ ������Ǯ�� �ƴ� ������Ǯ�� ������ �ּ� ����
#define THREADPOOL_THREAD_MAX_COUNT 500 // ����Ʈ ������Ǯ�� �ƴ� ������Ǯ�� ������ �ִ� ����

// ������Ǯ �׷��� ������ �Ű� ���� ���� ����ü
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
	// �ش� ��ü�� �����ϰ� ���� �� ERROR_CODE�� Ȯ���� ��
	CThreadpoolCallbackWorkWrapper(THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode);

	~CThreadpoolCallbackWorkWrapper();

	// �ݹ� ������ ����� �ݹ� �Լ� ȣ��(�񵿱�) ���� (�۾��� �з����� �ʴ� �̻� ���� �ٷ� ���۵�)
	BOOL ExecuteThreadpoolCallbackWork(ICallbackData* pCallbackData, ERROR_CODE& errorCode);
};

class CThreadpoolCallbackIo;
// Io �ݹ� ��ü ���� Ŭ����
class THREADPOOLENGINE_API CThreadpoolCallbackIoWrapper
{
protected:
	// Io �ݹ� ��ü
	CThreadpoolCallbackIo* _pThreadpoolCallbackIo;

public:
	// �ش� ��ü�� �����ϰ� ���� �� ERROR_CODE�� Ȯ���� ��
	CThreadpoolCallbackIoWrapper(HANDLE hDevice, THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode);

	~CThreadpoolCallbackIoWrapper();

	// ���ε��� �ش� ��ġ�� �񵿱� ����� �Ϸ� �� �ݹ� ������ ����� �ݹ� �Լ��� ȣ��ǵ��� ����
	// �ݵ�� �񵿱� ����� �Լ�(ex WSARecv)�� ȣ���ϱ� ���� �� �Լ��� ���� ȣ���ؾ� ��
	BOOL ExecuteThreadpoolCallbackIo(ERROR_CODE& errorCode);

	// ���� 1ȸ�� �ݹ� �����͸� ������ �� ����
	// �ݹ� ������ ���� ���� ���� �Ǵ� �ϳ��� ��ġ�� �ټ��� �ݹ� ������ ���� ���� ���δ� ���� �ʿ信 ���� ���� ����
	BOOL SetCallbackData(ICallbackData* pCallbackData, ERROR_CODE& errorCode);

	// �񵿱� ������� ����ϰ� ���� �� ���� ��� ���� �ݹ� �Լ� ���� �ϷḦ ���
	// bCancelPendingCallbacks�� �ݹ� �۾� ť�� �ִ� �۾�(�񵿱� ����� �Ϸ�� ���� �ݹ� �Լ� ȣ��)�� ����� �� ����
	BOOL CancelThreadpoolCallbackIo(BOOL bCancelPendingCallbacks, ERROR_CODE& errorCode);
};


// ���� Ŭ�������� �ϳ��� ���� �� �����..
template <class C>
class THREADPOOLENGINE_API CThreadpoolCallbackWrapper
{
protected:
	C* _pThreadpoolCallbackObject;
};

