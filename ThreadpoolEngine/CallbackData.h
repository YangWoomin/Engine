#pragma once
/*
struct CALLBACK_DATA_PARAMETER
{
	// ����
	ERROR_CODE							_errorCode;

	// TP_WAIT ����
	TP_WAIT_RESULT						_tpWaitResult;

	// TP_IO ����
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

// �ݹ� ��ü�� �ݹ� �Լ��� ���޵� �ݹ� ������
interface ICallbackData
{
	virtual ~ICallbackData() { }

	// ȣ��� �ݹ� �Լ�
	virtual void CallbackFunction(CALLBACK_DATA_PARAMETER callbackDataParameter) = 0;

	// �ݹ� �Լ� ȣ�� �� �� ��ü�� �ڵ� ���� ����, ü�̴׿� �����ϰ� ����� �� ����
	virtual BOOL DeleteCallbackDataAutomatically() = 0;
	
};
*/