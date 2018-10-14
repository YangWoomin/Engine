#pragma once


#ifdef THREADPOOLENGINE_EXPORTS
#define THREADPOOLENGINE_API __declspec(dllexport)
#else
#define THREADPOOLENGINE_API __declspec(dllimport)
#endif

namespace ThreadpoolEngine
{
	class CThreadpoolCallbackWait;

	// Wait 콜백 객체 래핑 클래스
	class THREADPOOLENGINE_API CThreadpoolCallbackWaitWrapper : public CThreadpoolCallbackObjectWrapper<CThreadpoolCallbackWait>
	{
	public:
		// 해당 객체를 생성하고 나서 꼭 ERROR_CODE를 확인할 것
		CThreadpoolCallbackWaitWrapper(THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode);
		virtual ~CThreadpoolCallbackWaitWrapper();

		// hObject는 콜백 객체와 바인딩할 동기화 전용 커널 객체 핸들
		// hObject가 signal 상태가 되면 콜백 데이터 기반의 비동기 콜백 함수 호출을 개시
		// ullTimeout = 0이면 INFINITE 옵션(무한 대기), 0보다 크면 밀리초(10^(-3)s) 단위 기준으로 대기, 최댓값은 ULONG_MAX
		// 절대 시간 기반의 동작 기능은 Wait 콜백 객체로 사용하지 않음 (Timer 객체 사용 권장)
		// 즉시 실행 (Wait 콜백 객체 기준 즉시 실행 옵션도 있음) 기능은 사용하지 않음 (Work 객체 사용 권장)
		// fCancelPendingCallbacks는 이전에 요청한 작업 취소(TRUE)/대기(FALSE) 여부
		BOOL ExecuteThreadpoolCallbackWait(HANDLE hObject, ICallbackData* pCallbackData, ULONG ulTimeoutMillisecond, BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode);

		// 콜백 함수 호출을 개시했던 것을 취소하는 함수
		// bWaitForCallbacks는 모든 콜백 함수들의 실행 완료를 대기할지 여부
		// fCancelPendingCallbacks는 현재 실행중인 콜백 함수의 완료만 대기할지(실행되어야 할 나머지 콜백 함수들의 호출을 취소), 아니면 모두 실행될 때까지 대기할지 여부
		BOOL CancelThreadpoolCallbackWait(BOOL bWaitForCallbacks, BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode);
	};
}