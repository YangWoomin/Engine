#pragma once

class CLock
{
private:
	CRITICAL_SECTION* _pCriticalSection;
public:
	CLock(CRITICAL_SECTION& criticalSection);
	~CLock();
};