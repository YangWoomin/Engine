#include "Common.h"
#include "ExportCommon.h"
#include "Utilities.h"
#include "ThreadpoolGroupManager.h"
#include "ThreadpoolCallbackObject.h"
#include "ThreadpoolCallbackWait.h"

using namespace ThreadpoolEngine;
using namespace ThreadpoolGroupManager;

CThreadpoolCallbackWait::CThreadpoolCallbackWait()
{
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
	_pThreadpoolCallbackObject = ::CreateThreadpoolWait(CallbackThreadpoolCallbackWait, (PVOID)this, pTpCallbackEnviron);

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
	if (ERROR_CODE_NONE != errorCode)
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

VOID CThreadpoolCallbackWait::CallbackThreadpoolCallbackWait(PTP_CALLBACK_INSTANCE pInstance, PVOID pParam, PTP_WAIT pTpWait, TP_WAIT_RESULT tpWaitResult)
{
	CThreadpoolCallbackWait* pThreadpoolCallbackWait = (CThreadpoolCallbackWait*)pParam;

	// pThreadpoolCallbackWait�� ��ȿ�� �˻��� ����� ���� ����, �ܺο��� ������ �� ���ִ� �� �ۿ�
	pThreadpoolCallbackWait->CallbackThreadpoolCallbackObject(CALLBACK_DATA_PARAMETER(ERROR_CODE_NONE, tpWaitResult, pThreadpoolCallbackWait->GetObjectHandle()));
}

HANDLE CThreadpoolCallbackWait::GetObjectHandle()
{
	return _hObject;
}