#pragma once

#include "Common.h"

namespace EngineBase
{
	// CRITICAL_SECTION의 Enter/Leave를 지역변수 생존기간에 의해 동작하도록 지원
	class CLock
	{
	private:
		CRITICAL_SECTION* _pCriticalSection;
	public:
		CLock(CRITICAL_SECTION& criticalSection);
		~CLock();
	};
}