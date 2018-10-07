#include "Common.h"
#include "Export.h"
#include "ThreadpoolCallbackObject.h"
//#include "CallbackData.h"
#include "ThreadpoolGroupManager.h" //
#include "ThreadpoolCallbackWork.h"
#include "ThreadpoolCallbackIo.h"

CThreadpoolCallbackWorkWrapper::CThreadpoolCallbackWorkWrapper(THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode)
{
	_pThreadpoolCallbackWork = new CThreadpoolCallbackWork(threadpoolGroupParameter, errorCode);
}

CThreadpoolCallbackWorkWrapper::~CThreadpoolCallbackWorkWrapper()
{
	if (NULL != _pThreadpoolCallbackWork)
	{
		delete _pThreadpoolCallbackWork;
	}
}

BOOL CThreadpoolCallbackWorkWrapper::ExecuteThreadpoolCallbackWork(ICallbackData* pCallbackData, ERROR_CODE& errorCode)
{
	if (NULL == _pThreadpoolCallbackWork)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_WORK_CREATE_CALLBACK_OBJECT_FAILURE;
		return FALSE;
	}

	return _pThreadpoolCallbackWork->ExecuteThreadpoolCallbackWork(pCallbackData, errorCode);
}

CThreadpoolCallbackIoWrapper::CThreadpoolCallbackIoWrapper(HANDLE hDevice, THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode)
{
	_pThreadpoolCallbackIo = new CThreadpoolCallbackIo(hDevice, threadpoolGroupParameter, errorCode);
}

CThreadpoolCallbackIoWrapper::~CThreadpoolCallbackIoWrapper()
{
	if (NULL != _pThreadpoolCallbackIo)
	{
		delete _pThreadpoolCallbackIo;
	}
}

BOOL CThreadpoolCallbackIoWrapper::ExecuteThreadpoolCallbackIo(ERROR_CODE& errorCode)
{
	if (NULL == _pThreadpoolCallbackIo)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_IO_CREATE_CALLBACK_OBJECT_FAILURE;
		return FALSE;
	}

	return _pThreadpoolCallbackIo->ExecuteThreadpoolCallbackIo(errorCode);
}

BOOL CThreadpoolCallbackIoWrapper::SetCallbackData(ICallbackData* pCallbackData, ERROR_CODE& errorCode)
{
	if (NULL == _pThreadpoolCallbackIo)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_IO_CREATE_CALLBACK_OBJECT_FAILURE;
		return FALSE;
	}

	return _pThreadpoolCallbackIo->SetCallbackData(pCallbackData, errorCode);
}

BOOL CThreadpoolCallbackIoWrapper::CancelThreadpoolCallbackIo(BOOL bCancelPendingCallbacks, ERROR_CODE& errorCode)
{
	return _pThreadpoolCallbackIo->CancelThreadpoolCallbackIo(bCancelPendingCallbacks, errorCode);
}