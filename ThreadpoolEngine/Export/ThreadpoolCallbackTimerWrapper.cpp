#include "Common.h"
#include "ExportCommon.h"
#include "Utilities.h"
#include "ThreadpoolGroupManager.h"
#include "ThreadpoolCallbackObject.h"
#include "ThreadpoolCallbackTimer.h"
#include "ThreadpoolCallbackTimerWrapper.h"

using namespace ThreadpoolEngine;
using namespace ThreadpoolGroupManager;

CThreadpoolCallbackTimerWrapper::CThreadpoolCallbackTimerWrapper(THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode)
	: CThreadpoolCallbackObjectWrapper(threadpoolGroupParameter, errorCode)
{

}

CThreadpoolCallbackTimerWrapper::~CThreadpoolCallbackTimerWrapper()
{

}

BOOL CThreadpoolCallbackTimerWrapper::ExecuteThreadpoolCallbackTimer(ICallbackData* pCallbackData, SYSTEMTIME callbackTime, BOOL bConvertToUtcTime, ULONG ulPeriodMillisecond, BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode)
{
	if (NULL == _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CREATE_CALLBACK_OBJECT_FAILURE;
		return FALSE;
	}

	FILETIME fileTime, utcFileTime;
	memset(&fileTime, 0, sizeof(fileTime));
	memset(&utcFileTime, 0, sizeof(utcFileTime));

	// �Է¹��� �ð��� filetime���� ��ȯ
	if (FALSE == SystemTimeToFileTime(&callbackTime, &fileTime))
	{
		errorCode = (ERROR_CODE)::GetLastError();
		return FALSE;
	}

	// UTC �ð� �������� ������ ���Ѵٸ�
	if (FALSE != bConvertToUtcTime)
	{
		if (FALSE == LocalFileTimeToFileTime(&fileTime, &utcFileTime))
		{
			errorCode = (ERROR_CODE)::GetLastError();
			return FALSE;
		}
	}

	return _pThreadpoolCallbackObject->ExecuteThreadpoolCallbackTimer(pCallbackData, FALSE != bConvertToUtcTime ? utcFileTime : fileTime, (DWORD)ulPeriodMillisecond, fCancelPendingCallbacks, errorCode);
}

BOOL CThreadpoolCallbackTimerWrapper::ExecuteThreadpoolCallbackTimer(ICallbackData* pCallbackData, ULONG ulTimeoutMillisecond, ULONG ulPeriodMillisecond, BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode)
{
	if (NULL == _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CREATE_CALLBACK_OBJECT_FAILURE;
		return FALSE;
	}

	// ��� �ð� ����
	LARGE_INTEGER largeInteger;
	largeInteger.QuadPart = (-1LL * (ulTimeoutMillisecond * 10000LL));

	FILETIME fileTime;
	memset(&fileTime, 0, sizeof(fileTime));

	fileTime.dwHighDateTime = largeInteger.HighPart;
	fileTime.dwLowDateTime = largeInteger.LowPart;

	return _pThreadpoolCallbackObject->ExecuteThreadpoolCallbackTimer(pCallbackData, fileTime, (DWORD)ulPeriodMillisecond, fCancelPendingCallbacks, errorCode);
}

BOOL CThreadpoolCallbackTimerWrapper::CancelThreadpoolCallbackTimer(BOOL bWaitForCallbacks, BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode)
{
	if (NULL == _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CREATE_CALLBACK_OBJECT_FAILURE;
		return FALSE;
	}

	return _pThreadpoolCallbackObject->CancelThreadpoolCallbackTimer(bWaitForCallbacks, fCancelPendingCallbacks, errorCode);
}