#pragma once


#ifdef THREADPOOLENGINE_EXPORTS
#define THREADPOOLENGINE_API __declspec(dllexport)
#else
#define THREADPOOLENGINE_API __declspec(dllimport)
#endif

namespace ThreadpoolEngine
{
	// Work �ݹ� ��ü ���� Ŭ����
	class THREADPOOLENGINE_API CThreadpoolCallbackWorkWrapper : public CThreadpoolCallbackObjectWrapper<CThreadpoolCallbackWork>
	{
	public:
		// �ش� ��ü�� �����ϰ� ���� �� ERROR_CODE�� Ȯ���� ��
		CThreadpoolCallbackWorkWrapper(THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode);
		virtual ~CThreadpoolCallbackWorkWrapper();

		// �ݹ� ������ ����� �񵿱� �ݹ� �Լ� ȣ�� ���� (�۾��� �з����� �ʴ� �̻� ���� �ٷ� ���۵�)
		BOOL ExecuteThreadpoolCallbackWork(ICallbackData* pCallbackData, ERROR_CODE& errorCode);
	};
}