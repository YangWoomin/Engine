#pragma once


#ifdef THREADPOOLENGINE_EXPORTS
#define THREADPOOLENGINE_API __declspec(dllexport)
#else
#define THREADPOOLENGINE_API __declspec(dllimport)
#endif

namespace ThreadpoolEngine
{
	class CThreadpoolCallbackTimer;

	// Timer �ݹ� ��ü ���� Ŭ����
	class THREADPOOLENGINE_API CThreadpoolCallbackTimerWrapper : public CThreadpoolCallbackObjectWrapper<CThreadpoolCallbackTimer>
	{
	public:
		// �ش� ��ü�� �����ϰ� ���� �� ERROR_CODE�� Ȯ���� ��
		CThreadpoolCallbackTimerWrapper(THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode);
		virtual ~CThreadpoolCallbackTimerWrapper();

		// ������ ���� �ð�(callbackTime, ex 2018-10-15 12:00)�� �ݹ� �Լ��� ȣ���ϰ� ������ �ֱ�(ulPeriodMillisecond)�� �ݹ� �Լ��� ȣ��
		// fCancelPendingCallbacks�� ������ ��û�� �۾� ���(TRUE)/���(FALSE) ����
		BOOL ExecuteThreadpoolCallbackTimer(ICallbackData* pCallbackData, SYSTEMTIME callbackTime, BOOL bConvertToUtcTime, ULONG ulPeriodMillisecond, BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode);

		// ������ ��� �ð�(ulTimeoutMillisecond, 3�� �� ��)�� �ݹ� �Լ��� ȣ���ϰ� ������ �ֱ�(ulPeriodMillisecond)�� �ݹ� �Լ��� ȣ��
		// �ΰ� ���� ulTimeoutMillisecond, ulPeriodMillisecond�� �и���(10^(-3)s) ���� �������� ���� ����
		// fCancelPendingCallbacks�� ������ ��û�� �۾� ���(TRUE)/���(FALSE) ����
		BOOL ExecuteThreadpoolCallbackTimer(ICallbackData* pCallbackData, ULONG ulTimeoutMillisecond, ULONG ulPeriodMillisecond, BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode);

		// �ݹ� �Լ� ȣ���� �����ߴ� ���� ����ϴ� �Լ�
		// bWaitForCallbacks�� ��� �ݹ� �Լ����� ���� �ϷḦ ������� ����
		// fCancelPendingCallbacks�� ���� �������� �ݹ� �Լ��� �ϷḸ �������(����Ǿ�� �� ������ �ݹ� �Լ����� ȣ���� ���), �ƴϸ� ��� ����� ������ ������� ����
		BOOL CancelThreadpoolCallbackTimer(BOOL bWaitForCallbacks, BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode);
	};
}