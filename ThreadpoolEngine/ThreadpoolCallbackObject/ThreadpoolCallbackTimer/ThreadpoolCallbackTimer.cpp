#include "Common.h"
#include "ExportCommon.h"
#include "Utilities.h"
#include "ThreadpoolGroupManager.h"
#include "ThreadpoolCallbackObject.h"
#include "ThreadpoolCallbackTimer.h"

using namespace ThreadpoolEngine;
using namespace ThreadpoolGroupManager;

CThreadpoolCallbackTimer::CThreadpoolCallbackTimer()
{
	_pCallbackData = NULL;
	memset(&_fileTime, 0, sizeof(_fileTime));
}

CThreadpoolCallbackTimer::~CThreadpoolCallbackTimer()
{

}

BOOL CThreadpoolCallbackTimer::BindThreadpoolCallbackObject(const PTP_CALLBACK_ENVIRON pTpCallbackEnviron, ERROR_CODE& errorCode)
{
	// �ݹ� ��ü�� �����Ǿ� ������ �� ��, �ߺ� ȣ��� ����
	if (NULL != _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_OBJECT_ALREADY_BOUND;
		return FALSE;
	}

	// �ݹ� �����͸� �ݹ� �Լ��� �Ű� ������ ���� �� �ֵ��� ��
	// pTpCallbackEnviron�� NULL�� ���� �־ �˻����� ����
	_pThreadpoolCallbackObject = ::CreateThreadpoolTimer(CallbackThreadpoolCallbackTimer, (PVOID)this, pTpCallbackEnviron);

	// �����ϸ� NULL�� ������
	if (NULL == _pThreadpoolCallbackObject)
	{
		errorCode = (ERROR_CODE)::GetLastError();
		return FALSE;
	}

	return TRUE;
}

BOOL CThreadpoolCallbackTimer::ReleaseThreadpoolCallbackObject(BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode)
{
	if (NULL != _pThreadpoolCallbackObject)
	{
		::SetThreadpoolTimer(_pThreadpoolCallbackObject, NULL, 0, 0);
		::WaitForThreadpoolTimerCallbacks(_pThreadpoolCallbackObject, fCancelPendingCallbacks);
		::CloseThreadpoolTimer(_pThreadpoolCallbackObject);
		_pThreadpoolCallbackObject = NULL;
	}

	return TRUE;
}

BOOL CThreadpoolCallbackTimer::ExecuteThreadpoolCallbackTimer(ICallbackData* pCallbackData, FILETIME fileTime, DWORD dwPeriodMillisecond, BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode)
{
	// �ݹ� ��ü�� ���ε����� �ʾ���
	if (NULL == _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_OBJECT_NOT_BOUND;
		return FALSE;
	}

	// ���� �۾��� �Ϸ�� ������ ���
	::SetThreadpoolTimer(_pThreadpoolCallbackObject, NULL, 0, 0);
	::WaitForThreadpoolTimerCallbacks(_pThreadpoolCallbackObject, fCancelPendingCallbacks);

	errorCode = (ERROR_CODE)::GetLastError();
	if (ERROR_CODE_NONE != errorCode)
	{
		return FALSE;
	}

	// �ݹ� ������ ����
	_pCallbackData = pCallbackData;

	// Ÿ�̸� ���� �ð� ������ ����
	_fileTime = fileTime;

	// Ÿ�̸� ����
	::SetThreadpoolTimer(_pThreadpoolCallbackObject, &_fileTime, dwPeriodMillisecond, 0);

	errorCode = (ERROR_CODE)::GetLastError();
	if (ERROR_CODE_NONE != errorCode && ERROR_IO_PENDING != (DWORD)errorCode)
	{
		return FALSE;
	}

	return TRUE;
}

VOID CThreadpoolCallbackTimer::CallbackThreadpoolCallbackTimer(PTP_CALLBACK_INSTANCE pInstance, PVOID pParam, PTP_TIMER pTpTimer)
{
	CThreadpoolCallbackTimer* pThreadpoolCallbackTimer = (CThreadpoolCallbackTimer*)pParam;

	// pThreadpoolCallbackWait�� ��ȿ�� �˻��� ����� ���� ����, �ܺο��� ������ �� ���ִ� �� �ۿ�
	pThreadpoolCallbackTimer->CallbackThreadpoolCallbackObject(CALLBACK_DATA_PARAMETER(ERROR_CODE_NONE));
}

BOOL CThreadpoolCallbackTimer::CancelThreadpoolCallbackTimer(BOOL bWaitForCallbacks, BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode)
{
	// �ݹ� ��ü�� ���ε����� �ʾ���
	if (NULL == _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_OBJECT_NOT_BOUND;
		return FALSE;
	}

	// ���̻� �ݹ� �Լ� ���ø� �������� �ʵ��� ó��
	::SetThreadpoolTimer(_pThreadpoolCallbackObject, NULL, 0, 0);

	// �ݹ� �Լ����� ������ ��� �Ϸ�Ǳ� ���Ѵٸ�
	if (FALSE != bWaitForCallbacks)
	{
		::WaitForThreadpoolTimerCallbacks(_pThreadpoolCallbackObject, fCancelPendingCallbacks);
	}
}