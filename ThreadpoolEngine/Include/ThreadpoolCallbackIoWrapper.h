#pragma once


#ifdef THREADPOOLENGINE_EXPORTS
#define THREADPOOLENGINE_API __declspec(dllexport)
#else
#define THREADPOOLENGINE_API __declspec(dllimport)
#endif

namespace ThreadpoolEngine
{
	// Io �ݹ� ��ü ���� Ŭ����
	class THREADPOOLENGINE_API CThreadpoolCallbackIoWrapper : public CThreadpoolCallbackObjectWrapper<CThreadpoolCallbackIo>
	{
	public:
		// �ش� ��ü�� �����ϰ� ���� �� ERROR_CODE�� Ȯ���� ��
		// hDevice�� IOCP�� ���ε��� ��ġ
		CThreadpoolCallbackIoWrapper(HANDLE hDevice, THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode);
		virtual ~CThreadpoolCallbackIoWrapper();

		// ���� 1ȸ�� �ݹ� �����͸� ������ �� ����
		// �ݹ� ������ ���� ���� ���� �Ǵ� �ϳ��� ��ġ�� �ټ��� �ݹ� ������ ���� ���� ���δ� ���� �ʿ信 ���� ���� ����
		BOOL SetCallbackData(ICallbackData* pCallbackData, ERROR_CODE& errorCode);

		// ���ε��� �ش� ��ġ�� �񵿱� ����� �Ϸ� �� �ݹ� ������ ����� �ݹ� �Լ��� ȣ��ǵ��� ����
		// �ݵ�� �񵿱� ����� �Լ�(ex WSARecv)�� ȣ���ϱ� ���� �� �Լ��� ���� ȣ���ؾ� ��
		BOOL ExecuteThreadpoolCallbackIo(ERROR_CODE& errorCode);

		// �ݹ� ��ü�κ��� ����� �Ϸ� �ݹ� �Լ� ȣ���� �����ߴ� ���� ����ϴ� �Լ� 
		// ��� �� : ExecuteThreadpoolCallbackIo ȣ�� �� �񵿱� ����� �Լ�(ex WSARecv) ȣ���� �������� ��� �� �Լ��� ȣ���ؾ� ��
		BOOL CancelThreadpoolCallbackIo(ERROR_CODE& errorCode);
	};
}