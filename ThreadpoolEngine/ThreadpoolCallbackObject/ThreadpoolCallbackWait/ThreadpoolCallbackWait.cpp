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

	// �� �����忡 ���õǾ� �ִ� �ݹ� ��ü �ν��Ͻ��� �� Ŭ���� �ν��Ͻ��� �ݹ� ��ü�� ����
	// �ݹ� �ν��Ͻ� ���� ���õǾ� �ִٸ� �ݹ� �Լ� ���ο��� �� �Լ�(ExecuteThreadpoolCallbackWait)�� ȣ���ϴ� ���̹Ƿ� ������� ���ƾ� ��
	if (_pThreadpoolCallbackObject == _pThreadpoolCallbackObjectInstance && NULL != _pTpCallbackInstance)
	{
		::DisassociateCurrentThreadFromCallback(_pTpCallbackInstance);
		_pTpCallbackInstance = NULL;
		_pThreadpoolCallbackObjectInstance = NULL;
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

VOID CThreadpoolCallbackWait::CallbackThreadpoolCallbackWait(PTP_CALLBACK_INSTANCE pTpCallbackInstance, PVOID pParam, PTP_WAIT pTpWait, TP_WAIT_RESULT tpWaitResult)
{
	CThreadpoolCallbackWait* pThreadpoolCallbackWait = (CThreadpoolCallbackWait*)pParam;

	// pThreadpoolCallbackWait�� ��ȿ�� �˻��� ����� ���� ����, �ܺο��� ������ �� ���ִ� �� �ۿ�
	pThreadpoolCallbackWait->CallbackThreadpoolCallbackObject(CALLBACK_DATA_PARAMETER(ERROR_CODE_NONE, tpWaitResult, pThreadpoolCallbackWait->GetObjectHandle()), pTpCallbackInstance);
}

BOOL CThreadpoolCallbackWait::CancelThreadpoolCallbackWait(BOOL bWaitForCallbacks, BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode)
{
	// �ݹ� ��ü�� ���ε����� �ʾ���
	if (NULL == _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_OBJECT_NOT_BOUND;
		return FALSE;
	}

	// ���̻� �ݹ� �Լ� ���ø� �������� �ʵ��� ó��
	::SetThreadpoolWait(_pThreadpoolCallbackObject, NULL, NULL);
	errorCode = (ERROR_CODE)::GetLastError();

	// �ݹ� �Լ����� ������ ��� �Ϸ�Ǳ� ���Ѵٸ�
	if (FALSE != bWaitForCallbacks)
	{
		// �� �����忡 ���õǾ� �ִ� �ݹ� ��ü �ν��Ͻ��� �� Ŭ���� �ν��Ͻ��� �ݹ� ��ü�� ����
		// �ݹ� �ν��Ͻ� ���� ���õǾ� �ִٸ� �ݹ� �Լ� ���ο��� �� �Լ�(CancelThreadpoolCallbackWait)�� ȣ���ϴ� ���̹Ƿ� ������� ���ƾ� ��
		if (_pThreadpoolCallbackObject == _pThreadpoolCallbackObjectInstance && NULL != _pTpCallbackInstance)
		{
			::DisassociateCurrentThreadFromCallback(_pTpCallbackInstance);
			_pTpCallbackInstance = NULL;
			_pThreadpoolCallbackObjectInstance = NULL;
		}

		::WaitForThreadpoolWaitCallbacks(_pThreadpoolCallbackObject, fCancelPendingCallbacks);
		errorCode = (ERROR_CODE)::GetLastError();
	}

	return ERROR_CODE_NONE == errorCode;
}

HANDLE CThreadpoolCallbackWait::GetObjectHandle()
{
	return _hObject;
}