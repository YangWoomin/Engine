#pragma once


#ifdef THREADPOOLENGINE_EXPORTS
#define THREADPOOLENGINE_API __declspec(dllexport)
#else
#define THREADPOOLENGINE_API __declspec(dllimport)
#endif

// 프로젝트 내부에서 사용하는 공용/전용 에러 코드
// 프로젝트 내부에서 정의한 에러 코드는 음수만 사용하며 양수는 DWORD GetLastError 코드로 사용
enum ERROR_CODE : __int64
{
	// 에러 아님 (정상)
	ERROR_CODE_NONE
	= 0,

	// 공용 에러 //
	// 이미 존재
	ERROR_CODE_ALREADY_EXISTS
	= -1,

	// 존재하지 않음
	ERROR_CODE_DOES_NOT_EXIST
	= -2,

	// 유효하지 않은 접근
	ERROR_CODE_INVALID_ACCESS
	= -3,

	// CThreadpoolCallbackWork 전용 에러 코드 //
	// 콜백 객체는 이미 바인딩 되었음
	ERROR_CODE_THREADPOOL_CALLBACK_WORK_ALREADY_BOUND
	= -101,

	// 콜백 객체의 콜백 함수가 실행중
	ERROR_CODE_THREADPOOL_CALLBACK_WORK_CALLBACK_FUNCTION_RUNNING
	= -102,

	// 콜백 객체의 콜백 함수로 넘겨질 콜백 데이터가 세팅되지 않음
	ERROR_CODE_THREADPOOL_CALLBACK_WORK_CALLBACK_DATA_NOT_SET
	= -103,

	// 콜백 객체가 바인딩된 적이 없음
	ERROR_CODE_THREADPOOL_CALLBACK_WORK_NOT_BOUND
	= -104,

	// 콜백 객체가 NULL인 상태
	ERROR_CODE_THREADPOOL_CALLBACK_WORK_PTP_WORK_IS_NULL
	= -105,

	// 콜백 함수가 호출되기 전에 콜백 객체가 해제됨
	ERROR_CODE_THREADPOOL_CALLBACK_WORK_RELEASE_CALLBACK_OBJECT
	= -106,

	// 콜백 객체 생성에 실패함
	ERROR_CODE_THREADPOOL_CALLBACK_WORK_CREATE_CALLBACK_OBJECT_FAILURE
	= -107,

	// CThreadpoolCallbackIo 전용 에러 코드 //
	// 콜백 객체는 이미 바인딩 되었음
	ERROR_CODE_THREADPOOL_CALLBACK_IO_ALREADY_BOUND
	= -401,

	// 콜백 객체의 콜백 함수가 실행중
	ERROR_CODE_THREADPOOL_CALLBACK_IO_CALLBACK_FUNCTION_RUNNING
	= -402,

	// 콜백 객체의 콜백 함수로 넘겨질 콜백 데이터가 세팅되지 않음
	ERROR_CODE_THREADPOOL_CALLBACK_IO_CALLBACK_DATA_NOT_SET
	= -403,

	// 콜백 객체가 바인딩된 적이 없음
	ERROR_CODE_THREADPOOL_CALLBACK_IO_NOT_BOUND
	= -404,

	// 콜백 객체가 NULL인 상태
	ERROR_CODE_THREADPOOL_CALLBACK_IO_PTP_IO_IS_NULL
	= -405,

	// IOCP와 바인딩할 장치가 유효하지 않음
	ERROR_CODE_THREADPOOL_CALLBACK_IO_INVALID_DEVICE
	= -410,

	// 콜백 데이터가 이미 세팅되었음
	ERROR_CODE_THREADPOOL_CALLBACK_IO_CALLBACK_DATA_ALREADY_SET
	= -411,

	// 스레드풀 전용 에러 코드 //
	// 디폴트 스레드풀이 아닌 스레드풀의 최소 스레드 개수(THREADPOOL_THREAD_MIN_COUNT)가 유효하지 않음
	ERROR_CODE_THREADPOOL_INVALID_THREAD_MIN_COUNT
	= -1001,

	// 디폴트 스레드풀이 아닌 스레드풀의 최대 스레드 개수(THREADPOOL_THREAD_MAX_COUNT)가 유효하지 않음
	ERROR_CODE_THREADPOOL_INVALID_THREAD_MAX_COUNT
	= -1002,

	// 디폴트 스레드풀은 명시적으로 제거할 수 없음
	ERROR_CODE_THREADPOOL_CANNOT_DESTROY_DEFAULT_THREADPOOL_EXPLICITLY
	= -1003,

	// 특정 그룹의 스레드풀이 이미 존재함 (에러로 안쓰일 수 있음)
	ERROR_CODE_THREADPOOL_THREADPOOL_GROUP_ALREADY_EXISTS
	= -1004,

	// 콜백 객체가 특정 그룹의 스레드풀에 이미 존재함
	ERROR_CODE_THREADPOOL_CALLBACK_OBJECT_ALREADY_EXISTS_IN_THREADPOOL_GROUP
	= -1005,
};

struct CALLBACK_DATA_PARAMETER
{
	// 공통
	ERROR_CODE							_errorCode;

	// TP_WAIT 전용
	TP_WAIT_RESULT						_tpWaitResult;

	// TP_IO 전용
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

// 콜백 객체의 콜백 함수로 전달될 콜백 데이터
interface ICallbackData
{
	virtual ~ICallbackData() { }

	// 호출될 콜백 함수
	virtual void CallbackFunction(CALLBACK_DATA_PARAMETER callbackDataParameter) = 0;

	// 콜백 함수 호출 후 이 객체의 자동 해제 여부, 체이닝에 유용하게 사용할 수 있음
	virtual BOOL DeleteCallbackDataAutomatically() = 0;

};

#define DEFAULT_THREAD_POOL 0
typedef DWORD ThreadpoolGroup; // DEFAULT_THREAD_POOL이 아닌 0보다 큰 값
#define THREADPOOL_THREAD_MIN_COUNT 1 // 디폴트 스레드풀이 아닌 스레드풀의 최수 스레드 개수
#define THREADPOOL_THREAD_MAX_COUNT 500 // 디폴트 스레드풀이 아닌 스레드풀의 최대 스레드 개수

// CreateThreadpool 매개 변수 묶음 구조체
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
// Work 콜백 객체 래핑 클래스
class THREADPOOLENGINE_API CThreadpoolCallbackWorkWrapper
{
protected:
	// Work 콜백 객체
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

