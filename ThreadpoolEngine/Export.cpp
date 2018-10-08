#include "Common.h"
#include "Export.h"
#include "ThreadpoolGroupManager.h" //
#include "ThreadpoolCallbackObject.h"
//#include "CallbackData.h"
//#include "ThreadpoolGroupManager.h" //
#include "ThreadpoolCallbackWork.h"
#include "ThreadpoolCallbackIo.h"

using namespace ThreadpoolEngine;

CThreadpoolCallbackWorkWrapper::CThreadpoolCallbackWorkWrapper(THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode)
{
	_pThreadpoolCallbackWork = new CThreadpoolCallbackWork();
	if (NULL == _pThreadpoolCallbackWork)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_WORK_CREATE_CALLBACK_OBJECT_FAILURE;
	}
	else
	{
		// 콜백 객체를 스레드풀에 그루핑
		_pThreadpoolCallbackWork->Initialize(threadpoolGroupParameter, errorCode);
	}
}

CThreadpoolCallbackWorkWrapper::~CThreadpoolCallbackWorkWrapper()
{
	if (NULL != _pThreadpoolCallbackWork)
	{
		// 콜백 객체를 그루핑된 스레드풀에서 제거
		_pThreadpoolCallbackWork->Finalize();

		delete _pThreadpoolCallbackWork;
		_pThreadpoolCallbackWork = NULL;
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
	_pThreadpoolCallbackIo = new CThreadpoolCallbackIo(hDevice);
	if (NULL == _pThreadpoolCallbackIo)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_IO_CREATE_CALLBACK_OBJECT_FAILURE;
	}
	else
	{
		// 콜백 객체를 스레드풀에 그루핑
		_pThreadpoolCallbackIo->Initialize(threadpoolGroupParameter, errorCode);
	}
}

CThreadpoolCallbackIoWrapper::~CThreadpoolCallbackIoWrapper()
{
	if (NULL != _pThreadpoolCallbackIo)
	{
		// 콜백 객체를 그루핑된 스레드풀에서 제거
		_pThreadpoolCallbackIo->Finalize();

		delete _pThreadpoolCallbackIo;
		_pThreadpoolCallbackIo = NULL;
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