#pragma once

#include "Common.h"

namespace EngineBase
{
	// 일반 클래스에서 사용할 싱글턴 템플릿
	// 이 클래스 템플릿을 상속받아서 사용
	// derived 클래스 생성자를 private에 선언하고 이 클래스를 friend로 추가해야 함
	// MyClass : 싱글톤 적용할 클래스
	// ex) 
	// class CTestSingleton : public CSingleton<CTestSingleton>
	// {
	//	private:
	//		friend CSingleton;
	//		CTestSingleton();
	//		...
	//	}
	template <class MyClass>
	class CSingleton
	{
	private:
		static std::once_flag _onceFlag;

		static MyClass* _pInstance;

		// 프로그램 종료 시 호출하는 함수
		static void Destroy();

	protected:
		CSingleton();

	public:
		static MyClass* GetInstance(ERROR_CODE& errorCode);
	};


	// 클래스 템플릿 전용 싱글톤 클래스 템플릿 (위처럼 std를 사용하지 않고 윈도우 API를 사용)
	// 이 클래스 템플릿을 상속받아서 사용
	// derived 클래스 생성자를 private에 선언하고 이 클래스를 friend로 추가해야 함
	// MyClass : 싱글톤 적용할 클래스 템플릿
	// Object : 싱글톤 적용할 클래스 탬플릿의 첫 번째 클래스(또는 타입)
	// Types : 싱글톤 적용할 클래스 템플릿의 첫 번째 이후의 가변인자들
	template <class Object, template <typename ... Types> class MyClass>
	class CSingletonTemplate
	{
	private:
		// 원타임 초기화
		// 여러 스레드에 의해 동시에 GetInstance를 호출할 시 동기적 초기화를 보장하기 위해 사용
		static INIT_ONCE _initOnce;

		// 원타임 초기화 진행 시 호출되는 콜백 함수
		// pParameter는 ERROR_CODE로 사용
		static BOOL CALLBACK InitOnceSingleton(PINIT_ONCE pInitOnce, PVOID pParameter, PVOID* ppContext);

		// 프로그램 종료 시 호출하는 함수
		static void Destroy();

	protected:
		CSingletonTemplate();

	public:
		// derived 클래스를 통해서 인스턴스를 획득하기 위해서 호출되는 함수
		static MyClass<Object>* GetInstance(ERROR_CODE& errorCode);
	};
}

#include "Singleton.inl"