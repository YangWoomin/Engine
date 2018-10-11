#pragma once


#ifdef THREADPOOLENGINE_EXPORTS
#define THREADPOOLENGINE_API __declspec(dllexport)
#else
#define THREADPOOLENGINE_API __declspec(dllimport)
#endif

namespace ThreadpoolEngine
{
	// Wait �ݹ� ��ü ���� Ŭ����
	class THREADPOOLENGINE_API CThreadpoolCallbackWaitWrapper : public CThreadpoolCallbackObjectWrapper<CThreadpoolCallbackWait>
	{
	public:
		// �ش� ��ü�� �����ϰ� ���� �� ERROR_CODE�� Ȯ���� ��
		CThreadpoolCallbackWaitWrapper(THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode);
		virtual ~CThreadpoolCallbackWaitWrapper();

		// hObject�� �ݹ� ��ü�� ���ε��� ����ȭ ���� Ŀ�� ��ü �ڵ�
		// hObject�� signal ���°� �Ǹ� �ݹ� ������ ����� �񵿱� �ݹ� �Լ� ȣ���� ����
		// ullTimeout = 0�̸� INFINITE �ɼ�(���� ���), 0���� ũ�� �и���(10^(-3)s) ���� �������� ���, �ִ��� ULONG_MAX
		// ���� �ð� ����� ���� ����� Wait �ݹ� ��ü�� ������� ���� (Timer ��ü ��� ����)
		// ��� ���� (Wait �ݹ� ��ü ���� ��� ���� �ɼǵ� ����) ����� ������� ���� (Work ��ü ��� ����)
		// fCancelPendingCallbacks�� ������ ��û�� �۾� ���(TRUE)/���(FALSE) ����
		BOOL ExecuteThreadpoolCallbackWait(HANDLE hObject, ICallbackData* pCallbackData, ULONG ulTimeoutMillisecond, BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode);
	};
}