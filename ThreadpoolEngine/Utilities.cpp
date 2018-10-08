#include "Common.h"
#include "Export.h"
#include "Utilities.h"

using namespace ThreadpoolEngine;

CLock::CLock(CRITICAL_SECTION& criticalSection)
{
	_pCriticalSection = &criticalSection;
	EnterCriticalSection(_pCriticalSection);
}

CLock::~CLock()
{
	LeaveCriticalSection(_pCriticalSection);
}