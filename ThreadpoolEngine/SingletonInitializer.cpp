#include "Common.h"
#include "Export.h"
//#include "Utilities.h"
#include "ThreadpoolGroupManager.h" //
#include "ThreadpoolCallbackObject.h"
//#include "CallbackData.h"
//#include "ThreadpoolGroupManager.h" //
#include "SingletonInitializer.h"

using namespace ThreadpoolEngine;
using namespace ThreadpoolGroupManager;

static CSingletonInitializer singletonInitializer;

CSingletonInitializer::CSingletonInitializer()
{
	// 스레드풀 그룹매니저 초기화
	ThreadpoolGroupManager();
}