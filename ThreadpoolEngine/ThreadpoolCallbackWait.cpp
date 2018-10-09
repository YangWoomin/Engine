#include "Common.h"
#include "Export.h"
#include "Utilities.h"
#include "ThreadpoolGroupManager.h" //
#include "ThreadpoolCallbackObject.h"
//#include "CallbackData.h"
//#include "ThreadpoolGroupManager.h" //
#include "ThreadpoolCallbackWait.h"

using namespace ThreadpoolEngine;
using namespace ThreadpoolGroupManager;

CThreadpoolCallbackWait::CThreadpoolCallbackWait()
{
	_pCallbackData = NULL;
	_hObject = INVALID_HANDLE_VALUE;
	memset(&_fileTime, 0, sizeof(_fileTime));
}

CThreadpoolCallbackWait::~CThreadpoolCallbackWait()
{
	
}

BOOL CThreadpoolCallbackWait::BindThreadpoolCallbackObject(const PTP_CALLBACK_ENVIRON pTpCallbackEnviron, ERROR_CODE& errorCode)
{
	// �ݹ� ��ü�� �����Ǿ� ������ �� ��, �ߺ� ȣ��� ����
	if (NULL != _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_OBJECT_ALREADY_BOUND;
		return FALSE;
	}

	// �ݹ� �����͸� �ݹ� �Լ��� �Ű� ������ ���� �� �ֵ��� ��
	// pTpCallbackEnviron�� NULL�� ���� �־ �˻����� ����
	_pThreadpoolCallbackObject = ::CreateThreadpoolWait(ThreadpoolCallbackWaitCallbackFunction, (PVOID)this, pTpCallbackEnviron);

	// �����ϸ� NULL�� ������
	if (NULL == _pThreadpoolCallbackObject)
	{
		errorCode = (ERROR_CODE)::GetLastError();
		return FALSE;
	}

	return TRUE;
}

BOOL CThreadpoolCallbackWait::ReleaseThreadpoolCallbackObject(BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode)
{
	if (NULL != _pThreadpoolCallbackObject)
	{
		::WaitForThreadpoolWaitCallbacks(_pThreadpoolCallbackObject, fCancelPendingCallbacks);
		::CloseThreadpoolWait(_pThreadpoolCallbackObject);
		_pThreadpoolCallbackObject = NULL;
	}

	return TRUE;
}

BOOL CThreadpoolCallbackWait::ExecuteThreadpoolCallbackWait(HANDLE hObject, ICallbackData* pCallbackData, FILETIME fileTime, BOOL bInfinite, BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode)
{
	// �ݹ� ��ü�� ���ε����� �ʾ���
	if (NULL == _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_OBJECT_NOT_BOUND;
		return FALSE;
	}

	// ���� �۾��� �Ϸ�� ������ ���
	::WaitForThreadpoolWaitCallbacks(_pThreadpoolCallbackObject, fCancelPendingCallbacks);

	errorCode = (ERROR_CODE)::GetLastError();
	if (ERROR_CODE_NONE != errorCode && ERROR_IO_PENDING != (DWORD)errorCode)
	{
		return FALSE;
	}

	// ����ȭ ��ü ����
	_hObject = hObject;

	// �ݹ� ������ ����
	_pCallbackData = pCallbackData;

	// �ð� ������ �ִٸ� ���� ������ ����
	if (FALSE == bInfinite)
	{
		_fileTime = fileTime;
		::SetThreadpoolWait(_pThreadpoolCallbackObject, _hObject, &_fileTime);
	}
	// ���� �����
	else
	{
		::SetThreadpoolWait(_pThreadpoolCallbackObject, _hObject, NULL);
	}

	errorCode = (ERROR_CODE)::GetLastError();
	if (ERROR_CODE_NONE != errorCode && ERROR_IO_PENDING != (DWORD)errorCode)
	{
		return FALSE;
	}

	return TRUE;
}

VOID CThreadpoolCallbackWait::ThreadpoolCallbackWaitCallbackFunction(PTP_CALLBACK_INSTANCE pInstance, PVOID pParam, PTP_WAIT pTpWait, TP_WAIT_RESULT tpWaitResult)
{
	CThreadpoolCallbackWait* pThreadpoolCallbackWait = (CThreadpoolCallbackWait*)pParam;

	// CThreadpoolCallbackWait�� ��ȿ�� �˻��� ����� ���� ����, �ܺο��� ������ �� ���ִ� �� �ۿ�
	ICallbackData* pCallbackData = pThreadpoolCallbackWait->GetCallbackData();
	if (NULL != pCallbackData)
	{
		if (NULL == pTpWait || NULL == pThreadpoolCallbackWait->GetThreadpoolCallbackObject())
		{
			pCallbackData->CallbackFunction(CALLBACK_DATA_PARAMETER(ERROR_CODE_THREADPOOL_CALLBACK_OBJECT_IS_NULL, tpWaitResult, pThreadpoolCallbackWait->GetObjectHandle()));
		}
		else
		{
			// �ݹ� �������� �ݹ� �Լ��� ȣ��
			pCallbackData->CallbackFunction(CALLBACK_DATA_PARAMETER(ERROR_CODE_NONE, tpWaitResult, pThreadpoolCallbackWait->GetObjectHandle()));
		}

		// �ݹ� �Լ� ȣ�� �� �ڵ����� �ݹ� ������ ������ ����ϸ� ����
		if (TRUE == pCallbackData->DeleteCallbackDataAutomatically())
		{
			delete pCallbackData;
		}
	}
}

ICallbackData* CThreadpoolCallbackWait::GetCallbackData()
{
	return _pCallbackData;
}

HANDLE CThreadpoolCallbackWait::GetObjectHandle()
{
	return _hObject;
}