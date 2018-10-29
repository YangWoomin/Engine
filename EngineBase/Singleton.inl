
using namespace EngineBase;

// CSingleton start //
template <class MyClass> std::once_flag CSingleton<MyClass>::_onceFlag;
template <class MyClass> MyClass* CSingleton<MyClass>::_pInstance = nullptr;

template <class MyClass>
void CSingleton<MyClass>::Destroy()
{
	if (nullptr != _pInstance)
	{
		delete _pInstance;
		_pInstance = nullptr;
	}
}

template <class MyClass>
CSingleton<MyClass>::CSingleton()
{

}

template <class MyClass>
MyClass* CSingleton<MyClass>::GetInstance(ERROR_CODE& errorCode)
{
	std::call_once(_onceFlag, [&]
	{
		_pInstance = new MyClass();
		atexit(Destroy);
	});

	return _pInstance;
}
// CSingleton end //

// CSingletonTemplate start //
template <class Object, template <typename ... Types> class MyClass>
INIT_ONCE CSingletonTemplate<Object, MyClass>::_initOnce = INIT_ONCE_STATIC_INIT;

template <class Object, template <typename ... Types> class MyClass>
BOOL CSingletonTemplate<Object, MyClass>::InitOnceSingleton(PINIT_ONCE pInitOnce, PVOID pParameter, PVOID* ppContext)
{
	// ���� �õ�
	MyClass<Object>* pMyClass = new MyClass<Object>();

	// ���� ��
	if (NULL == pMyClass)
	{
		*(static_cast<ERROR_CODE*>(pParameter)) = static_cast<ERROR_CODE>(::GetLastError());
		return FALSE;
	}

	// ���� ��
	*ppContext = static_cast<PVOID>(pMyClass);
	atexit(Destroy);
	return TRUE;
}

template <class Object, template <typename ... Types> class MyClass>
void CSingletonTemplate<Object, MyClass>::Destroy()
{
	ERROR_CODE errorCode = ERROR_CODE_NONE;
	MyClass<Object>* pInstance = GetInstance(errorCode);
	if (NULL != pInstance)
	{
		delete pInstance;
		pInstance = NULL;
	}
}

template <class Object, template <typename ... Types> class MyClass>
CSingletonTemplate<Object, MyClass>::CSingletonTemplate()
{

}

template <class Object, template <typename ... Types> class MyClass>
MyClass<Object>* CSingletonTemplate<Object, MyClass>::GetInstance(ERROR_CODE& errorCode)
{
	PVOID pContext = NULL;

	// ó�� ȣ���ϸ� InitOnceSingleton�� �ݹ����� ȣ��ǰ� errorCode�� pContext�� ����� ��ȯ
	// ó�� ȣ���ϴ°� �ƴϸ� pContext�� ������ �ν��Ͻ��� ��ȯ
	InitOnceExecuteOnce(&_initOnce, CSingletonTemplate::InitOnceSingleton, static_cast<PVOID>(&errorCode), &pContext);

	return static_cast<MyClass<Object>*>(pContext);
}
// CSingletonTemplate end //