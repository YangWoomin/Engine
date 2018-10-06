#pragma once
/*
struct CALLBACK_DATA_PARAMETER
{
	// 공통
	ERROR_CODE							_errorCode;

	// TP_WAIT 전용
	TP_WAIT_RESULT						_tpWaitResult;

	// TP_IO 전용
	LPOVERLAPPED						_pOverlapped;
	ULONG								_ulIoResult;
	ULONG_PTR							_ulpNumberOfBytesTransferred;

	CALLBACK_DATA_PARAMETER(ERROR_CODE errorCode, TP_WAIT_RESULT tpWaitResult, LPOVERLAPPED pOverlapped, ULONG ulIoResult, ULONG_PTR ulpNumberOfBytesTransferred)
	{
		_errorCode						= errorCode;

		_tpWaitResult					= tpWaitResult;

		_pOverlapped					= pOverlapped;
		_ulIoResult						= ulIoResult;
		_ulpNumberOfBytesTransferred	= ulpNumberOfBytesTransferred;
	}
};

// 콜백 객체의 콜백 함수로 전달될 콜백 데이터
interface ICallbackData
{
	virtual ~ICallbackData() { }

	// 호출될 콜백 함수
	virtual void CallbackFunction(CALLBACK_DATA_PARAMETER callbackDataParameter) = 0;

	// 콜백 함수 호출 후 이 객체의 자동 해제 여부, 체이닝에 유용하게 사용할 수 있음
	virtual BOOL DeleteCallbackDataAutomatically() = 0;
	
};
*/