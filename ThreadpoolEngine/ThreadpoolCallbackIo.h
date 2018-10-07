#pragma once

class CThreadpoolCallbackIo : public CThreadpoolCallbackObject<PTP_IO>
{
private:
	// IO 통지에 적용할 장치(파일, 소켓, 커널 객체 등)
	HANDLE _hDevice;

	// 콜백 함수가 호출되고 작업할 콜백 데이터
	ICallbackData* _pCallbackData;

public:
	CThreadpoolCallbackIo(HANDLE hDevice, THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode);
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
	static VOID CALLBACK ThreadpoolCallbackIoCallbackFunction(PTP_CALLBACK_INSTANCE pInstance, PVOID pParam, PVOID pOverlapped, ULONG ulIoResult, ULONG_PTR ulpNumberOfBytesTransferred, PTP_IO pTpIo);

	ICallbackData* GetCallbackData();

	// 바인딩된 장치의 중첩 입출력을 취소
	BOOL CancelThreadpoolCallbackIo(BOOL bCancelPendingCallbacks, ERROR_CODE& errorCode);
};