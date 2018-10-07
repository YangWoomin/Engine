#include "Common.h"
#include "Export.h"
#include "Utilities.h"
#include "ThreadpoolCallbackObject.h"
//#include "CallbackData.h"
#include "ThreadpoolGroupManager.h" //
//#include "ThreadpoolCallbackWork.h" //
#include "ThreadpoolCallbackIo.h"

using namespace ThreadpoolGroupManager;

CThreadpoolCallbackIo::CThreadpoolCallbackIo(HANDLE hDevice, THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode)
{
	_pThreadpoolCallbackObject = NULL;
	_hDevice = hDevice;
	_pCallbackData = NULL;

	// 특정 그룹의 스레드풀 생성, 없으면 생성하고 있으면 그냥 쓰면 됨
	// _errorCode = ERROR_CODE_NONE;
	GetThreadpoolManager()->CreateThreadpool(threadpoolGroupParameter._dwThreadpoolGroup, threadpoolGroupParameter._iMinThreadCount, threadpoolGroupParameter._iMaxThreadCount, errorCode);

	// 스레드풀 특정 그룹에 콜백 객체를 추가
	if (ERROR_CODE_THREADPOOL_THREADPOOL_GROUP_ALREADY_EXISTS == errorCode || ERROR_CODE_NONE == errorCode)
	{
		errorCode = ERROR_CODE_NONE;
		_bIsGrouping = GetThreadpoolManager()->InsertThreadpoolCallbackObject(threadpoolGroupParameter._dwThreadpoolGroup, this, errorCode);
	}
}

CThreadpoolCallbackIo::~CThreadpoolCallbackIo()
{
	// 특정 그룹의 스레드풀에 추가되었다면 제거
	if (TRUE == _bIsGrouping)
	{
		ERROR_CODE errorCode = ERROR_CODE_NONE;
		GetThreadpoolManager()->DeleteThreadpoolCallbackObject(this, FALSE, errorCode);
		_bIsGrouping = FALSE;
	}
}

BOOL CThreadpoolCallbackIo::BindThreadpoolCallbackObject(const PTP_CALLBACK_ENVIRON pTpCallbackEnviron, ERROR_CODE& errorCode)
{
	// 콜백 객체가 생성되어 있으면 안 됨, 중복 호출로 간주
	if (NULL != _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_IO_ALREADY_BOUND;
		return FALSE;
	}

	// 장치가 세팅되어 있어야 함
	if (NULL == _hDevice || INVALID_HANDLE_VALUE == _hDevice)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_IO_INVALID_DEVICE;
		return FALSE;
	}

	// 콜백 데이터를 콜백 함수의 매개 변수로 받을 수 있도록 함
	// pTpCallbackEnviron는 NULL일 수도 있어서 검사하지 않음
	_pThreadpoolCallbackObject = ::CreateThreadpoolIo(_hDevice, ThreadpoolCallbackIoCallbackFunction, (PVOID)this, pTpCallbackEnviron);
	
	// 실패하면 NULL을 리턴함, 보통 장치가 문제 있을 때 
	// OVERLAPPED로 중첩 입출력 장치로 생성하지 않았을 경우 ::GetLastError()에서 87 (ERROR_INVALID_PARAMETER) 리턴
	if (NULL == _pThreadpoolCallbackObject)
	{
		errorCode = (ERROR_CODE)::GetLastError();
		return FALSE;
	}

	return TRUE;
}

BOOL CThreadpoolCallbackIo::ReleaseThreadpoolCallbackObject(BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode)
{
	BOOL bResult = FALSE;

	if (NULL != _pThreadpoolCallbackObject)
	{
		bResult = CancelThreadpoolCallbackIo(fCancelPendingCallbacks, errorCode);

		// 바인딩된 장치의 모든 중첩 입출력 취소의 결과와 무관하게 콜백 객체는 해제되어야 함
		// ::WaitForThreadpoolIoCallbacks(_pThreadpoolCallbackObject, fCancelPendingCallbacks);
		::CloseThreadpoolIo(_pThreadpoolCallbackObject);
		_pThreadpoolCallbackObject = NULL;
	}

	return bResult;
}

// 콜백 데이터 세팅
BOOL CThreadpoolCallbackIo::SetCallbackData(ICallbackData* pCallbackData, ERROR_CODE& errorCode)
{
	errorCode = ERROR_CODE_NONE;

	// 콜백 객체가 바인딩되지 않았음
	if (NULL == _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_IO_NOT_BOUND;
		return FALSE;
	}

	// 스레드풀 IO 객체의 콜백 데이터를 중간에 변경하지 못하도록 막음
	if (NULL != _pCallbackData)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_IO_CALLBACK_DATA_ALREADY_SET;
		return FALSE;
	}

	/*
	// 현재 콜백 함수 호출 개시된 횟수가 0보다 크다면
	if (0 < GetCallbackRunningCount())
	{
		// 기다리지 않는다면 에러 처리
		if (FALSE == bWaitForPreviousCallback)
		{
			errorCode = ERROR_CODE_THREADPOOL_CALLBACK_IO_CALLBACK_FUNCTION_RUNNING;
			return FALSE;
		}

		// 이전 콜백 함수의 작업을 모두 취소한다면
		if (TRUE == fCancelPendingCallbacks)
		{
			
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
	*/

	// 콜백 데이터 변경
	_pCallbackData = pCallbackData;

	return TRUE;
}

BOOL CThreadpoolCallbackIo::ExecuteThreadpoolCallbackIo(ERROR_CODE& errorCode)
{
	// 콜백 객체가 바인딩되지 않았음
	if (NULL == _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_IO_NOT_BOUND;
		return FALSE;
	}

	//// 장치가 세팅되어 있어야 함
	//if (NULL == _hDevice || INVALID_HANDLE_VALUE == _hDevice)
	//{
	//	errorCode = ERROR_CODE_THREADPOOL_CALLBACK_IO_INVALID_DEVICE;
	//	return FALSE;
	//}

	// 콜백 데이터가 세팅되어 있어야 함
	if (NULL == _pCallbackData)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_IO_CALLBACK_DATA_NOT_SET;
		return FALSE;
	}

	// 중첩 입출력 개시
	::StartThreadpoolIo(_pThreadpoolCallbackObject);

	// 중첩 입출력 개시 결과 이상 여부 확인
	errorCode = (ERROR_CODE)::GetLastError();
	if (ERROR_CODE_NONE != errorCode)
	{
		return FALSE;
	}

	return TRUE;
}

VOID CThreadpoolCallbackIo::ThreadpoolCallbackIoCallbackFunction(PTP_CALLBACK_INSTANCE pInstance, PVOID pParam, PVOID pOverlapped, ULONG ulIoResult, ULONG_PTR ulpNumberOfBytesTransferred, PTP_IO pTpIo)
{
	CThreadpoolCallbackIo* pThreadpoolCallbackIo = (CThreadpoolCallbackIo*)pParam;

	// pThreadpoolCallbackIo의 유효성 검사할 방법이 딱히 없음, 외부에서 관리를 잘 해주는 수 밖에
	ICallbackData* pCallbackData = pThreadpoolCallbackIo->GetCallbackData();
	// 콜백 데이터가 NULL이면 아무것도 못함
	if (NULL == pCallbackData)
	{
		//// 콜백 함수의 호출 개시 횟수를 감소
		//pThreadpoolCallbackIo->DecreaseCallbackRunningCount();
		return;
	}

	if (NULL == pTpIo || NULL == pThreadpoolCallbackIo->GetThreadpoolCallbackObject())
	{
		pCallbackData->CallbackFunction(CALLBACK_DATA_PARAMETER(ERROR_CODE_THREADPOOL_CALLBACK_IO_PTP_IO_IS_NULL, 0, (LPOVERLAPPED)pOverlapped, ulIoResult, ulpNumberOfBytesTransferred));
	}
	else
	{
		// 콜백 데이터의 콜백 함수를 호출
		pCallbackData->CallbackFunction(CALLBACK_DATA_PARAMETER(ERROR_CODE_NONE, 0, (LPOVERLAPPED)pOverlapped, ulIoResult, ulpNumberOfBytesTransferred));
	}

	// 콜백 함수 호출 후 자동으로 콜백 데이터 해제를 허용하면 해제
	if (TRUE == pCallbackData->DeleteCallbackDataAutomatically())
	{
		delete pCallbackData;
	}

	//// 콜백 함수의 호출 개시 횟수를 감소
	//pThreadpoolCallbackIo->DecreaseCallbackRunningCount();
}

ICallbackData* CThreadpoolCallbackIo::GetCallbackData()
{
	return _pCallbackData;
}

BOOL CThreadpoolCallbackIo::CancelThreadpoolCallbackIo(BOOL bCancelPendingCallbacks, ERROR_CODE& errorCode)
{
	// 콜백 객체가 바인딩되지 않았음
	if (NULL == _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_IO_NOT_BOUND;
		return FALSE;
	}

	//// 장치가 세팅되어 있어야 함
	//if (NULL == _hDevice || INVALID_HANDLE_VALUE == _hDevice)
	//{
	//	errorCode = ERROR_CODE_THREADPOOL_CALLBACK_IO_INVALID_DEVICE;
	//	return FALSE;
	//}

	// 바인딩된 장치의 모든 중첩 입출력을 취소
	BOOL bResult = ::CancelIoEx(_hDevice, NULL);
	if (FALSE == bResult)
	{
		errorCode = (ERROR_CODE)::GetLastError();
	}

	::WaitForThreadpoolIoCallbacks(_pThreadpoolCallbackObject, bCancelPendingCallbacks);
	errorCode = (ERROR_CODE)::GetLastError();
	if (ERROR_CODE_NONE != errorCode)
	{
		bResult = FALSE;
	}

	return bResult;
}