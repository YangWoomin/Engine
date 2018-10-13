#include "Common.h"
#include "ExportCommon.h"
#include "Utilities.h"
#include "ThreadpoolGroupManager.h"
#include "ThreadpoolCallbackObject.h"
#include "ThreadpoolCallbackWait.h"
#include "ThreadpoolCallbackWaitWrapper.h"

using namespace ThreadpoolEngine;
using namespace ThreadpoolGroupManager;

CThreadpoolCallbackWaitWrapper::CThreadpoolCallbackWaitWrapper(THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode)
	: CThreadpoolCallbackObjectWrapper(threadpoolGroupParameter, errorCode)
{

}

CThreadpoolCallbackWaitWrapper::~CThreadpoolCallbackWaitWrapper()
{

}

BOOL CThreadpoolCallbackWaitWrapper::ExecuteThreadpoolCallbackWait(HANDLE hObject, ICallbackData* pCallbackData, ULONG ulTimeoutMillisecond, BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode)
{
	if (NULL == _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CREATE_CALLBACK_OBJECT_FAILURE;
		return FALSE;
	}

	BOOL bResult = FALSE;

	FILETIME fileTime;
	memset(&fileTime, 0, sizeof(fileTime));

	// 0�̸� ���� ���
	if (0 == ulTimeoutMillisecond)
	{
		// FILETIME�� ����
		bResult = _pThreadpoolCallbackObject->ExecuteThreadpoolCallbackWait(hObject, pCallbackData, fileTime, TRUE, fCancelPendingCallbacks, errorCode);
	}
	// 0�� �ƴϸ� �и��� ���� ���
	else
	{
		// ��� �ð� ����
		LARGE_INTEGER largeInteger;
		largeInteger.QuadPart = (-1LL * (ulTimeoutMillisecond * 10000LL)); // 100 ns ������ �Է��ؾ� �ϹǷ� �Է¹��� ms ������ ns ������ ����
		fileTime.dwHighDateTime = largeInteger.HighPart;
		fileTime.dwLowDateTime = largeInteger.LowPart;

		bResult = _pThreadpoolCallbackObject->ExecuteThreadpoolCallbackWait(hObject, pCallbackData, fileTime, FALSE, fCancelPendingCallbacks, errorCode);
	}

	return bResult;
}