#include "Common.h"
#include "Export.h"
#include "Utilities.h"
#include "ThreadpoolCallbackObject.h"
//#include "CallbackData.h"
#include "ThreadpoolGroupManager.h"

using namespace ThreadpoolGroupManager;

CThreadpoolGroupManager* CThreadpoolGroupManager::_pInstance = NULL;

CThreadpoolGroupManager::CThreadpoolGroupManager()
{
	::InitializeCriticalSection(&_criticalSection);

	// 디폴트 스레드풀을 생성
	ERROR_CODE errorCode = ERROR_CODE_NONE;
	CreateThreadpool(DEFAULT_THREAD_POOL, 0, 0, errorCode);
}

CThreadpoolGroupManager::~CThreadpoolGroupManager()
{
	ERROR_CODE errorCode = ERROR_CODE_NONE;

	{
		// 이 함수 마지막에 크리티컬 섹션을 삭제하므로 이 블럭 내부에서 CLock 소멸자가 호출되어야 함
		// for문 안의 함수에서 다시 락을 걸지만 동일 스레드가 중복으로 걸어도 Enter/Leave 횟수가 동일하면 문제되지 않음
		CLock lock(_criticalSection);
		ThreadpoolGroupDataMap::iterator itCursorGroup = _threadpoolGroupDataMap.begin();
		ThreadpoolGroupDataMap::iterator itFinishGroup = _threadpoolGroupDataMap.end();
		while (itCursorGroup != itFinishGroup)
		{
			// 디폴트 스레드풀이면 여기서 직접 해제
			if (DEFAULT_THREAD_POOL == itCursorGroup->first)
			{
				ThreadpoolCallbackObjectSet::iterator itCursorObject 
					= itCursorGroup->second._threadpoolCallbackObjectSet.begin();
				ThreadpoolCallbackObjectSet::iterator itFinishObject
					= itCursorGroup->second._threadpoolCallbackObjectSet.end();
				for (; itCursorObject != itFinishObject; ++itCursorObject)
				{
					// ReleaseThreadpoolCallbackObject에서 해당 콜백 객체가 해제되는 것이 보장됨
					// 이 소멸자(프로그램 종료시점 호출)에서 콜백 객체들을 참조하는 것은 매우 위험하겠지만 (dangling pointer일 수 있음)
					// 차라리 ACCESS_VIOLATION을 일으켜서 메모리 누수가 있음을 알리는게 나을듯
					((IThreadpoolCallbackObject*)(*itCursorObject))->ReleaseThreadpoolCallbackObject(TRUE, errorCode);
				}

				++itCursorGroup;
			}
			else
			{
				ERROR_CODE errorCode = ERROR_CODE_NONE;
				ThreadpoolGroup dwThreadpoolGroup = itCursorGroup->first;
				++itCursorGroup;
				DestroyThreadpool(dwThreadpoolGroup, errorCode);
			}
		}
	}

	::DeleteCriticalSection(&_criticalSection);
}

void CThreadpoolGroupManager::Destroy()
{
	if (NULL != _pInstance)
	{
		delete _pInstance;
		_pInstance = NULL;
	}
}

CThreadpoolGroupManager* CThreadpoolGroupManager::GetInstance()
{
	if (NULL == _pInstance)
	{
		_pInstance = new CThreadpoolGroupManager();
		atexit(Destroy);
	}

	return _pInstance;
}


BOOL CThreadpoolGroupManager::CreateThreadpool(ThreadpoolGroup dwThreadpoolGroup, int iMinThreadCount, int iMaxThreadCount, ERROR_CODE& errorCode)
{
	CLock lock(_criticalSection);
	{
		ThreadpoolGroupDataMap::iterator itFinder = _threadpoolGroupDataMap.find(dwThreadpoolGroup);
		if(_threadpoolGroupDataMap.end() != itFinder)
		{
			// 해당 그룹의 TP_CALLBACK_ENVRION 객체가 있으면 실패
			errorCode = ERROR_CODE_THREADPOOL_THREADPOOL_GROUP_ALREADY_EXISTS;
			return FALSE;
		}
		else
		{
			THREADPOOL_GROUP_DATA threadpoolGroupData;
			try
			{
				// 디폴트 스레드풀이면 TP_CALLBACK_ENVIRON와 TP_POOL을 생성하지 않음
				if (DEFAULT_THREAD_POOL == dwThreadpoolGroup)
				{
					threadpoolGroupData._pTpCallbackEnvironment = NULL;
					threadpoolGroupData._pTpPool = NULL;
				}
				else
				{
					// 스레드풀 스레드 최소 개수 예외 처리
					if (THREADPOOL_THREAD_MIN_COUNT > iMinThreadCount)
					{
						throw (DWORD)ERROR_CODE_THREADPOOL_INVALID_THREAD_MIN_COUNT;
					}

					// 스레드풀 스레드 최대 개수 예외 처리
					if (THREADPOOL_THREAD_MAX_COUNT < iMaxThreadCount)
					{
						throw (DWORD)ERROR_CODE_THREADPOOL_INVALID_THREAD_MAX_COUNT;
					}

					// TP_CALLBACK_ENVIRON 초기화
					threadpoolGroupData._pTpCallbackEnvironment = new TP_CALLBACK_ENVIRON();
					if (NULL == threadpoolGroupData._pTpCallbackEnvironment)
					{
						throw ::GetLastError();
					}
					::InitializeThreadpoolEnvironment(threadpoolGroupData._pTpCallbackEnvironment);

					// TP_POOL 초기화
					threadpoolGroupData._pTpPool = ::CreateThreadpool(NULL);
					if (NULL == threadpoolGroupData._pTpPool)
					{
						throw ::GetLastError();
					}
					::SetThreadpoolThreadMaximum(threadpoolGroupData._pTpPool, iMaxThreadCount);
					if (FALSE == ::SetThreadpoolThreadMinimum(threadpoolGroupData._pTpPool, iMinThreadCount))
					{
						throw ::GetLastError();
					}

					// TP_POOL을 TP_CALLBACK_ENVIRON에 바인딩
					::SetThreadpoolCallbackPool(threadpoolGroupData._pTpCallbackEnvironment, threadpoolGroupData._pTpPool);
				}
			}
			catch (DWORD dwErrorCode)
			{
				if (NULL != threadpoolGroupData._pTpPool)
				{
					::CloseThreadpool(threadpoolGroupData._pTpPool);
				}

				if (NULL != threadpoolGroupData._pTpCallbackEnvironment)
				{
					delete threadpoolGroupData._pTpCallbackEnvironment;
				}

				errorCode = (ERROR_CODE)dwErrorCode;
				return FALSE;
			}
			
			_threadpoolGroupDataMap.insert(ThreadpoolGroupDataMap::value_type(dwThreadpoolGroup, threadpoolGroupData));

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CThreadpoolGroupManager::DestroyThreadpool(ThreadpoolGroup dwThreadpoolGroup, ERROR_CODE& errorCode)
{
	// 디폴트 스레드풀은 제거할 수 없음
	// CThreadpoolGroupManager 소멸자에서 제거
	if (DEFAULT_THREAD_POOL == dwThreadpoolGroup)
	{
		errorCode = ERROR_CODE_THREADPOOL_CANNOT_DESTROY_DEFAULT_THREADPOOL_EXPLICITLY;
		return FALSE;
	}

	CLock lock(_criticalSection);
	{
		ThreadpoolGroupDataMap::iterator itFinder = _threadpoolGroupDataMap.find(dwThreadpoolGroup);
		if (_threadpoolGroupDataMap.end() != itFinder)
		{
			// 해당 스레드풀에 바인딩된 콜백 객체들에 대해 해제 작업을 수행
			ThreadpoolCallbackObjectSet::iterator itCursor 
				= itFinder->second._threadpoolCallbackObjectSet.begin();
			ThreadpoolCallbackObjectSet::iterator itFinish 
				= itFinder->second._threadpoolCallbackObjectSet.end();
			for (; itCursor != itFinish; ++itCursor)
			{
				// ReleaseThreadpoolCallbackObject에서 해당 콜백 객체가 해제되는 것이 보장됨
				((IThreadpoolCallbackObject*)(*itCursor))->ReleaseThreadpoolCallbackObject(TRUE, errorCode);
			}

			// 스레드풀 해제
			PTP_POOL pTpPool = itFinder->second._pTpPool;
			if (NULL != pTpPool)
			{
				::CloseThreadpool(pTpPool);
			}

			PTP_CALLBACK_ENVIRON pTpCallbackEnviron = itFinder->second._pTpCallbackEnvironment;
			if (NULL != pTpCallbackEnviron)
			{
				::DestroyThreadpoolEnvironment(pTpCallbackEnviron);
				delete pTpCallbackEnviron;
			}

			_threadpoolGroupDataMap.erase(itFinder);

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CThreadpoolGroupManager::InsertThreadpoolCallbackObject(ThreadpoolGroup dwThreadpoolGroup, IThreadpoolCallbackObject* pThreadpoolCallbackObject, ERROR_CODE& errorCode)
{
	CLock lock(_criticalSection);
	{
		// 콜백 객체가 전체 그룹에 대해서 존재하는지도 검사하면 좋겠지만 이 모듈이 사용되는 곳도 엔진이므로 배제
		ThreadpoolGroupDataMap::iterator itFinder = _threadpoolGroupDataMap.find(dwThreadpoolGroup);
		if (_threadpoolGroupDataMap.end() != itFinder)
		{
			std::pair<ThreadpoolCallbackObjectSet::iterator, bool> pairInsertResult 
				= itFinder->second._threadpoolCallbackObjectSet.insert(pThreadpoolCallbackObject);
			// 이미 존재한다면
			if (false == pairInsertResult.second)
			{
				errorCode = ERROR_CODE_THREADPOOL_CALLBACK_OBJECT_ALREADY_EXISTS_IN_THREADPOOL_GROUP;
				return FALSE;
			}

			// 콜백 객체와 TP_CALLBACK_ENVIRON을 바인딩하도록 호출
			BOOL bResult = pThreadpoolCallbackObject->BindThreadpoolCallbackObject(itFinder->second._pTpCallbackEnvironment, errorCode);

			// 실패했다면 set에서 제거
			if (FALSE == bResult)
			{
				itFinder->second._threadpoolCallbackObjectSet.erase(pairInsertResult.first);
			}

			return bResult;
		}
	}

	return FALSE;
}

BOOL CThreadpoolGroupManager::DeleteThreadpoolCallbackObject(IThreadpoolCallbackObject* pThreadpoolCallbackObject, BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode)
{
	CLock lock(_criticalSection);
	{
		// 스레드풀 그룹이 많이 사용되지 않을 것이라 성능에 큰 지장은 없을듯
		ThreadpoolGroupDataMap::iterator itCursor = _threadpoolGroupDataMap.begin();
		ThreadpoolGroupDataMap::iterator itFinish = _threadpoolGroupDataMap.end();
		for (; itCursor != itFinish; ++itCursor)
		{
			ThreadpoolCallbackObjectSet& threadpoolCallbackObjectSet = itCursor->second._threadpoolCallbackObjectSet;
			ThreadpoolCallbackObjectSet::iterator itFinder = threadpoolCallbackObjectSet.find(pThreadpoolCallbackObject);
			if (threadpoolCallbackObjectSet.end() != itFinder)
			{
				BOOL bResult = ((IThreadpoolCallbackObject*)(*itFinder))->ReleaseThreadpoolCallbackObject(fCancelPendingCallbacks, errorCode);
				threadpoolCallbackObjectSet.erase(itFinder);
				return bResult;
			}
		}
	}

	return FALSE;
}