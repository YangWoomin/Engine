#include "Common.h"
#include "ExportCommon.h"
#include "Utilities.h"
#include "ThreadpoolGroupManager.h"
#include "ThreadpoolCallbackObject.h"

using namespace ThreadpoolEngine;
using namespace ThreadpoolGroupManager;


// Singleton initializer start //
static CSingletonInitializer singletonInitializer;
CSingletonInitializer::CSingletonInitializer()
{
	// 스레드풀 그룹매니저 초기화
	ThreadpoolGroupManager();
}
// Singleton initializer end //


// CLock start //
CLock::CLock(CRITICAL_SECTION& criticalSection)
{
	_pCriticalSection = &criticalSection;
	EnterCriticalSection(_pCriticalSection);
}

CLock::~CLock()
{
	LeaveCriticalSection(_pCriticalSection);
}
// CLock end //

