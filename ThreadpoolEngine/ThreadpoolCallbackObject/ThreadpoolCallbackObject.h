#pragma once

namespace ThreadpoolEngine
{
	using namespace ThreadpoolGroupManager;

	// TP_WORK, TP_WAIT, TP_TIMER, TP_IO 4가지 콜백 객체의 추상 타입
	interface IThreadpoolCallbackObject
	{
		//////////////////////////// 전체 스레드 대상으로 동시에 실행될 수 없는 함수 ////////////////////////////
		// ThreadpoolGroupManager를 사용하기 위한 소유권 때문에 발생
		//
		// 해당 콜백 객체와 TP_CALLBACK_ENVIRON과 바인딩하도록 정의해야 함
		virtual BOOL BindThreadpoolCallbackObject(const PTP_CALLBACK_ENVIRON pTpCallbackEnviron, ERROR_CODE& errorCode) = 0;
		//
		// 해당 콜백 객체가 해제되는 것을 보장해야 함
		virtual BOOL ReleaseThreadpoolCallbackObject(BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode) = 0;
		//
		/////////////////////////////////////////////////////////////////////////////////////////////////////

		virtual ~IThreadpoolCallbackObject() { }
	};

	// TP_WORK, TP_WAIT, TP_TIMER, TP_IO 4종류로 사용
	template <typename PTP_OBJECT>
	class CThreadpoolCallbackObject : public IThreadpoolCallbackObject
	{
	protected:
		// 콜백 객체
		PTP_OBJECT _pThreadpoolCallbackObject;

		// 스레드풀 그룹에 정상적으로 추가되었는지 여부
		BOOL _bIsGrouping;

		// 콜백 함수가 호출되고 작업할 콜백 데이터
		ICallbackData* _pCallbackData;

		// 콜백 함수를 실행하는 스레드마다 콜백 객체의 인스턴스를 저장하기 위한 메모리 공간
		// 호출된 콜백 함수 내부에서 콜백 함수의 실행이 완료되기를 기다리면 데드락 상태에 빠지게 되므로 회피하기 위해서 사용함
		__declspec(thread) static PTP_OBJECT _pThreadpoolCallbackObjectInstance;

		// 콜백 함수를 실행하는 스레드마다 콜백 인스턴스 포인터를 저장하기 위한 메모리 공간
		// 호출된 콜백 함수 내부에서 콜백 함수의 실행이 완료되기를 기다리면 데드락 상태에 빠지게 되므로 회피하기 위해서 사용함
		__declspec(thread) static PTP_CALLBACK_INSTANCE _pTpCallbackInstance;

	public:
		CThreadpoolCallbackObject();
		virtual ~CThreadpoolCallbackObject();

		PTP_OBJECT GetThreadpoolCallbackObject();

		// 콜백 객체 생성자 호출 후에 이 함수를 호출해야 함
		void InitializeThreadpoolCallbackObject(THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode);

		// 콜백 객체의 소멸자 호출 전에 이 함수를 호출해야 함
		void FinalizeThreadpoolCallbackObject();

		// 콜백 데이터 기반의 콜백 함수 호출
		void CallbackThreadpoolCallbackObject(CALLBACK_DATA_PARAMETER callbackDataParameter, PTP_CALLBACK_INSTANCE pTpCallbackInstance);
	};
}

#include "ThreadpoolCallbackObject.inl"