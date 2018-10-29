#pragma once

#include "Common.h"

namespace EngineBase
{
	// �Ϲ� Ŭ�������� ����� �̱��� ���ø�
	// �� Ŭ���� ���ø��� ��ӹ޾Ƽ� ���
	// derived Ŭ���� �����ڸ� private�� �����ϰ� �� Ŭ������ friend�� �߰��ؾ� ��
	// MyClass : �̱��� ������ Ŭ����
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

		// ���α׷� ���� �� ȣ���ϴ� �Լ�
		static void Destroy();

	protected:
		CSingleton();

	public:
		static MyClass* GetInstance(ERROR_CODE& errorCode);
	};


	// Ŭ���� ���ø� ���� �̱��� Ŭ���� ���ø� (��ó�� std�� ������� �ʰ� ������ API�� ���)
	// �� Ŭ���� ���ø��� ��ӹ޾Ƽ� ���
	// derived Ŭ���� �����ڸ� private�� �����ϰ� �� Ŭ������ friend�� �߰��ؾ� ��
	// MyClass : �̱��� ������ Ŭ���� ���ø�
	// Object : �̱��� ������ Ŭ���� ���ø��� ù ��° Ŭ����(�Ǵ� Ÿ��)
	// Types : �̱��� ������ Ŭ���� ���ø��� ù ��° ������ �������ڵ�
	template <class Object, template <typename ... Types> class MyClass>
	class CSingletonTemplate
	{
	private:
		// ��Ÿ�� �ʱ�ȭ
		// ���� �����忡 ���� ���ÿ� GetInstance�� ȣ���� �� ������ �ʱ�ȭ�� �����ϱ� ���� ���
		static INIT_ONCE _initOnce;

		// ��Ÿ�� �ʱ�ȭ ���� �� ȣ��Ǵ� �ݹ� �Լ�
		// pParameter�� ERROR_CODE�� ���
		static BOOL CALLBACK InitOnceSingleton(PINIT_ONCE pInitOnce, PVOID pParameter, PVOID* ppContext);

		// ���α׷� ���� �� ȣ���ϴ� �Լ�
		static void Destroy();

	protected:
		CSingletonTemplate();

	public:
		// derived Ŭ������ ���ؼ� �ν��Ͻ��� ȹ���ϱ� ���ؼ� ȣ��Ǵ� �Լ�
		static MyClass<Object>* GetInstance(ERROR_CODE& errorCode);
	};
}

#include "Singleton.inl"