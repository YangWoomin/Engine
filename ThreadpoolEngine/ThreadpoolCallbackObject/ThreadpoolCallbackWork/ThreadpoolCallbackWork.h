#pragma once

namespace ThreadpoolEngine
{
	typedef std::queue<ICallbackData*> CallbackDataQueueInWork;

	class CThreadpoolCallbackWork : public CThreadpoolCallbackObject<PTP_WORK>
	{
	private:
		// 콜백 함수가 호출되고 작업할 콜백 데이터 큐
		CallbackDataQueueInWork _callbackDataQueue;

		// 콜백 데이터 큐를 보호하기 위한 크리티컬 섹션
		CRITICAL_SECTION _criticalSection;

	public:
		CThreadpoolCallbackWork();
		virtual ~CThreadpoolCallbackWork();

		// 콜백 객체가 특정 스레드풀과 바인딩될 때 호출됨
		virtual BOOL BindThreadpoolCallbackObject(const PTP_CALLBACK_ENVIRON pTpCallbackEnviron, ERROR_CODE& errorCode);

		// 콜백 객체가 특정 스레드 풀에서 제거될 때 호출됨
		virtual BOOL ReleaseThreadpoolCallbackObject(BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode);

		// 콜백 객체의 콜백 함수 비동기 호출 개시
		// 콜백 데이터가 큐에 쌓여서 순차적으로 호출되는 구조
		BOOL ExecuteThreadpoolCallbackWork(ICallbackData* pCallbackData, ERROR_CODE& errorCode);

		// 콜백 객체에 대해 호출될 콜백 함수
		static VOID CALLBACK CallbackThreadpoolCallbackWork(PTP_CALLBACK_INSTANCE pTpCallbackInstance, PVOID pParam, PTP_WORK pTpWork);

		// 콜백 함수에서 작업할 콜백 데이터를 큐로부터 추출
		ICallbackData* PopCallbackDataFromQueue();
	};
}