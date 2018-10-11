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