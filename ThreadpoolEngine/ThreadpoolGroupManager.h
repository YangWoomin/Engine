#pragma once

namespace ThreadpoolEngine
{
	interface IThreadpoolCallbackObject;

	namespace ThreadpoolGroupManager
	{

		//#define DEFAULT_THREAD_POOL 0
#define ThreadpoolGroupManager() CThreadpoolGroupManager::GetInstance()

		//typedef DWORD ThreadpoolGroup; // DEFAULT_THREAD_POOL�� �ƴ� 0���� ū ��

		typedef std::set<IThreadpoolCallbackObject*> ThreadpoolCallbackObjectSet;

		//// CreateThreadpool �Ű� ���� ���� ����ü
		//struct THREADPOOL_GROUP_PARAMETER
		//{
		//	ThreadpoolGroup				_dwThreadpoolGroup;
		//	int							_iMinThreadCount;
		//	int							_iMaxThreadCount;

		//	THREADPOOL_GROUP_PARAMETER(ThreadpoolGroup dwThreadpoolGroup, int iMinThreadCount, int iMaxThreadCount)
		//	{
		//		_dwThreadpoolGroup = dwThreadpoolGroup;
		//		_iMinThreadCount = iMinThreadCount;
		//		_iMaxThreadCount = iMaxThreadCount;
		//	}
		//};

		// �׷캰 ������Ǯ�� �����ϱ� ���� ����ü
		struct THREADPOOL_GROUP_DATA
		{
			PTP_CALLBACK_ENVIRON			_pTpCallbackEnvironment;
			PTP_POOL						_pTpPool;
			ThreadpoolCallbackObjectSet		_threadpoolCallbackObjectSet;

			THREADPOOL_GROUP_DATA()
			{
				_pTpCallbackEnvironment = NULL;
				_pTpPool = NULL;
				_threadpoolCallbackObjectSet.clear();
			}
		};
		typedef std::map<ThreadpoolGroup, THREADPOOL_GROUP_DATA> ThreadpoolGroupDataMap;

		class CThreadpoolGroupManager
		{
			// �̱���
		private:
			// ������Ǯ �׷� �Ŵ����� ���α׷��� �����ϰ� 1���� �����ؾ� ��
			static CThreadpoolGroupManager* _pInstance;
			CThreadpoolGroupManager();

		public:
			~CThreadpoolGroupManager();
			static void Destroy();
			static CThreadpoolGroupManager* GetInstance();

		private:
			// ������ Ǯ �׷� ������
			// key:�ĺ���, value:THREADPOOL_GROUP_DATA
			ThreadpoolGroupDataMap _threadpoolGroupDataMap;
			CRITICAL_SECTION _criticalSection;

		public:
			// Ư�� �׷����� ������Ǯ ����/����
			// �ּ� ������ ������ �ִ� ������ ������ ���� ������ ���� ������Ǯ ���� ����
			BOOL CreateThreadpool(ThreadpoolGroup dwThreadpoolGroup, int iMinThreadCount, int iMaxThreadCount, ERROR_CODE& errorCode);
			BOOL DestroyThreadpool(ThreadpoolGroup dwThreadpoolGroup, ERROR_CODE& errorCode);

			// Ư�� �׷��� ������Ǯ�� �ݹ� ��ü ����/����
			BOOL InsertThreadpoolCallbackObject(ThreadpoolGroup dwThreadpoolGroup, IThreadpoolCallbackObject* pThreadpoolCallbackObject, ERROR_CODE& errorCode);
			BOOL DeleteThreadpoolCallbackObject(IThreadpoolCallbackObject* pThreadpoolCallbackObject, BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode);
		};
	}
}