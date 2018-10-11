#pragma once

namespace ThreadpoolEngine
{
	// Singleton initializer start //
	// 프로그램 main 시작 전에 생성되어야 할 싱글톤 클래스들의 인스턴스 생성
	// 생성자에서 싱글톤 클래스들의 인스턴스들을 생성
	class CSingletonInitializer
	{
	public:
		CSingletonInitializer();
	};
	// Singleton initializer end //


	// CLock start //
	// CRITICAL_SECTION의 Enter/Leave를 지역변수 생존기간에 의해 동작하도록 지원
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