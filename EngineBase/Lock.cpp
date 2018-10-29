#include "Lock.h"

using namespace EngineBase;

CLock::CLock(CRITICAL_SECTION& criticalSection)
{
	_pCriticalSection = &criticalSection;
	EnterCriticalSection(_pCriticalSection);
}

CLock::~CLock()
{
	LeaveCriticalSection(_pCriticalSection);
}