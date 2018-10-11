#pragma once


#ifdef THREADPOOLENGINE_EXPORTS
#define THREADPOOLENGINE_API __declspec(dllexport)
#else
#define THREADPOOLENGINE_API __declspec(dllimport)
#endif

namespace ThreadpoolEngine
{
	// Work 콜백 객체 래핑 클래스
	class THREADPOOLENGINE_API CThreadpoolCallbackWorkWrapper : public CThreadpoolCallbackObjectWrapper<CThreadpoolCallbackWork>
	{
	public:
		// 해당 객체를 생성하고 나서 꼭 ERROR_CODE를 확인할 것
		CThreadpoolCallbackWorkWrapper(THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode);
		virtual ~CThreadpoolCallbackWorkWrapper();

		// 콜백 데이터 기반의 비동기 콜백 함수 호출 개시 (작업이 밀려있지 않는 이상 거의 바로 시작됨)
		BOOL ExecuteThreadpoolCallbackWork(ICallbackData* pCallbackData, ERROR_CODE& errorCode);
	};
}