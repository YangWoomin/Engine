#include "Common.h"
#include "Export.h"
#include "ThreadpoolGroupManager.h" //
#include "ThreadpoolCallbackObject.h"
//#include "CallbackData.h"
//#include "ThreadpoolGroupManager.h" //
#include "ThreadpoolCallbackWork.h"
#include "ThreadpoolCallbackWait.h"
#include "ThreadpoolCallbackIo.h"

using namespace ThreadpoolEngine;

// CThreadpoolCallbackWorkWrapper start //
CThreadpoolCallbackWorkWrapper::CThreadpoolCallbackWorkWrapper(THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode)
{
	_pThreadpoolCallbackWork = new CThreadpoolCallbackWork();
	if (NULL == _pThreadpoolCallbackWork)
	{
		errorCode = ERROR_CODE_THREADPOOL_CREATE_CALLBACK_OBJECT_FAILURE;
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
		errorCode = ERROR_CODE_THREADPOOL_CREATE_CALLBACK_OBJECT_FAILURE;
		return FALSE;
	}

	return _pThreadpoolCallbackWork->ExecuteThreadpoolCallbackWork(pCallbackData, errorCode);
}
// CThreadpoolCallbackWorkWrapper end //

// CThreadpoolCallbackWaitWrapper start //
CThreadpoolCallbackWaitWrapper::CThreadpoolCallbackWaitWrapper(THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode)
{
	_pThreadpoolCallbackWait = new CThreadpoolCallbackWait();
	if (NULL == _pThreadpoolCallbackWait)
	{
		errorCode = ERROR_CODE_THREADPOOL_CREATE_CALLBACK_OBJECT_FAILURE;
	}
	else
	{
		// 콜백 객체를 스레드풀에 그루핑
		_pThreadpoolCallbackWait->Initialize(threadpoolGroupParameter, errorCode);
	}
}

CThreadpoolCallbackWaitWrapper::~CThreadpoolCallbackWaitWrapper()
{
	if (NULL != _pThreadpoolCallbackWait)
	{
		// 콜백 객체를 그루핑된 스레드풀에서 제거
		_pThreadpoolCallbackWait->Finalize();

		delete _pThreadpoolCallbackWait;
		_pThreadpoolCallbackWait = NULL;
	}
}

BOOL CThreadpoolCallbackWaitWrapper::ExecuteThreadpoolCallbackWait(HANDLE hObject, ICallbackData* pCallbackData, ULONG ulTimeout, BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode)
{
	if (NULL == _pThreadpoolCallbackWait)
	{
		errorCode = ERROR_CODE_THREADPOOL_CREATE_CALLBACK_OBJECT_FAILURE;
		return FALSE;
	}

	BOOL bResult = FALSE;

	FILETIME fileTime;
	memset(&fileTime, 0, sizeof(fileTime));

	// 0이면 무한 대기
	if (0 == ulTimeout)
	{
		// FILETIME은 무관
		bResult = _pThreadpoolCallbackWait->ExecuteThreadpoolCallbackWait(hObject, pCallbackData, fileTime, TRUE, fCancelPendingCallbacks, errorCode);
	}
	// 0이 아니면 밀리초 단위 대기
	else
	{
		// 대기 시간 세팅
		LARGE_INTEGER largeInteger;
		largeInteger.QuadPart = (-1LL * (ulTimeout * 10000LL)); // 100 ns 단위로 입력해야 하므로 입력받은 ms 단위를 ns 단위로 변경
		fileTime.dwHighDateTime = largeInteger.HighPart;
		fileTime.dwLowDateTime = largeInteger.LowPart;

		bResult = _pThreadpoolCallbackWait->ExecuteThreadpoolCallbackWait(hObject, pCallbackData, fileTime, FALSE, fCancelPendingCallbacks, errorCode);
	}

	return bResult;
}
// CThreadpoolCallbackWaitWrapper end //

// CThreadpoolCallbackIoWrapper start //
CThreadpoolCallbackIoWrapper::CThreadpoolCallbackIoWrapper(HANDLE hDevice, THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode)
{
	_pThreadpoolCallbackIo = new CThreadpoolCallbackIo(hDevice);
	if (NULL == _pThreadpoolCallbackIo)
	{
		errorCode = ERROR_CODE_THREADPOOL_CREATE_CALLBACK_OBJECT_FAILURE;
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
		errorCode = ERROR_CODE_THREADPOOL_CREATE_CALLBACK_OBJECT_FAILURE;
		return FALSE;
	}

	return _pThreadpoolCallbackIo->ExecuteThreadpoolCallbackIo(errorCode);
}

BOOL CThreadpoolCallbackIoWrapper::SetCallbackData(ICallbackData* pCallbackData, ERROR_CODE& errorCode)
{
	if (NULL == _pThreadpoolCallbackIo)
	{
		errorCode = ERROR_CODE_THREADPOOL_CREATE_CALLBACK_OBJECT_FAILURE;
		return FALSE;
	}

	return _pThreadpoolCallbackIo->SetCallbackData(pCallbackData, errorCode);
}

//BOOL CThreadpoolCallbackIoWrapper::CleanupThreadpoolCallbackIo(BOOL bCancelPendingCallbacks, ERROR_CODE& errorCode)
//{
//	if (NULL == _pThreadpoolCallbackIo)
//	{
//		errorCode = ERROR_CODE_THREADPOOL_CREATE_CALLBACK_OBJECT_FAILURE;
//		return FALSE;
//	}
//
//	return _pThreadpoolCallbackIo->CleanupThreadpoolCallbackIo(bCancelPendingCallbacks, errorCode);
//}

BOOL CThreadpoolCallbackIoWrapper::CancelThreadpoolCallbackIo(ERROR_CODE& errorCode)
{
	if (NULL == _pThreadpoolCallbackIo)
	{
		errorCode = ERROR_CODE_THREADPOOL_CREATE_CALLBACK_OBJECT_FAILURE;
		return FALSE;
	}

	return _pThreadpoolCallbackIo->CancelThreadpoolCallbackIo(errorCode);
}
// CThreadpoolCallbackIoWrapper end //