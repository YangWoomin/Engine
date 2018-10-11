#pragma once

namespace ThreadpoolEngine
{
	// Singleton initializer start //
	// ���α׷� main ���� ���� �����Ǿ�� �� �̱��� Ŭ�������� �ν��Ͻ� ����
	// �����ڿ��� �̱��� Ŭ�������� �ν��Ͻ����� ����
	class CSingletonInitializer
	{
	public:
		CSingletonInitializer();
	};
	// Singleton initializer end //


	// CLock start //
	// CRITICAL_SECTION�� Enter/Leave�� �������� �����Ⱓ�� ���� �����ϵ��� ����
	class CLock
	{
	private:
		CRITICAL_SECTION* _pCriticalSection;
	public:
		CLock(CRITICAL_SECTION& criticalSection);
		~CLock();
	};
	// CLock end //


}