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
	// ������Ǯ �׷�Ŵ��� �ʱ�ȭ
	ThreadpoolGroupManager();
}