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
}

CThreadpoolCallbackWork::~CThreadpoolCallbackWork()
{
	::DeleteCriticalSection(&_criticalSection);
}

BOOL CThreadpoolCallbackWork::BindThreadpoolCallbackObject(const PTP_CALLBACK_ENVIRON pTpCallbackEnviron, ERROR_CODE& errorCode)
{
	// �ݹ� ��ü�� �����Ǿ� ������ �� ��, �ߺ� ȣ��� ����
	if (NULL != _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_OBJECT_ALREADY_BOUND;
		return FALSE;
	}

	// �ݹ� �����͸� �ݹ� �Լ��� �Ű� ������ ���� �� �ֵ��� ��
	// pTpCallbackEnviron�� NULL�� ���� �־ �˻����� ����
	_pThreadpoolCallbackObject = ::CreateThreadpoolWork(CallbackThreadpoolCallbackWork, (PVOID)this, pTpCallbackEnviron);

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
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_OBJECT_NOT_BOUND;
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
	// STATUS_PENDING(ERROR_IO_PENDING)�� ���ϵ� �� ����
	errorCode = (ERROR_CODE)::GetLastError();
	if (ERROR_CODE_NONE != errorCode && ERROR_IO_PENDING != (DWORD)errorCode)
	{
		return FALSE;
	}

	return TRUE;
}

VOID CThreadpoolCallbackWork::CallbackThreadpoolCallbackWork(PTP_CALLBACK_INSTANCE pInstance, PVOID pParam, PTP_WORK pTpWork)
{
	CThreadpoolCallbackWork* pThreadpoolCallbackWork = (CThreadpoolCallbackWork*)pParam;

	// pThreadpoolCallbackWork�� ��ȿ�� �˻��� ����� ���� ����, �ܺο��� ������ �� ���ִ� �� �ۿ�
	// ���� ���� push�� �ݹ� �����ͺ��� ó��
	ICallbackData* pCallbackData = pThreadpoolCallbackWork->PopCallbackDataFromQueue();
	if (NULL != pCallbackData)
	{
		if (NULL == pThreadpoolCallbackWork->GetThreadpoolCallbackObject())
		{
			pCallbackData->CallbackFunction(CALLBACK_DATA_PARAMETER(ERROR_CODE_THREADPOOL_CALLBACK_OBJECT_IS_NULL));
		}
		else
		{
			// �ݹ� �������� �ݹ� �Լ��� ȣ��
			pCallbackData->CallbackFunction(CALLBACK_DATA_PARAMETER(ERROR_CODE_NONE));
		}

		// �ݹ� �Լ� ȣ�� �� �ڵ����� �ݹ� ������ ������ ����ϸ� ����
		if (TRUE == pCallbackData->DeleteCallbackDataAutomatically())
		{
			delete pCallbackData;
		}
	}
}

ICallbackData* CThreadpoolCallbackWork::PopCallbackDataFromQueue()
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