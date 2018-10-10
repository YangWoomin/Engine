#pragma once


#ifdef THREADPOOLENGINE_EXPORTS
#define THREADPOOLENGINE_API __declspec(dllexport)
#else
#define THREADPOOLENGINE_API __declspec(dllimport)
#endif

namespace ThreadpoolEngine
{
	// 프로젝트 내부에서 사용하는 공용/전용 에러 코드
	// 프로젝트 내부에서 정의한 에러 코드는 INT_MIN 미만의 음수만 사용
	// 양수는 DWORD GetLastError, int WSAGetLastError,  LONG NTSTATUS 코드로 사용
	enum ERROR_CODE : __int64
	{
		// 에러 아님 (정상)
		ERROR_CODE_NONE
		= 0,

		// 프로젝트 내부의 에러 코드 기준값
		ERROR_CODE_BASE
		= INT_MIN, // -2147483647 - 1

		// 공용 에러 //
		// 이미 존재
		ERROR_CODE_ALREADY_EXISTS
		= ERROR_CODE_BASE - 1,

		// 존재하지 않음
		ERROR_CODE_DOES_NOT_EXIST
		= ERROR_CODE_BASE - 2,

		// 유효하지 않은 접근
		ERROR_CODE_INVALID_ACCESS
		= ERROR_CODE_BASE - 3,

		// new 메모리 할당을 실패
		ERROR_CODE_NEW_ALLOCATION_FAILED
		= ERROR_CODE_BASE - 4,


		// 콜백 객체 관련 에러 코드 //
		// 콜백 객체가 이미 바인딩 되었음
		ERROR_CODE_THREADPOOL_CALLBACK_OBJECT_ALREADY_BOUND
		= ERROR_CODE_BASE - 101,

		// 콜백 객체의 콜백 함수가 실행중
		/*ERROR_CODE_THREADPOOL_CALLBACK_WORK_CALLBACK_FUNCTION_RUNNING
		= ERROR_CODE_BASE - 102,*/

		// 콜백 데이터가 이미 세팅되었음
		ERROR_CODE_THREADPOOL_CALLBACK_DATA_ALREADY_SET
		= ERROR_CODE_BASE - 102,

		// 콜백 객체의 콜백 함수로 넘겨질 콜백 데이터가 세팅되지 않음
		ERROR_CODE_THREADPOOL_CALLBACK_DATA_NOT_SET
		= ERROR_CODE_BASE - 103,

		// 콜백 객체가 바인딩되지 않음
		ERROR_CODE_THREADPOOL_CALLBACK_OBJECT_NOT_BOUND
		= ERROR_CODE_BASE - 104,

		// 콜백 객체가 NULL인 상태
		ERROR_CODE_THREADPOOL_CALLBACK_OBJECT_IS_NULL
		= ERROR_CODE_BASE - 105,

		// 콜백 함수가 호출되기 전에 콜백 객체가 해제됨
		/*ERROR_CODE_THREADPOOL_CALLBACK_WORK_RELEASE_CALLBACK_OBJECT
		= ERROR_CODE_BASE - 106,*/

		// 콜백 객체 생성에 실패함
		ERROR_CODE_THREADPOOL_CREATE_CALLBACK_OBJECT_FAILURE
		= ERROR_CODE_BASE - 107,

		// CThreadpoolCallbackWait 전용 에러 코드 //
		// 콜백 객체는 이미 바인딩 되었음
		/*ERROR_CODE_THREADPOOL_CALLBACK_OBJECT_ALREADY_BOUND
		= ERROR_CODE_BASE - 201,*/

		// CThreadpoolCallbackIo 전용 에러 코드 //
		// 콜백 객체는 이미 바인딩 되었음
		/*ERROR_CODE_THREADPOOL_CALLBACK_OBJECT_ALREADY_BOUND
		= ERROR_CODE_BASE - 401,*/

		// 콜백 객체의 콜백 함수가 실행중
		/*ERROR_CODE_THREADPOOL_CALLBACK_IO_CALLBACK_FUNCTION_RUNNING
		= ERROR_CODE_BASE - 402,*/

		// 콜백 객체의 콜백 함수로 넘겨질 콜백 데이터가 세팅되지 않음
		/*ERROR_CODE_THREADPOOL_CALLBACK_DATA_NOT_SET
		= ERROR_CODE_BASE - 403,*/

		// 콜백 객체가 바인딩된 적이 없음
		/*ERROR_CODE_THREADPOOL_CALLBACK_OBJECT_NOT_BOUND
		= ERROR_CODE_BASE - 404,*/

		// 콜백 객체가 NULL인 상태
		/*ERROR_CODE_THREADPOOL_CALLBACK_OBJECT_IS_NULL
		= ERROR_CODE_BASE - 405,*/

		// 콜백 객체 생성에 실패함
		/*ERROR_CODE_THREADPOOL_CREATE_CALLBACK_OBJECT_FAILURE
		= ERROR_CODE_BASE - 407,*/

		// 콜백 객체와 바인딩할 핸들이 유효하지 않음
		ERROR_CODE_THREADPOOL_INVALID_HANDLE
		= ERROR_CODE_BASE - 110,

		// 콜백 데이터가 이미 세팅되었음
		/*ERROR_CODE_THREADPOOL_CALLBACK_DATA_ALREADY_SET
		= ERROR_CODE_BASE - 411,*/

		// 스레드풀 전용 에러 코드 //
		// 디폴트 스레드풀이 아닌 스레드풀의 최소 스레드 개수(THREADPOOL_THREAD_MIN_COUNT)가 유효하지 않음
		ERROR_CODE_THREADPOOL_INVALID_THREAD_MIN_COUNT
		= ERROR_CODE_BASE - 1001,

		// 디폴트 스레드풀이 아닌 스레드풀의 최대 스레드 개수(THREADPOOL_THREAD_MAX_COUNT)가 유효하지 않음
		ERROR_CODE_THREADPOOL_INVALID_THREAD_MAX_COUNT
		= ERROR_CODE_BASE - 1002,

		// 디폴트 스레드풀은 명시적으로 제거할 수 없음
		ERROR_CODE_THREADPOOL_CANNOT_DESTROY_DEFAULT_THREADPOOL_EXPLICITLY
		= ERROR_CODE_BASE - 1003,

		// 특정 그룹의 스레드풀이 이미 존재함 (에러로 안쓰일 수 있음)
		ERROR_CODE_THREADPOOL_THREADPOOL_GROUP_ALREADY_EXISTS
		= ERROR_CODE_BASE - 1004,

		// 콜백 객체가 특정 그룹의 스레드풀에 이미 존재함
		ERROR_CODE_THREADPOOL_CALLBACK_OBJECT_ALREADY_EXISTS_IN_THREADPOOL_GROUP
		= ERROR_CODE_BASE - 1005,
	};

	struct CALLBACK_DATA_PARAMETER
	{
		// 공통
		ERROR_CODE							_errorCode;

		// TP_WAIT 전용
		TP_WAIT_RESULT						_tpWaitResult;					// WAIT_OBJECT_0(signaled), WAIT_TIMEOUT
		HANDLE								_hObject;						// TP_WAIT이랑 바인딩한 동기화 커널 객체 핸들

		// TP_IO 전용
		LPOVERLAPPED						_pOverlapped;					// 입출력을 식별할 OVERLAPPED
		ULONG								_ulIoResult;					// 윈도우 시스템 에러 코드와 동일, 입출력 결과
		ULONG_PTR							_ulpNumberOfBytesTransferred;	// 송신/수신한 데이터 크기

		// 공용 생성자
		CALLBACK_DATA_PARAMETER(ERROR_CODE errorCode, TP_WAIT_RESULT tpWaitResult, HANDLE hObject, LPOVERLAPPED pOverlapped, ULONG ulIoResult, ULONG_PTR ulpNumberOfBytesTransferred)
		{
			_errorCode = errorCode;

			_tpWaitResult = tpWaitResult;
			_hObject = hObject;

			_pOverlapped = pOverlapped;
			_ulIoResult = ulIoResult;
			_ulpNumberOfBytesTransferred = ulpNumberOfBytesTransferred;
		}

		// Work/Timer 콜백 객체 전용 생성자
		CALLBACK_DATA_PARAMETER(ERROR_CODE errorCode)
		{
			_errorCode = errorCode;

			_tpWaitResult = WAIT_OBJECT_0;

			_pOverlapped = NULL;
			_ulIoResult = 0;
			_ulpNumberOfBytesTransferred = 0;
		}

		// Wait 콜백 객체 전용 생성자
		CALLBACK_DATA_PARAMETER(ERROR_CODE errorCode, TP_WAIT_RESULT tpWaitResult, HANDLE hObject)
		{
			_errorCode = errorCode;

			_tpWaitResult = tpWaitResult;
			_hObject = hObject;

			_pOverlapped = NULL;
			_ulIoResult = 0;
			_ulpNumberOfBytesTransferred = 0;
		}

		// Io 콜백 객체 전용 생성자
		CALLBACK_DATA_PARAMETER(ERROR_CODE errorCode, LPOVERLAPPED pOverlapped, ULONG ulIoResult, ULONG_PTR ulpNumberOfBytesTransferred)
		{
			_errorCode = errorCode;

			_tpWaitResult = WAIT_OBJECT_0;

			_pOverlapped = pOverlapped;
			_ulIoResult = ulIoResult;
			_ulpNumberOfBytesTransferred = ulpNumberOfBytesTransferred;
		}

		void SetErrorCode(ERROR_CODE errorCode)
		{
			_errorCode = errorCode;
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


	typedef DWORD ThreadpoolGroup; // DEFAULT_THREAD_POOL이 아닌 0보다 큰 값
#define DEFAULT_THREAD_POOL 0
#define THREADPOOL_THREAD_MIN_COUNT 1 // 디폴트 스레드풀이 아닌 스레드풀의 스레드 최소 개수
#define THREADPOOL_THREAD_MAX_COUNT 500 // 디폴트 스레드풀이 아닌 스레드풀의 스레드 최대 개수

	// 스레드풀 그루핑 관련한 매개 변수 묶음 구조체
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
		// 해당 객체를 생성하고 나서 꼭 ERROR_CODE를 확인할 것
		CThreadpoolCallbackWorkWrapper(THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode);
		~CThreadpoolCallbackWorkWrapper();

		// 콜백 데이터 기반의 비동기 콜백 함수 호출 개시 (작업이 밀려있지 않는 이상 거의 바로 시작됨)
		BOOL ExecuteThreadpoolCallbackWork(ICallbackData* pCallbackData, ERROR_CODE& errorCode);
	};

	
	class CThreadpoolCallbackWait;
	// Wait 콜백 객체 래핑 클래스
	class THREADPOOLENGINE_API CThreadpoolCallbackWaitWrapper
	{
	protected:
		// Wait 콜백 객체
		CThreadpoolCallbackWait* _pThreadpoolCallbackWait;

	public:
		// 해당 객체를 생성하고 나서 꼭 ERROR_CODE를 확인할 것
		// hObject는 콜백 객체와 바인딩할 동기화 전용 커널 객체 핸들
		CThreadpoolCallbackWaitWrapper(THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode);
		~CThreadpoolCallbackWaitWrapper();
		
		// 동기화 커널 객체(hObject)가 signal 상태가 되면 콜백 데이터 기반의 비동기 콜백 함수 호출을 개시
		// ullTimeout = 0이면 INFINITE 옵션(무한 대기), 0보다 크면 밀리초(10^(-3)s) 단위 기준으로 대기, 최댓값은 ULONG_MAX
		// 절대 시간 기반의 동작 기능은 Wait 콜백 객체로 사용하지 않음 (Timer 객체 사용 권장)
		// 즉시 실행 (Wait 콜백 객체 기준 즉시 실행 옵션도 있음) 기능은 사용하지 않음 (Work 객체 사용 권장)
		// fCancelPendingCallbacks는 이전에 요청한 작업 취소(TRUE)/대기(FALSE) 여부
		BOOL ExecuteThreadpoolCallbackWait(HANDLE hObject, ICallbackData* pCallbackData, ULONG ulTimeout, BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode);
	};


	class CThreadpoolCallbackIo;
	// Io 콜백 객체 래핑 클래스
	class THREADPOOLENGINE_API CThreadpoolCallbackIoWrapper
	{
	protected:
		// Io 콜백 객체
		CThreadpoolCallbackIo* _pThreadpoolCallbackIo;

	public:
		// 해당 객체를 생성하고 나서 꼭 ERROR_CODE를 확인할 것
		// hDevice는 IOCP와 바인딩할 장치
		CThreadpoolCallbackIoWrapper(HANDLE hDevice, THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode);
		~CThreadpoolCallbackIoWrapper();

		// 바인딩된 해당 장치의 비동기 입출력 완료 시 콜백 데이터 기반의 콜백 함수가 호출되도록 개시
		// 반드시 비동기 입출력 함수(ex WSARecv)를 호출하기 전에 이 함수를 먼저 호출해야 함
		BOOL ExecuteThreadpoolCallbackIo(ERROR_CODE& errorCode);

		// 최초 1회만 콜백 데이터를 세팅할 수 있음
		// 콜백 데이터 변경 가능 여부 또는 하나의 장치에 다수의 콜백 데이터 세팅 가능 여부는 차후 필요에 따라 구현 결정
		BOOL SetCallbackData(ICallbackData* pCallbackData, ERROR_CODE& errorCode);

		// 비동기 입출력을 취소하고 실행 및 실행 대기 중인 콜백 함수 실행 완료를 대기
		// bCancelPendingCallbacks는 콜백 작업 큐에 있는 작업(비동기 입출력 완료로 인한 콜백 함수 호출)을 취소할 지 여부
		//BOOL CleanupThreadpoolCallbackIo(BOOL bCancelPendingCallbacks, ERROR_CODE& errorCode);

		// 콜백 객체로부터 입출력 완료 콜백 함수 호출을 개시했던 것을 취소하는 함수 
		// 사용 예 : ExecuteThreadpoolCallbackIo 호출 후 비동기 입출력 함수(ex WSARecv) 호출을 실패했을 경우 이 함수를 호출해야 함
		BOOL CancelThreadpoolCallbackIo(ERROR_CODE& errorCode);
	};


	// 래핑 클래스들을 하나로 묶을 지 고민중..
	template <class C>
	class THREADPOOLENGINE_API CThreadpoolCallbackWrapper
	{
	protected:
		C* _pThreadpoolCallbackObject;
	};
}
