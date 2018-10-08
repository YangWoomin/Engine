#pragma once

namespace ThreadpoolEngine
{
	class CLock
	{
	private:
		CRITICAL_SECTION* _pCriticalSection;
	public:
		CLock(CRITICAL_SECTION& criticalSection);
		~CLock();
	};
}