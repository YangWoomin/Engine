#pragma once


#ifdef THREADPOOLENGINE_EXPORTS
#define THREADPOOLENGINE_API __declspec(dllexport)
#else
#define THREADPOOLENGINE_API __declspec(dllimport)
#endif

namespace ThreadpoolEngine
{
	// Timer �ݹ� ��ü ���� Ŭ����
	class THREADPOOLENGINE_API CThreadpoolCallbackTimerWrapper : public CThreadpoolCallbackObjectWrapper<CThreadpoolCallbackTimer>
	{
	public:
		// �ش� ��ü�� �����ϰ� ���� �� ERROR_CODE�� Ȯ���� ��
		CThreadpoolCallbackTimerWrapper(THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode);
		virtual ~CThreadpoolCallbackTimerWrapper();

		
	};
}