#pragma once


#ifdef THREADPOOLENGINE_EXPORTS
#define THREADPOOLENGINE_API __declspec(dllexport)
#else
#define THREADPOOLENGINE_API __declspec(dllimport)
#endif

namespace ThreadpoolEngine
{
	class CThreadpoolCallbackTimer;

	// Timer 콜백 객체 래핑 클래스
	class THREADPOOLENGINE_API CThreadpoolCallbackTimerWrapper : public CThreadpoolCallbackObjectWrapper<CThreadpoolCallbackTimer>
	{
	public:
		// 해당 객체를 생성하고 나서 꼭 ERROR_CODE를 확인할 것
		CThreadpoolCallbackTimerWrapper(THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode);
		virtual ~CThreadpoolCallbackTimerWrapper();

		// 지정한 절대 시간(callbackTime, ex 2018-10-15 12:00)에 콜백 함수를 호출하고 지정한 주기(ulPeriodMillisecond)로 콜백 함수를 호출
		// fCancelPendingCallbacks는 이전에 요청한 작업 취소(TRUE)/대기(FALSE) 여부
		BOOL ExecuteThreadpoolCallbackTimer(ICallbackData* pCallbackData, SYSTEMTIME callbackTime, BOOL bConvertToUtcTime, ULONG ulPeriodMillisecond, BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode);

		// 지정한 상대 시간(ulTimeoutMillisecond, 3초 후 등)에 콜백 함수를 호출하고 지정한 주기(ulPeriodMillisecond)로 콜백 함수를 호출
		// 두개 변수 ulTimeoutMillisecond, ulPeriodMillisecond는 밀리초(10^(-3)s) 단위 기준으로 지정 가능
		// fCancelPendingCallbacks는 이전에 요청한 작업 취소(TRUE)/대기(FALSE) 여부
		BOOL ExecuteThreadpoolCallbackTimer(ICallbackData* pCallbackData, ULONG ulTimeoutMillisecond, ULONG ulPeriodMillisecond, BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode);

		// 콜백 함수 호출을 개시했던 것을 취소하는 함수
		// bWaitForCallbacks는 모든 콜백 함수들의 실행 완료를 대기할지 여부
		// fCancelPendingCallbacks는 현재 실행중인 콜백 함수의 완료만 대기할지(실행되어야 할 나머지 콜백 함수들의 호출을 취소), 아니면 모두 실행될 때까지 대기할지 여부
		BOOL CancelThreadpoolCallbackTimer(BOOL bWaitForCallbacks, BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode);
	};
}