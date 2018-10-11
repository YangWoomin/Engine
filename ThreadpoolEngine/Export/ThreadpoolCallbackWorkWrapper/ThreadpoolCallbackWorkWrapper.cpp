#include "Common.h"
#include "ExportCommon.h"
#include "Utilities.h"
#include "ThreadpoolGroupManager.h"
#include "ThreadpoolCallbackObject.h"
#include "ThreadpoolCallbackWork.h"
#include "ThreadpoolCallbackWorkWrapper.h"

using namespace ThreadpoolEngine;
using namespace ThreadpoolGroupManager;

CThreadpoolCallbackWorkWrapper::CThreadpoolCallbackWorkWrapper(THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode)
	: CThreadpoolCallbackObjectWrapper(threadpoolGroupParameter, errorCode)
{

}

CThreadpoolCallbackWorkWrapper::~CThreadpoolCallbackWorkWrapper()
{

}

BOOL CThreadpoolCallbackWorkWrapper::ExecuteThreadpoolCallbackWork(ICallbackData* pCallbackData, ERROR_CODE& errorCode)
{
	if (NULL == _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CREATE_CALLBACK_OBJECT_FAILURE;
		return FALSE;
	}

	return _pThreadpoolCallbackObject->ExecuteThreadpoolCallbackWork(pCallbackData, errorCode);
}