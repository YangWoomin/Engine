#pragma once

namespace ThreadpoolEngine
{
	interface IThreadpoolCallbackObject;

	namespace ThreadpoolGroupManager
	{

		//#define DEFAULT_THREAD_POOL 0
#define ThreadpoolGroupManager() CThreadpoolGroupManager::GetInstance()

		//typedef DWORD ThreadpoolGroup; // DEFAULT_THREAD_POOL이 아닌 0보다 큰 값

		typedef std::set<IThreadpoolCallbackObject*> ThreadpoolCallbackObjectSet;

		//// CreateThreadpool 매개 변수 묶음 구조체
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

		// 그룹별 스레드풀을 관리하기 위한 구조체
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
			// 싱글톤
		private:
			// 스레드풀 그룹 매니저는 프로그램에 유일하게 1개만 존재해야 함
			static CThreadpoolGroupManager* _pInstance;
			CThreadpoolGroupManager();

		public:
			~CThreadpoolGroupManager();
			static void Destroy();
			static CThreadpoolGroupManager* GetInstance();

		private:
			// 스레드 풀 그룹 데이터
			// key:식별자, value:THREADPOOL_GROUP_DATA
			ThreadpoolGroupDataMap _threadpoolGroupDataMap;
			CRITICAL_SECTION _criticalSection;

		public:
			// 특정 그룹으로 스레드풀 생성/삭제
			// 최소 스레드 개수와 최대 스레드 개수로 영속 스레드 전용 스레드풀 생성 가능
			BOOL CreateThreadpool(ThreadpoolGroup dwThreadpoolGroup, int iMinThreadCount, int iMaxThreadCount, ERROR_CODE& errorCode);
			BOOL DestroyThreadpool(ThreadpoolGroup dwThreadpoolGroup, ERROR_CODE& errorCode);

			// 특정 그룹의 스레드풀에 콜백 객체 삽입/제거
			BOOL InsertThreadpoolCallbackObject(ThreadpoolGroup dwThreadpoolGroup, IThreadpoolCallbackObject* pThreadpoolCallbackObject, ERROR_CODE& errorCode);
			BOOL DeleteThreadpoolCallbackObject(IThreadpoolCallbackObject* pThreadpoolCallbackObject, BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode);
		};
	}
}