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
	// ������Ǯ �׷�Ŵ��� �ʱ�ȭ
	GetThreadpoolManager();
}