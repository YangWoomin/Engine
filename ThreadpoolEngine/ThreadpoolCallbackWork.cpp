#include "Common.h"
#include "Export.h"
#include "Utilities.h"
#include "ThreadpoolGroupManager.h" //
#include "ThreadpoolCallbackObject.h"
//#include "CallbackData.h"
//#include "ThreadpoolGroupManager.h" //
#include "ThreadpoolCallbackWork.h"

using namespace ThreadpoolEngine;
using namespace ThreadpoolGroupManager;

CThreadpoolCallbackWork::CThreadpoolCallbackWork()
{
	::InitializeCriticalSection(&_criticalSection);
	
	//_pThreadpoolCallbackObject = NULL;

	//// Ư�� �׷��� ������Ǯ ����, ������ �����ϰ� ������ �׳� ���� ��
	//ThreadpoolGroupManager()->CreateThreadpool(threadpoolGroupParameter._dwThreadpoolGroup, threadpoolGroupParameter._iMinThreadCount, threadpoolGroupParameter._iMaxThreadCount, errorCode);

	//// ������Ǯ Ư�� �׷쿡 �ݹ� ��ü�� �߰�
	//if (ERROR_CODE_THREADPOOL_THREADPOOL_GROUP_ALREADY_EXISTS == errorCode || ERROR_CODE_NONE == errorCode)
	//{
	//	// ������Ǯ �׷� �Ŵ����� ���� BindThreadpoolCallbackObject ȣ���
	//	errorCode = ERROR_CODE_NONE;
	//	_bIsGrouping = ThreadpoolGroupManager()->InsertThreadpoolCallbackObject(threadpoolGroupParameter._dwThreadpoolGroup, this, errorCode);
	//}
}

CThreadpoolCallbackWork::~CThreadpoolCallbackWork()
{
	//// Ư�� �׷��� ������Ǯ�� �߰��Ǿ��ٸ� ����
	//if (TRUE == _bIsGrouping)
	//{
	//	// ������Ǯ �׷� �Ŵ����� ���� ReleaseThreadpoolCallbackObject ȣ���
	//	ERROR_CODE errorCode = ERROR_CODE_NONE;
	//	ThreadpoolGroupManager()->DeleteThreadpoolCallbackObject(this, FALSE, errorCode);
	//	_bIsGrouping = FALSE;
	//}

	//// �����ߴ� ������ Ǯ�� ���⼭ �������� �ʵ��� ��
	//// ������Ǯ �׷� �Ŵ����� ���α׷� ������ �� �ϰ������� �ڵ� ����

	::DeleteCriticalSection(&_criticalSection);
}

BOOL CThreadpoolCallbackWork::BindThreadpoolCallbackObject(const PTP_CALLBACK_ENVIRON pTpCallbackEnviron, ERROR_CODE& errorCode)
{
	// �ݹ� ��ü�� �����Ǿ� ������ �� ��, �ߺ� ȣ��� ����
	if (NULL != _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_WORK_ALREADY_BOUND;
		return FALSE;
	}

	// �ݹ� �����͸� �ݹ� �Լ��� �Ű� ������ ���� �� �ֵ��� ��
	// pTpCallbackEnviron�� NULL�� ���� �־ �˻����� ����
	_pThreadpoolCallbackObject = ::CreateThreadpoolWork(ThreadpoolCallbackWorkCallbackFunction, (PVOID)this, pTpCallbackEnviron);

	// �����ϸ� NULL�� ������
	if (NULL == _pThreadpoolCallbackObject)
	{
		errorCode = (ERROR_CODE)::GetLastError();
		return FALSE;
	}

	return TRUE;
}

BOOL CThreadpoolCallbackWork::ReleaseThreadpoolCallbackObject(BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode)
{
	if (NULL != _pThreadpoolCallbackObject)
	{
		::WaitForThreadpoolWorkCallbacks(_pThreadpoolCallbackObject, fCancelPendingCallbacks);
		::CloseThreadpoolWork(_pThreadpoolCallbackObject);
		_pThreadpoolCallbackObject = NULL;
	}

	return TRUE;
}

BOOL CThreadpoolCallbackWork::ExecuteThreadpoolCallbackWork(ICallbackData* pCallbackData, ERROR_CODE& errorCode)
{
	// �ݹ� ��ü�� ���ε����� �ʾ���
	if (NULL == _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_WORK_NOT_BOUND;
		return FALSE;
	}

	{
		CLock lock(_criticalSection);
		// �ݹ� ������ ť�� �ݹ� �����͸� �߰�
		_callbackDataQueue.push(pCallbackData);
	}

	// �ݹ� �Լ� ȣ�� ����
	::SubmitThreadpoolWork(_pThreadpoolCallbackObject);

	// submit ��� �̻� ���� Ȯ��
	errorCode = (ERROR_CODE)::GetLastError();
	if (ERROR_CODE_NONE != errorCode)
	{
		return FALSE;
	}

	// �Ʒ� �ڵ�� Ȥ�ó� �� ���� ������� �켱 ���� ������ ��
	/*
	// �ݹ� ��ü�� �ݹ� �Լ� ȣ�� ���� Ƚ���� �ִ�ġ�� �����ϸ� ���� ó��
	if (_MAXIMUM_CALLBACK_RUNNING_COUNT <= GetCallbackRunningCount())
	{
	errorCode = ERROR_CODE_THREADPOOL_CALLBACK_WORK_CALLBACK_COUNT_MAX;
	return FALSE;
	}

	// ������ �ݹ� �Լ� ȣ���� �Ϸ�� �� �ݹ� �Լ� ȣ�� ���ø� �ϰ� �ʹٸ�
	if (TRUE == bWaitForPreviousCallback)
	{
	// ���� �ݹ� �Լ��� �۾��� �Ϸ�ǵ��� ��� ���
	::WaitForThreadpoolWorkCallbacks(_pThreadpoolCallbackObject, fCancelPendingCallbacks);

	// ���� ���� �ݹ� �Լ��� �۾��� ��� ����Ѵٸ�
	if (TRUE == fCancelPendingCallbacks)
	{
	// �ݹ� �Լ� ȣ�� ���� Ƚ���� 0���� �ʱ�ȭ
	ResetCallbackRunningCount();
	}
	}

	// �ݹ� �Լ� ȣ�� ���� Ƚ�� ����
	IncreaseCallbackRunningCount();
	*/

	return TRUE;
}

VOID CThreadpoolCallbackWork::ThreadpoolCallbackWorkCallbackFunction(PTP_CALLBACK_INSTANCE pInstance, PVOID pParam, PTP_WORK pTpWork)
{
	CThreadpoolCallbackWork* pThreadpoolCallbackWork = (CThreadpoolCallbackWork*)pParam;

	// pThreadpoolCallbackWork�� ��ȿ�� �˻��� ����� ���� ����, �ܺο��� ������ �� ���ִ� �� �ۿ�
	// ���� ���� push�� �ݹ� �����ͺ��� ó��
	ICallbackData* pCallbackData = pThreadpoolCallbackWork->GetCallbackDataFromQueue();
	if (NULL != pCallbackData)
	{
		if (NULL == pTpWork || NULL == pThreadpoolCallbackWork->GetThreadpoolCallbackObject())
		{
			pCallbackData->CallbackFunction(CALLBACK_DATA_PARAMETER(ERROR_CODE_THREADPOOL_CALLBACK_WORK_PTP_WORK_IS_NULL, 0, NULL, 0, 0));
		}
		else
		{
			// �ݹ� �������� �ݹ� �Լ��� ȣ��
			pCallbackData->CallbackFunction(CALLBACK_DATA_PARAMETER(ERROR_CODE_NONE, 0, NULL, 0, 0));
		}

		// �ݹ� �Լ� ȣ�� �� �ڵ����� �ݹ� ������ ������ ����ϸ� ����
		if (TRUE == pCallbackData->DeleteCallbackDataAutomatically())
		{
			delete pCallbackData;
		}
	}
}

// �Ʒ� �ڵ�� Ȥ�ó� �� ���� ������� �켱 ���� ������ ��
/*
BOOL CThreadpoolCallbackWork::SetCallbackData(ICallbackData* pCallbackData, BOOL bWaitForPreviousCallback, BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode)
{
	errorCode = ERROR_CODE_NONE;

	// �ݹ� ��ü�� ���ε����� �ʾ���
	if (NULL != _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_WORK_NOT_BOUND;
		return FALSE;
	}

	// ���� �ݹ� �Լ� ȣ�� ���õ� Ƚ���� 0���� ũ�ٸ�
	if (0 < GetCallbackRunningCount())
	{
		// ��ٸ��� �ʴ´ٸ� ���� ó��
		if (FALSE == bWaitForPreviousCallback)
		{
			errorCode = ERROR_CODE_THREADPOOL_CALLBACK_WORK_CALLBACK_FUNCTION_RUNNING;
			return FALSE;
		}

		// ���� �ݹ� �Լ� ȣ���� �Ϸ�� ������ ���
		::WaitForThreadpoolWorkCallbacks(_pThreadpoolCallbackObject, fCancelPendingCallbacks);

		// ���� �ݹ� �Լ��� �۾��� ��� ����Ѵٸ�
		if (TRUE == fCancelPendingCallbacks)
		{
			// �ݹ� �Լ� ȣ�� ���� Ƚ���� 0���� �ʱ�ȭ
			ResetCallbackRunningCount();
		}
	}
	
	// �ݹ� ������ ����
	_pCallbackData = pCallbackData;

	return TRUE;
}
*/

ICallbackData* CThreadpoolCallbackWork::GetCallbackDataFromQueue()
{
	ICallbackData* pCallbackData = NULL;

	{
		CLock lock(_criticalSection);
		if (false == _callbackDataQueue.empty())
		{
			pCallbackData = _callbackDataQueue.front();
			_callbackDataQueue.pop();
		}
	}

	return pCallbackData;
}