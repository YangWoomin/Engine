#pragma once

namespace ThreadpoolEngine
{
	class CThreadpoolCallbackIo : public CThreadpoolCallbackObject<PTP_IO>
	{
	private:
		// IO 통지에 적용할 장치(파일, 소켓, 커널 객체 등)
		HANDLE _hDevice;

	public:
		CThreadpoolCallbackIo(HANDLE hDevice);
		~CThreadpoolCallbackIo();

		// 콜백 객체가 특정 스레드풀과 바인딩될 때 호출됨
		virtual BOOL BindThreadpoolCallbackObject(const PTP_CALLBACK_ENVIRON pTpCallbackEnviron, ERROR_CODE& errorCode);

		// 콜백 객체가 특정 스레드 풀에서 제거될 때 호출됨
		virtual BOOL ReleaseThreadpoolCallbackObject(BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode);

		// 콜백 데이터 세팅
		BOOL SetCallbackData(ICallbackData* pCallbackData, ERROR_CODE& errorCode);

		// 콜백 객체와 바인딩된 장치의 중첩 입출력 개시될 것을 통지
		BOOL ExecuteThreadpoolCallbackIo(ERROR_CODE& errorCode);

		// 콜백 객체에 대해 호출될 콜백 함수
		static VOID CALLBACK CallbackThreadpoolCallbackIo(PTP_CALLBACK_INSTANCE pInstance, PVOID pParam, PVOID pOverlapped, ULONG ulIoResult, ULONG_PTR ulpNumberOfBytesTransferred, PTP_IO pTpIo);

		// 바인딩된 장치의 중첩 입출력을 정리
		BOOL CleanupThreadpoolCallbackIo(BOOL bCancelPendingCallbacks, ERROR_CODE& errorCode);

		// 콜백 객체로부터 입출력 완료 콜백 함수 호출을 개시했던 것을 취소하는 함수 
		// 사용 예 : ExecuteThreadpoolCallbackIo 호출 후 비동기 입출력 함수(ex WSARecv) 호출을 실패했을 경우 이 함수를 호출해야 함
		BOOL CancelThreadpoolCallbackIo(ERROR_CODE& errorCode);
	};
}