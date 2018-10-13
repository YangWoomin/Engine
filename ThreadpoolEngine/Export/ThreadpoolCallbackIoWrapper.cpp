#include "Common.h"
#include "ExportCommon.h"
#include "Utilities.h"
#include "ThreadpoolGroupManager.h"
#include "ThreadpoolCallbackObject.h"
#include "ThreadpoolCallbackIo.h"
#include "ThreadpoolCallbackIoWrapper.h"

using namespace ThreadpoolEngine;
using namespace ThreadpoolGroupManager;

CThreadpoolCallbackIoWrapper::CThreadpoolCallbackIoWrapper(HANDLE hDevice, THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode)
	: CThreadpoolCallbackObjectWrapper(hDevice, threadpoolGroupParameter, errorCode)
{

}

CThreadpoolCallbackIoWrapper::~CThreadpoolCallbackIoWrapper()
{

}

BOOL CThreadpoolCallbackIoWrapper::SetCallbackData(ICallbackData* pCallbackData, ERROR_CODE& errorCode)
{
	if (NULL == _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CREATE_CALLBACK_OBJECT_FAILURE;
		return FALSE;
	}

	return _pThreadpoolCallbackObject->SetCallbackData(pCallbackData, errorCode);
}

BOOL CThreadpoolCallbackIoWrapper::ExecuteThreadpoolCallbackIo(ERROR_CODE& errorCode)
{
	if (NULL == _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CREATE_CALLBACK_OBJECT_FAILURE;
		return FALSE;
	}

	return _pThreadpoolCallbackObject->ExecuteThreadpoolCallbackIo(errorCode);
}

BOOL CThreadpoolCallbackIoWrapper::CancelThreadpoolCallbackIo(ERROR_CODE& errorCode)
{
	if (NULL == _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CREATE_CALLBACK_OBJECT_FAILURE;
		return FALSE;
	}

	return _pThreadpoolCallbackObject->CancelThreadpoolCallbackIo(errorCode);
}