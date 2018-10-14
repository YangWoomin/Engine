#pragma once

namespace ThreadpoolEngine
{
	class CThreadpoolCallbackWait : public CThreadpoolCallbackObject<PTP_WAIT>
	{
	private:
		// �ݹ� ��ü�� ���ε��� ����ȭ Ŀ�� ��ü
		HANDLE _hObject;

		// ��� �ð��� ���� ���� Ÿ�� ������
		FILETIME _fileTime;

	public:
		CThreadpoolCallbackWait();
		virtual ~CThreadpoolCallbackWait();

		// �ݹ� ��ü�� Ư�� ������Ǯ�� ���ε��� �� ȣ���
		virtual BOOL BindThreadpoolCallbackObject(const PTP_CALLBACK_ENVIRON pTpCallbackEnviron, ERROR_CODE& errorCode);

		// �ݹ� ��ü�� Ư�� ������ Ǯ���� ���ŵ� �� ȣ���
		virtual BOOL ReleaseThreadpoolCallbackObject(BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode);

		// �ݹ� ��ü�� �ݹ� �Լ� �񵿱� ȣ�� ����
		BOOL ExecuteThreadpoolCallbackWait(HANDLE hObject, ICallbackData* pCallbackData, FILETIME fileTime, BOOL bInfinite, BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode);

		// �ݹ� ��ü�� ���� ȣ��� �ݹ� �Լ�
		static VOID CALLBACK CallbackThreadpoolCallbackWait(PTP_CALLBACK_INSTANCE pTpCallbackInstance, PVOID pParam, PTP_WAIT pTpWait, TP_WAIT_RESULT tpWaitResult);

		// �ݹ� �Լ� ȣ���� �����ߴ� ���� ����ϴ� �Լ�
		// bWaitForCallbacks�� ��� �ݹ� �Լ����� ���� �ϷḦ ������� ����
		// fCancelPendingCallbacks�� ���� �������� �ݹ� �Լ��� �ϷḸ �������(����Ǿ�� �� ������ �ݹ� �Լ����� ȣ���� ���), �ƴϸ� ��� ����� ������ ������� ����
		BOOL CancelThreadpoolCallbackWait(BOOL bWaitForCallbacks, BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode);

		HANDLE GetObjectHandle();
	};
}