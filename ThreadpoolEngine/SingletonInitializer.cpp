#include "Common.h"
#include "Export.h"
//#include "Utilities.h"
#include "ThreadpoolCallbackObject.h"
//#include "CallbackData.h"
#include "ThreadpoolGroupManager.h" //
#include "SingletonInitializer.h"

static CSingletonInitializer singletonInitializer;

using namespace ThreadpoolGroupManager;

CSingletonInitializer::CSingletonInitializer()
{
	// 스레드풀 그룹매니저 초기화
	GetThreadpoolManager();
}