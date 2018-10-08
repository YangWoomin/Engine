#pragma once

/*

	- ��� �� ���� ����
	

	- ����
	

*/

typedef std::queue<ICallbackData*> CallbackDataQueue;

class CThreadpoolCallbackWork : public CThreadpoolCallbackObject<PTP_WORK>
{
private:
	// �ݹ� �Լ��� ȣ��ǰ� �۾��� �ݹ� ������ ť
	CallbackDataQueue _callbackDataQueue;

	// �ݹ� ������ ť�� ��ȣ�ϱ� ���� ũ��Ƽ�� ����
	CRITICAL_SECTION _criticalSection;

public:
	CThreadpoolCallbackWork();
	virtual ~CThreadpoolCallbackWork();

	// �ݹ� ��ü�� Ư�� ������Ǯ�� ���ε��� �� ȣ���
	virtual BOOL BindThreadpoolCallbackObject(const PTP_CALLBACK_ENVIRON pTpCallbackEnviron, ERROR_CODE& errorCode);

	// �ݹ� ��ü�� Ư�� ������ Ǯ���� ���ŵ� �� ȣ���
	virtual BOOL ReleaseThreadpoolCallbackObject(BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode);

	// �ݹ� ��ü�� �ݹ� �Լ� �񵿱� ȣ�� ����
	// �ݹ� �����Ͱ� ť�� �׿��� ���������� ȣ��Ǵ� ����
	BOOL ExecuteThreadpoolCallbackWork(ICallbackData* pCallbackData, ERROR_CODE& errorCode);

	// �ݹ� ��ü�� ���� ȣ��� �ݹ� �Լ�
	static VOID CALLBACK ThreadpoolCallbackWorkCallbackFunction(PTP_CALLBACK_INSTANCE pInstance, PVOID pParam, PTP_WORK pTpWork);

	// �ݹ� �Լ����� �۾��� �ݹ� �����͸� ť�κ��� ����
	ICallbackData* GetCallbackDataFromQueue();
};