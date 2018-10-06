#include "Common.h"
#include "Export.h"
#include "ThreadpoolCallbackObject.h"
//#include "CallbackData.h"
#include "ThreadpoolGroupManager.h" //
#include "ThreadpoolCallbackWork.h"
#include "ThreadpoolCallbackIo.h"

//THREADPOOLENGINE_API CThreadpoolCallbackWork* CreateThreadpoolCallbackWork(THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter)
//{
//	return new CThreadpoolCallbackWork(threadpoolGroupParameter);
//}
//
//THREADPOOLENGINE_API BOOL ExecuteThreadpoolCallbackWork(CThreadpoolCallbackWork* pThreadpoolCallbackWork, ICallbackData* pCallbackData, ERROR_CODE& errorCode)
//{
//	return pThreadpoolCallbackWork->ExecuteThreadpoolCallbackWork(errorCode, pCallbackData);
//}

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

BOOL CThreadpoolCallbackWorkWrapper::ExecuteThreadpoolCallbackWorkWrapper(ICallbackData* pCallbackData, ERROR_CODE& errorCode)
{
	if (NULL == _pThreadpoolCallbackWork)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_WORK_CREATE_CALLBACK_OBJECT_FAILURE;
		return FALSE;
	}

	return _pThreadpoolCallbackWork->ExecuteThreadpoolCallbackWork(pCallbackData, errorCode);
}

//THREADPOOLENGINE_API CThreadpoolCallbackIo* CreateThreadpoolCallbackIo(HANDLE hDevice, THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter)
//{
//	return new CThreadpoolCallbackIo(hDevice, threadpoolGroupParameter);
//}