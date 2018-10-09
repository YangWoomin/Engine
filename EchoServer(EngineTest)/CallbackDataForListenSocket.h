#pragma once

using namespace ThreadpoolEngine;

class CCallbackDataForChildSocket;

class CCallbackDataForListenSocket : public ICallbackData
{
private:
	CThreadpoolCallbackIoWrapper* _pThreadpoolCallbackIoWrapper;
	BOOL _bDeleteThisObject;
	SOCKET _listenSocket;

	CRITICAL_SECTION _criticalSection;
	std::set<CCallbackDataForChildSocket*> _callbackDataForChildSocketSet;
	std::set<SOCK_ITEM*> _sockItemSet;

	BYTE _byChildSockSN;

public:
	CCallbackDataForListenSocket(CThreadpoolCallbackIoWrapper* pThreadpoolCallbackIoWrapper, SOCKET listenSocket);

	~CCallbackDataForListenSocket();

	virtual void CallbackFunction(CALLBACK_DATA_PARAMETER callbackDataParameter);

	virtual BOOL DeleteCallbackDataAutomatically();

	BOOL SetChildSocketPool(SOCK_ITEM* pSockItemOld, BOOL bSetReuse, ERROR_CODE& errorCode);

	CCallbackDataForChildSocket* FindCallbackDataForChildSocket(SOCKET childSocket);
};