#pragma once

#include "Common.h"

namespace EngineBase
{
	// CRITICAL_SECTION�� Enter/Leave�� �������� �����Ⱓ�� ���� �����ϵ��� ����
	class CLock
	{
	private:
		CRITICAL_SECTION* _pCriticalSection;
	public:
		CLock(CRITICAL_SECTION& criticalSection);
		~CLock();
	};
}