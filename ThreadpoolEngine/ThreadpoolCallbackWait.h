#pragma once

namespace ThreadpoolEngine
{
	class CThreadpoolCallbackWait : public CThreadpoolCallbackObject<PTP_WAIT>
	{
	private:
		// 콜백 객체와 바인딩할 동기화 커널 객체
		HANDLE _hObject;

		// 대기 시간을 위한 파일 타임 데이터
		FILETIME _fileTime;

	public:
		CThreadpoolCallbackWait();
		virtual ~CThreadpoolCallbackWait();

		// 콜백 객체가 특정 스레드풀과 바인딩될 때 호출됨
		virtual BOOL BindThreadpoolCallbackObject(const PTP_CALLBACK_ENVIRON pTpCallbackEnviron, ERROR_CODE& errorCode);

		// 콜백 객체가 특정 스레드 풀에서 제거될 때 호출됨
		virtual BOOL ReleaseThreadpoolCallbackObject(BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode);

		// 콜백 객체의 콜백 함수 비동기 호출 개시
		BOOL ExecuteThreadpoolCallbackWait(HANDLE hObject, ICallbackData* pCallbackData, FILETIME fileTime, BOOL bInfinite, BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode);

		// 콜백 객체에 대해 호출될 콜백 함수
		static VOID CALLBACK CallbackThreadpoolCallbackWait(PTP_CALLBACK_INSTANCE pInstance, PVOID pParam, PTP_WAIT pTpWait, TP_WAIT_RESULT tpWaitResult);

		HANDLE GetObjectHandle();
	};
}