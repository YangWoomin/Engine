#pragma once


#ifdef THREADPOOLENGINE_EXPORTS
#define THREADPOOLENGINE_API __declspec(dllexport)
#else
#define THREADPOOLENGINE_API __declspec(dllimport)
#endif

namespace ThreadpoolEngine
{
	// Timer 콜백 객체 래핑 클래스
	class THREADPOOLENGINE_API CThreadpoolCallbackTimerWrapper : public CThreadpoolCallbackObjectWrapper<CThreadpoolCallbackTimer>
	{
	public:
		// 해당 객체를 생성하고 나서 꼭 ERROR_CODE를 확인할 것
		CThreadpoolCallbackTimerWrapper(THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode);
		virtual ~CThreadpoolCallbackTimerWrapper();

		
	};
}