#include "Common.h"
#include "Export.h"
#include "Utilities.h"
#include "ThreadpoolGroupManager.h" //
#include "ThreadpoolCallbackObject.h"
//#include "CallbackData.h"
//#include "ThreadpoolGroupManager.h" //
#include "ThreadpoolCallbackWork.h"

using namespace ThreadpoolEngine;
using namespace ThreadpoolGroupManager;

CThreadpoolCallbackWork::CThreadpoolCallbackWork()
{
	::InitializeCriticalSection(&_criticalSection);
	
	//_pThreadpoolCallbackObject = NULL;

	//// 특정 그룹의 스레드풀 생성, 없으면 생성하고 있으면 그냥 쓰면 됨
	//ThreadpoolGroupManager()->CreateThreadpool(threadpoolGroupParameter._dwThreadpoolGroup, threadpoolGroupParameter._iMinThreadCount, threadpoolGroupParameter._iMaxThreadCount, errorCode);

	//// 스레드풀 특정 그룹에 콜백 객체를 추가
	//if (ERROR_CODE_THREADPOOL_THREADPOOL_GROUP_ALREADY_EXISTS == errorCode || ERROR_CODE_NONE == errorCode)
	//{
	//	// 스레드풀 그룹 매니저에 의해 BindThreadpoolCallbackObject 호출됨
	//	errorCode = ERROR_CODE_NONE;
	//	_bIsGrouping = ThreadpoolGroupManager()->InsertThreadpoolCallbackObject(threadpoolGroupParameter._dwThreadpoolGroup, this, errorCode);
	//}
}

CThreadpoolCallbackWork::~CThreadpoolCallbackWork()
{
	//// 특정 그룹의 스레드풀에 추가되었다면 제거
	//if (TRUE == _bIsGrouping)
	//{
	//	// 스레드풀 그룹 매니저에 의해 ReleaseThreadpoolCallbackObject 호출됨
	//	ERROR_CODE errorCode = ERROR_CODE_NONE;
	//	ThreadpoolGroupManager()->DeleteThreadpoolCallbackObject(this, FALSE, errorCode);
	//	_bIsGrouping = FALSE;
	//}

	//// 생성했던 스레드 풀은 여기서 제거하지 않도록 함
	//// 스레드풀 그룹 매니저가 프로그램 종료할 때 일괄적으로 자동 제거

	::DeleteCriticalSection(&_criticalSection);
}

BOOL CThreadpoolCallbackWork::BindThreadpoolCallbackObject(const PTP_CALLBACK_ENVIRON pTpCallbackEnviron, ERROR_CODE& errorCode)
{
	// 콜백 객체가 생성되어 있으면 안 됨, 중복 호출로 간주
	if (NULL != _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_WORK_ALREADY_BOUND;
		return FALSE;
	}

	// 콜백 데이터를 콜백 함수의 매개 변수로 받을 수 있도록 함
	// pTpCallbackEnviron는 NULL일 수도 있어서 검사하지 않음
	_pThreadpoolCallbackObject = ::CreateThreadpoolWork(ThreadpoolCallbackWorkCallbackFunction, (PVOID)this, pTpCallbackEnviron);

	// 실패하면 NULL을 리턴함
	if (NULL == _pThreadpoolCallbackObject)
	{
		errorCode = (ERROR_CODE)::GetLastError();
		return FALSE;
	}

	return TRUE;
}

BOOL CThreadpoolCallbackWork::ReleaseThreadpoolCallbackObject(BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode)
{
	if (NULL != _pThreadpoolCallbackObject)
	{
		::WaitForThreadpoolWorkCallbacks(_pThreadpoolCallbackObject, fCancelPendingCallbacks);
		::CloseThreadpoolWork(_pThreadpoolCallbackObject);
		_pThreadpoolCallbackObject = NULL;
	}

	return TRUE;
}

BOOL CThreadpoolCallbackWork::ExecuteThreadpoolCallbackWork(ICallbackData* pCallbackData, ERROR_CODE& errorCode)
{
	// 콜백 객체가 바인딩되지 않았음
	if (NULL == _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_WORK_NOT_BOUND;
		return FALSE;
	}

	{
		CLock lock(_criticalSection);
		// 콜백 데이터 큐에 콜백 데이터를 추가
		_callbackDataQueue.push(pCallbackData);
	}

	// 콜백 함수 호출 개시
	::SubmitThreadpoolWork(_pThreadpoolCallbackObject);

	// submit 결과 이상 여부 확인
	errorCode = (ERROR_CODE)::GetLastError();
	if (ERROR_CODE_NONE != errorCode)
	{
		return FALSE;
	}

	// 아래 코드는 혹시나 쓸 일이 있을까봐 우선 남겨 놓도록 함
	/*
	// 콜백 객체의 콜백 함수 호출 개시 횟수가 최대치에 도달하면 에러 처리
	if (_MAXIMUM_CALLBACK_RUNNING_COUNT <= GetCallbackRunningCount())
	{
	errorCode = ERROR_CODE_THREADPOOL_CALLBACK_WORK_CALLBACK_COUNT_MAX;
	return FALSE;
	}

	// 이전의 콜백 함수 호출이 완료된 후 콜백 함수 호출 개시를 하고 싶다면
	if (TRUE == bWaitForPreviousCallback)
	{
	// 이전 콜백 함수의 작업이 완료되도록 잠시 대기
	::WaitForThreadpoolWorkCallbacks(_pThreadpoolCallbackObject, fCancelPendingCallbacks);

	// 만약 이전 콜백 함수의 작업을 모두 취소한다면
	if (TRUE == fCancelPendingCallbacks)
	{
	// 콜백 함수 호출 개시 횟수도 0으로 초기화
	ResetCallbackRunningCount();
	}
	}

	// 콜백 함수 호출 개시 횟수 증가
	IncreaseCallbackRunningCount();
	*/

	return TRUE;
}

VOID CThreadpoolCallbackWork::ThreadpoolCallbackWorkCallbackFunction(PTP_CALLBACK_INSTANCE pInstance, PVOID pParam, PTP_WORK pTpWork)
{
	CThreadpoolCallbackWork* pThreadpoolCallbackWork = (CThreadpoolCallbackWork*)pParam;

	// pThreadpoolCallbackWork의 유효성 검사할 방법이 딱히 없음, 외부에서 관리를 잘 해주는 수 밖에
	// 가장 먼저 push한 콜백 데이터부터 처리
	ICallbackData* pCallbackData = pThreadpoolCallbackWork->GetCallbackDataFromQueue();
	if (NULL != pCallbackData)
	{
		if (NULL == pTpWork || NULL == pThreadpoolCallbackWork->GetThreadpoolCallbackObject())
		{
			pCallbackData->CallbackFunction(CALLBACK_DATA_PARAMETER(ERROR_CODE_THREADPOOL_CALLBACK_WORK_PTP_WORK_IS_NULL, 0, NULL, 0, 0));
		}
		else
		{
			// 콜백 데이터의 콜백 함수를 호출
			pCallbackData->CallbackFunction(CALLBACK_DATA_PARAMETER(ERROR_CODE_NONE, 0, NULL, 0, 0));
		}

		// 콜백 함수 호출 후 자동으로 콜백 데이터 해제를 허용하면 해제
		if (TRUE == pCallbackData->DeleteCallbackDataAutomatically())
		{
			delete pCallbackData;
		}
	}
}

// 아래 코드는 혹시나 쓸 일이 있을까봐 우선 남겨 놓도록 함
/*
BOOL CThreadpoolCallbackWork::SetCallbackData(ICallbackData* pCallbackData, BOOL bWaitForPreviousCallback, BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode)
{
	errorCode = ERROR_CODE_NONE;

	// 콜백 객체가 바인딩되지 않았음
	if (NULL != _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_WORK_NOT_BOUND;
		return FALSE;
	}

	// 현재 콜백 함수 호출 개시된 횟수가 0보다 크다면
	if (0 < GetCallbackRunningCount())
	{
		// 기다리지 않는다면 에러 처리
		if (FALSE == bWaitForPreviousCallback)
		{
			errorCode = ERROR_CODE_THREADPOOL_CALLBACK_WORK_CALLBACK_FUNCTION_RUNNING;
			return FALSE;
		}

		// 이전 콜백 함수 호출이 완료될 때까지 대기
		::WaitForThreadpoolWorkCallbacks(_pThreadpoolCallbackObject, fCancelPendingCallbacks);

		// 이전 콜백 함수의 작업을 모두 취소한다면
		if (TRUE == fCancelPendingCallbacks)
		{
			// 콜백 함수 호출 개시 횟수도 0으로 초기화
			ResetCallbackRunningCount();
		}
	}
	
	// 콜백 데이터 변경
	_pCallbackData = pCallbackData;

	return TRUE;
}
*/

ICallbackData* CThreadpoolCallbackWork::GetCallbackDataFromQueue()
{
	ICallbackData* pCallbackData = NULL;

	{
		CLock lock(_criticalSection);
		if (false == _callbackDataQueue.empty())
		{
			pCallbackData = _callbackDataQueue.front();
			_callbackDataQueue.pop();
		}
	}

	return pCallbackData;
}