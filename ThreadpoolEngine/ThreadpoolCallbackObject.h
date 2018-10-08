#pragma once

using namespace ThreadpoolGroupManager;

// TP_WORK, TP_WAIT, TP_TIMER, TP_IO 4가지 콜백 객체의 추상 타입
interface IThreadpoolCallbackObject
{
	//////////////////////////// 전체 스레드 대상으로 동시에 실행될 수 없는 함수 ////////////////////////////
	// ThreadpoolGroupManager를 사용하기 위한 소유권 때문에 발생
	//
	// 해당 콜백 객체와 TP_CALLBACK_ENVIRON과 바인딩하도록 정의해야 함
	virtual BOOL BindThreadpoolCallbackObject(const PTP_CALLBACK_ENVIRON pTpCallbackEnviron, ERROR_CODE& errorCode) = 0;
	//
	// 해당 콜백 객체가 해제되는 것을 보장해야 함
	virtual BOOL ReleaseThreadpoolCallbackObject(BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode) = 0;
	//
	/////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual ~IThreadpoolCallbackObject() { }
};

// TP_WORK, TP_WAIT, TP_TIMER, TP_IO 4종류로 사용
template <typename T> 
class CThreadpoolCallbackObject : public IThreadpoolCallbackObject
{
protected:
	// 콜백 객체
	T _pThreadpoolCallbackObject;

	// 스레드풀 그룹에 정상적으로 추가되었는지 여부
	BOOL _bIsGrouping;

	// 콜백 객체에 대한 콜백 함수 호출 개시 횟수
	// USHORT _usCallbackRunningCount;

	// 콜백 객체에 대한 콜백 함수 호출 개시 가능한 최대 횟수
	// const ULONG _MAXIMUM_CALLBACK_RUNNING_COUNT = USHRT_MAX;

public:
	CThreadpoolCallbackObject() { }

	virtual ~CThreadpoolCallbackObject() { }

	T GetThreadpoolCallbackObject()
	{
		return _pThreadpoolCallbackObject;
	}

	// 콜백 객체 생성자 호출 후에 이 함수를 호출해야 함
	void Initialize(THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode)
	{
		_pThreadpoolCallbackObject = NULL;

		// 특정 그룹의 스레드풀 생성, 없으면 생성하고 있으면 그냥 쓰면 됨
		ThreadpoolGroupManager()->CreateThreadpool(threadpoolGroupParameter._dwThreadpoolGroup, threadpoolGroupParameter._iMinThreadCount, threadpoolGroupParameter._iMaxThreadCount, errorCode);

		// 스레드풀 특정 그룹에 콜백 객체를 추가
		if (ERROR_CODE_THREADPOOL_THREADPOOL_GROUP_ALREADY_EXISTS == errorCode || ERROR_CODE_NONE == errorCode)
		{
			// 스레드풀 그룹 매니저에 의해 BindThreadpoolCallbackObject 호출됨
			errorCode = ERROR_CODE_NONE;
			_bIsGrouping = ThreadpoolGroupManager()->InsertThreadpoolCallbackObject(threadpoolGroupParameter._dwThreadpoolGroup, this, errorCode);
		}
	}

	// 콜백 객체의 소멸자 호출 전에 이 함수를 호출해야 함
	void Finalize()
	{
		// 특정 그룹의 스레드풀에 추가되었다면 제거
		if (TRUE == _bIsGrouping)
		{
			// 스레드풀 그룹 매니저에 의해 ReleaseThreadpoolCallbackObject 호출됨
			ERROR_CODE errorCode = ERROR_CODE_NONE;
			ThreadpoolGroupManager()->DeleteThreadpoolCallbackObject(this, FALSE, errorCode);
			_bIsGrouping = FALSE;
		}

		// 생성했던 스레드 풀은 여기서 제거하지 않도록 함
		// 스레드풀 그룹 매니저가 프로그램 종료할 때 일괄적으로 자동 제거
	}

	/*
	// 콜백 함수 호출 개시 횟수 추출
	ULONG GetCallbackRunningCount()
	{
		// LONG 최솟값은 세팅되지 않으므로 현재 개시 횟수만 추출 가능
		return ::InterlockedCompareExchange((LONG*)&_usCallbackRunningCount, 0, LONG_MIN);
	}

	// 콜백 함수 호출 개시 횟수를 0으로 초기화
	// WaitForThreadpoolXXXCallbacks 호출에서 fCancelPendingCallbacks를 TRUE로 넘기면 이 함수를 호출해야 함
	void ResetCallbackRunningCount()
	{
		::InterlockedExchange((LONG*)&_usCallbackRunningCount, 0);
	}

	// 콜백 함수 호출 개시 횟수 증가
	ULONG IncreaseCallbackRunningCount()
	{
		return ::InterlockedIncrement((LONG*)&_usCallbackRunningCount);
	}

	// 콜백 함수 호출 개시 횟수 감소
	LONG DecreaseCallbackRunningCount()
	{
		// 0 이하면 내리지 않음
		LONG lCallbackRunningCount = GetCallbackRunningCount();
		if (0 >= lCallbackRunningCount)
		{
			return lCallbackRunningCount;
		}

		return ::InterlockedDecrement((LONG*)&_usCallbackRunningCount);
	}

	// 콜백 데이터 세팅
	// bWaitForPreviousCallback은 콜백 객체의 이전에 요청한 콜백 함수 호출이 완료된 후 실행할 지 여부
	// fCancelPendingCallbacks은 콜백 객체의 이전에 요청한 콜백 함수 호출 개시를 취소할 지 여부
	virtual BOOL SetCallbackData(ICallbackData* pCallbackData, BOOL bWaitForPreviousCallback, BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode) = 0;
	*/

	// 콜백 객체의 콜백 함수 비동기 호출 개시
	// 1개의 콜백 객체에 대해서 중첩으로 실행할 수 있지만 콜백 데이터에 있는 콜백 함수가 비동기적으로 실행
	// bWaitForPreviousCallback은 콜백 객체의 이전에 요청한 콜백 함수 호출이 완료된 후 실행할 지 여부
	// fCancelPendingCallbacks은 콜백 객체의 이전에 요청한 콜백 함수 호출 개시를 취소할 지 여부
	// virtual BOOL ExecuteThreadpoolCallbackWork(ERROR_CODE& errorCode, BOOL bWaitForPreviousCallback, BOOL fCancelPendingCallbacks) = 0;
};