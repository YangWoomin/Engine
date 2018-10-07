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

	// ����Ʈ ������Ǯ�� ����
	ERROR_CODE errorCode = ERROR_CODE_NONE;
	CreateThreadpool(DEFAULT_THREAD_POOL, 0, 0, errorCode);
}

CThreadpoolGroupManager::~CThreadpoolGroupManager()
{
	ERROR_CODE errorCode = ERROR_CODE_NONE;

	{
		// �� �Լ� �������� ũ��Ƽ�� ������ �����ϹǷ� �� �� ���ο��� CLock �Ҹ��ڰ� ȣ��Ǿ�� ��
		// for�� ���� �Լ����� �ٽ� ���� ������ ���� �����尡 �ߺ����� �ɾ Enter/Leave Ƚ���� �����ϸ� �������� ����
		CLock lock(_criticalSection);
		ThreadpoolGroupDataMap::iterator itCursorGroup = _threadpoolGroupDataMap.begin();
		ThreadpoolGroupDataMap::iterator itFinishGroup = _threadpoolGroupDataMap.end();
		while (itCursorGroup != itFinishGroup)
		{
			// ����Ʈ ������Ǯ�̸� ���⼭ ���� ����
			if (DEFAULT_THREAD_POOL == itCursorGroup->first)
			{
				ThreadpoolCallbackObjectSet::iterator itCursorObject 
					= itCursorGroup->second._threadpoolCallbackObjectSet.begin();
				ThreadpoolCallbackObjectSet::iterator itFinishObject
					= itCursorGroup->second._threadpoolCallbackObjectSet.end();
				for (; itCursorObject != itFinishObject; ++itCursorObject)
				{
					// ReleaseThreadpoolCallbackObject���� �ش� �ݹ� ��ü�� �����Ǵ� ���� �����
					// �� �Ҹ���(���α׷� ������� ȣ��)���� �ݹ� ��ü���� �����ϴ� ���� �ſ� �����ϰ����� (dangling pointer�� �� ����)
					// ���� ACCESS_VIOLATION�� �����Ѽ� �޸� ������ ������ �˸��°� ������
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
			// �ش� �׷��� TP_CALLBACK_ENVRION ��ü�� ������ ����
			errorCode = ERROR_CODE_THREADPOOL_THREADPOOL_GROUP_ALREADY_EXISTS;
			return FALSE;
		}
		else
		{
			THREADPOOL_GROUP_DATA threadpoolGroupData;
			try
			{
				// ����Ʈ ������Ǯ�̸� TP_CALLBACK_ENVIRON�� TP_POOL�� �������� ����
				if (DEFAULT_THREAD_POOL == dwThreadpoolGroup)
				{
					threadpoolGroupData._pTpCallbackEnvironment = NULL;
					threadpoolGroupData._pTpPool = NULL;
				}
				else
				{
					// ������Ǯ ������ �ּ� ���� ���� ó��
					if (THREADPOOL_THREAD_MIN_COUNT > iMinThreadCount)
					{
						throw (DWORD)ERROR_CODE_THREADPOOL_INVALID_THREAD_MIN_COUNT;
					}

					// ������Ǯ ������ �ִ� ���� ���� ó��
					if (THREADPOOL_THREAD_MAX_COUNT < iMaxThreadCount)
					{
						throw (DWORD)ERROR_CODE_THREADPOOL_INVALID_THREAD_MAX_COUNT;
					}

					// TP_CALLBACK_ENVIRON �ʱ�ȭ
					threadpoolGroupData._pTpCallbackEnvironment = new TP_CALLBACK_ENVIRON();
					if (NULL == threadpoolGroupData._pTpCallbackEnvironment)
					{
						throw ::GetLastError();
					}
					::InitializeThreadpoolEnvironment(threadpoolGroupData._pTpCallbackEnvironment);

					// TP_POOL �ʱ�ȭ
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

					// TP_POOL�� TP_CALLBACK_ENVIRON�� ���ε�
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
	// ����Ʈ ������Ǯ�� ������ �� ����
	// CThreadpoolGroupManager �Ҹ��ڿ��� ����
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
			// �ش� ������Ǯ�� ���ε��� �ݹ� ��ü�鿡 ���� ���� �۾��� ����
			ThreadpoolCallbackObjectSet::iterator itCursor 
				= itFinder->second._threadpoolCallbackObjectSet.begin();
			ThreadpoolCallbackObjectSet::iterator itFinish 
				= itFinder->second._threadpoolCallbackObjectSet.end();
			for (; itCursor != itFinish; ++itCursor)
			{
				// ReleaseThreadpoolCallbackObject���� �ش� �ݹ� ��ü�� �����Ǵ� ���� �����
				((IThreadpoolCallbackObject*)(*itCursor))->ReleaseThreadpoolCallbackObject(TRUE, errorCode);
			}

			// ������Ǯ ����
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
		// �ݹ� ��ü�� ��ü �׷쿡 ���ؼ� �����ϴ����� �˻��ϸ� �������� �� ����� ���Ǵ� ���� �����̹Ƿ� ����
		ThreadpoolGroupDataMap::iterator itFinder = _threadpoolGroupDataMap.find(dwThreadpoolGroup);
		if (_threadpoolGroupDataMap.end() != itFinder)
		{
			std::pair<ThreadpoolCallbackObjectSet::iterator, bool> pairInsertResult 
				= itFinder->second._threadpoolCallbackObjectSet.insert(pThreadpoolCallbackObject);
			// �̹� �����Ѵٸ�
			if (false == pairInsertResult.second)
			{
				errorCode = ERROR_CODE_THREADPOOL_CALLBACK_OBJECT_ALREADY_EXISTS_IN_THREADPOOL_GROUP;
				return FALSE;
			}

			// �ݹ� ��ü�� TP_CALLBACK_ENVIRON�� ���ε��ϵ��� ȣ��
			BOOL bResult = pThreadpoolCallbackObject->BindThreadpoolCallbackObject(itFinder->second._pTpCallbackEnvironment, errorCode);

			// �����ߴٸ� set���� ����
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
		// ������Ǯ �׷��� ���� ������ ���� ���̶� ���ɿ� ū ������ ������
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