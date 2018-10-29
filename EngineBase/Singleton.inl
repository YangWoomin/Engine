
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
	// 생성 시도
	MyClass<Object>* pMyClass = new MyClass<Object>();

	// 실패 시
	if (NULL == pMyClass)
	{
		*(static_cast<ERROR_CODE*>(pParameter)) = static_cast<ERROR_CODE>(::GetLastError());
		return FALSE;
	}

	// 성공 시
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

	// 처음 호출하면 InitOnceSingleton이 콜백으로 호출되고 errorCode와 pContext에 결과를 반환
	// 처음 호출하는게 아니면 pContext에 기존의 인스턴스를 반환
	InitOnceExecuteOnce(&_initOnce, CSingletonTemplate::InitOnceSingleton, static_cast<PVOID>(&errorCode), &pContext);

	return static_cast<MyClass<Object>*>(pContext);
}
// CSingletonTemplate end //