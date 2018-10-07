#pragma once

class CCallbackDataForListenSocket;

class CCallbackDataForChildSocket : public ICallbackData 
{
private:
	CThreadpoolCallbackIoWrapper* _pThreadpoolCallbackIoWrapper;
	CCallbackDataForListenSocket* _pCallbackDataForListenSocket;
	BOOL _bDeleteThisObject;
	SOCKET _childSocket;
	SOCK_ITEM* _pSockItem;

public:
	CCallbackDataForChildSocket(CThreadpoolCallbackIoWrapper* pThreadpoolCallbackWrapper, CCallbackDataForListenSocket* pCallbackDataForListenSocket, SOCKET childSocket, SOCK_ITEM* pSockItem);

	CThreadpoolCallbackIoWrapper* GetThreadpoolCallbackIoWrapper();

	SOCK_ITEM* GetSockItem();

	virtual void CallbackFunction(CALLBACK_DATA_PARAMETER callbackDataParameter);

	virtual BOOL DeleteCallbackDataAutomatically();

	SOCKET GetChildSocket();
};