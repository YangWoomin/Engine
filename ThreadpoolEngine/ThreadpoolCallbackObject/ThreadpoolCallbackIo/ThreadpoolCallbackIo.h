#pragma once

namespace ThreadpoolEngine
{
	class CThreadpoolCallbackIo : public CThreadpoolCallbackObject<PTP_IO>
	{
	private:
		// IO ������ ������ ��ġ(����, ����, Ŀ�� ��ü ��)
		HANDLE _hDevice;

	public:
		CThreadpoolCallbackIo(HANDLE hDevice);
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
		static VOID CALLBACK CallbackThreadpoolCallbackIo(PTP_CALLBACK_INSTANCE pInstance, PVOID pParam, PVOID pOverlapped, ULONG ulIoResult, ULONG_PTR ulpNumberOfBytesTransferred, PTP_IO pTpIo);

		// ���ε��� ��ġ�� ��ø ������� ����
		BOOL CleanupThreadpoolCallbackIo(BOOL bCancelPendingCallbacks, ERROR_CODE& errorCode);

		// �ݹ� ��ü�κ��� ����� �Ϸ� �ݹ� �Լ� ȣ���� �����ߴ� ���� ����ϴ� �Լ� 
		// ��� �� : ExecuteThreadpoolCallbackIo ȣ�� �� �񵿱� ����� �Լ�(ex WSARecv) ȣ���� �������� ��� �� �Լ��� ȣ���ؾ� ��
		BOOL CancelThreadpoolCallbackIo(ERROR_CODE& errorCode);
	};
}