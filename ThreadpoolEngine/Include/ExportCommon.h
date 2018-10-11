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

	// Work, Wait, Timer, Io 콜백 객체의 생성/삭제를 관리하기 위한 템플릿 레퍼 글래스
	template <class ThreadpoolCallbackObject>
	class THREADPOOLENGINE_API CThreadpoolCallbackObjectWrapper
	{
	protected:
		// Work, Wait, Timer, Io 콜백 객체
		ThreadpoolCallbackObject* _pThreadpoolCallbackObject;

	public:
		// Work, Wait, Timer 콜백 객체 전용 래퍼 생성자
		CThreadpoolCallbackObjectWrapper(THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode)
		{
			_pThreadpoolCallbackObject = new ThreadpoolCallbackObject();
			if (NULL == _pThreadpoolCallbackObject)
			{
				errorCode = ERROR_CODE_THREADPOOL_CREATE_CALLBACK_OBJECT_FAILURE;
			}
			else
			{
				// 콜백 객체를 스레드풀에 그루핑
				_pThreadpoolCallbackObject->InitializeThreadpoolCallbackObject(threadpoolGroupParameter, errorCode);
			}
		}

		// Io 콜백 객체 전용 래퍼 생성자
		CThreadpoolCallbackObjectWrapper(HANDLE hDevice, THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode)
		{
			_pThreadpoolCallbackObject = new ThreadpoolCallbackObject(hDevice);
			if (NULL == _pThreadpoolCallbackObject)
			{
				errorCode = ERROR_CODE_THREADPOOL_CREATE_CALLBACK_OBJECT_FAILURE;
			}
			else
			{
				// 콜백 객체를 스레드풀에 그루핑
				_pThreadpoolCallbackObject->InitializeThreadpoolCallbackObject(threadpoolGroupParameter, errorCode);
			}
		}

		~CThreadpoolCallbackObjectWrapper()
		{
			if (NULL != _pThreadpoolCallbackObject)
			{
				// 콜백 객체를 그루핑된 스레드풀에서 제거
				_pThreadpoolCallbackObject->FinalizeThreadpoolCallbackObject();

				delete _pThreadpoolCallbackObject;
				_pThreadpoolCallbackObject = NULL;
			}
		}
	};
}
