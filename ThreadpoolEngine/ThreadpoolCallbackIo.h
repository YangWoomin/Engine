#pragma once

class CThreadpoolCallbackIo : public CThreadpoolCallbackObject<PTP_IO>
{
private:
	// IO ������ ������ ��ġ(����, ����, Ŀ�� ��ü ��)
	HANDLE _hDevice;

	// �ݹ� �Լ��� ȣ��ǰ� �۾��� �ݹ� ������
	ICallbackData* _pCallbackData;

public:
	CThreadpoolCallbackIo(HANDLE hDevice, THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode);
	~CThreadpoolCallbackIo();

	// �ݹ� ��ü�� Ư�� ������Ǯ�� ���ε��� �� ȣ���
	virtual BOOL BindThreadpoolCallbackObject(const PTP_CALLBACK_ENVIRON pTpCallbackEnviron, ERROR_CODE& errorCode);

	// �ݹ� ��ü�� Ư�� ������ Ǯ���� ���ŵ� �� ȣ���
	virtual BOOL ReleaseThreadpoolCallbackObject(BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode);

	// �ݹ� ������ ����
	BOOL SetCallbackData(ICallbackData* pCallbackData, ERROR_CODE& errorCode);

	// �ݹ� ��ü�� ���ε��� ��ġ�� ��ø ����� ���õ� ���� ����
	BOOL ExecuteThreadpoolCallbackIo(ERROR_CODE& errorCode);

	// �ݹ� ��ü�� ���� ȣ��� �ݹ� �Լ�
	static VOID CALLBACK ThreadpoolCallbackIoCallbackFunction(PTP_CALLBACK_INSTANCE pInstance, PVOID pParam, PVOID pOverlapped, ULONG ulIoResult, ULONG_PTR ulpNumberOfBytesTransferred, PTP_IO pTpIo);

	ICallbackData* GetCallbackData();

	// ���ε��� ��ġ�� ��ø ������� ���
	BOOL CancelThreadpoolCallbackIo(BOOL bCancelPendingCallbacks, ERROR_CODE& errorCode);
};