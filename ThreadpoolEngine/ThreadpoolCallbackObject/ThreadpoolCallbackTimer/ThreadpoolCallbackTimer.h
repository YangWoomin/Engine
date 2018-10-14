#pragma once

namespace ThreadpoolEngine
{
	class CThreadpoolCallbackTimer : public CThreadpoolCallbackObject<PTP_TIMER>
	{
	private:
		// ��� �ð��� ���� ���� Ÿ�� ������
		FILETIME _fileTime;

	public:
		CThreadpoolCallbackTimer();
		~CThreadpoolCallbackTimer();

		// �ݹ� ��ü�� Ư�� ������Ǯ�� ���ε��� �� ȣ���
		virtual BOOL BindThreadpoolCallbackObject(const PTP_CALLBACK_ENVIRON pTpCallbackEnviron, ERROR_CODE& errorCode);

		// �ݹ� ��ü�� Ư�� ������ Ǯ���� ���ŵ� �� ȣ���
		virtual BOOL ReleaseThreadpoolCallbackObject(BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode);

		// �ݹ� ��ü�� �ݹ� �Լ� �񵿱� ȣ�� ����
		BOOL ExecuteThreadpoolCallbackTimer(ICallbackData* pCallbackData, FILETIME fileTime, DWORD dwPeriodMillisecond, BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode);

		// �ݹ� ��ü�� ���� ȣ��� �ݹ� �Լ�
		static VOID CALLBACK CallbackThreadpoolCallbackTimer(PTP_CALLBACK_INSTANCE pTpCallbackInstance, PVOID pParam, PTP_TIMER pTpTimer);

		// �ݹ� �Լ� ȣ���� �����ߴ� ���� ����ϴ� �Լ�
		// bWaitForCallbacks�� ��� �ݹ� �Լ����� ���� �ϷḦ ������� ����
		// fCancelPendingCallbacks�� ���� �������� �ݹ� �Լ��� �ϷḸ �������(����Ǿ�� �� ������ �ݹ� �Լ����� ȣ���� ���), �ƴϸ� ��� ����� ������ ������� ����
		BOOL CancelThreadpoolCallbackTimer(BOOL bWaitForCallbacks, BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode);
	};
}