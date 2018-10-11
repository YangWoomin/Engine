#pragma once


#ifdef THREADPOOLENGINE_EXPORTS
#define THREADPOOLENGINE_API __declspec(dllexport)
#else
#define THREADPOOLENGINE_API __declspec(dllimport)
#endif

namespace ThreadpoolEngine
{
	// Io 콜백 객체 래핑 클래스
	class THREADPOOLENGINE_API CThreadpoolCallbackIoWrapper : public CThreadpoolCallbackObjectWrapper<CThreadpoolCallbackIo>
	{
	public:
		// 해당 객체를 생성하고 나서 꼭 ERROR_CODE를 확인할 것
		// hDevice는 IOCP와 바인딩할 장치
		CThreadpoolCallbackIoWrapper(HANDLE hDevice, THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode);
		virtual ~CThreadpoolCallbackIoWrapper();

		// 최초 1회만 콜백 데이터를 세팅할 수 있음
		// 콜백 데이터 변경 가능 여부 또는 하나의 장치에 다수의 콜백 데이터 세팅 가능 여부는 차후 필요에 따라 구현 결정
		BOOL SetCallbackData(ICallbackData* pCallbackData, ERROR_CODE& errorCode);

		// 바인딩된 해당 장치의 비동기 입출력 완료 시 콜백 데이터 기반의 콜백 함수가 호출되도록 개시
		// 반드시 비동기 입출력 함수(ex WSARecv)를 호출하기 전에 이 함수를 먼저 호출해야 함
		BOOL ExecuteThreadpoolCallbackIo(ERROR_CODE& errorCode);

		// 콜백 객체로부터 입출력 완료 콜백 함수 호출을 개시했던 것을 취소하는 함수 
		// 사용 예 : ExecuteThreadpoolCallbackIo 호출 후 비동기 입출력 함수(ex WSARecv) 호출을 실패했을 경우 이 함수를 호출해야 함
		BOOL CancelThreadpoolCallbackIo(ERROR_CODE& errorCode);
	};
}