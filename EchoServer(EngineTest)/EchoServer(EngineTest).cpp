// EchoServer(EngineTest).cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Export.h"

#include "CallbackDataForChildSocket.h"
#include "CallbackDataForListenSocket.h"

using namespace std;

#define COUT std::cout

PVOID GetSockExtAPI(SOCKET sock, GUID guidFn)
{
	PVOID pfnEx = NULL;
	GUID guid = guidFn;
	DWORD dwBytes = 0;
	LONG lRet = WSAIoctl
	(
		sock, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guid, sizeof(guid), &pfnEx,
		sizeof(pfnEx), &dwBytes, NULL, NULL
	);
	if (lRet == SOCKET_ERROR)
	{
		cout << "WSAIoctl failed, code : " << WSAGetLastError() << endl;
		return NULL;
	}
	return pfnEx;
}

class CCallbackDataWork : public ICallbackData
{
private:
	SOCKET _sock;
	char _szBuff[512];
	LONG _lLen;
public:
	CCallbackDataWork(SOCKET sock, char* szBuff, LONG lLen)
	{
		_sock = sock;
		memcpy(_szBuff, szBuff, 512);
		_szBuff[lLen] = 0;
		_lLen = lLen;
	}
	virtual void CallbackFunction(CALLBACK_DATA_PARAMETER callbackDataParameter)
	{
		if (SOCKET_ERROR == send(_sock, _szBuff, _lLen, 0))
		{
			cout << "send failed : " << WSAGetLastError() << endl;
		}
	}

	virtual BOOL DeleteCallbackDataAutomatically()
	{
		return TRUE;
	}

};

int main()
{
	ERROR_CODE errorCode = ERROR_CODE_NONE;
	/*
	CThreadpoolCallbackWorkWrapper* pThreadpoolCallbackWorkWrapper = new CThreadpoolCallbackWorkWrapper(THREADPOOL_GROUP_PARAMETER((ThreadpoolGroup)1, 1, 10), errorCode);
	if (ERROR_CODE_NONE != errorCode)
	{
	cout << "Creating pThreadpoolCallbackWorkWrapper failed! ERROR_CODE : " << (int)errorCode << endl;
	delete pThreadpoolCallbackWorkWrapper;
	return;
	}
	*/

	WSADATA	wsd;
	int nErrCode = WSAStartup(MAKEWORD(2, 2), &wsd);
	if (nErrCode)
	{
		cout << "WSAStartup failed with error : " << nErrCode << endl;
		return 0;
	}

	SOCKET hsoListen = INVALID_SOCKET;
	CThreadpoolCallbackIoWrapper* pThreadpoolCallbackIoWrapperForListenSocket = NULL;
	CCallbackDataForListenSocket* pCallbackDataForListenSocket = NULL;

	try
	{
		hsoListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (hsoListen == INVALID_SOCKET)
		{
			errorCode = (ERROR_CODE)::WSAGetLastError();
			cout << "socket failed, code : " << errorCode << endl;
			throw (ERROR_CODE)errorCode;
		}

		SOCKADDR_IN	sa;
		memset(&sa, 0, sizeof(SOCKADDR_IN));
		sa.sin_family = AF_INET;
		sa.sin_port = htons(9001);
		sa.sin_addr.s_addr = htonl(INADDR_ANY);
		LONG lSockRet = bind(hsoListen, (PSOCKADDR)&sa, sizeof(SOCKADDR_IN));
		if (lSockRet == SOCKET_ERROR)
		{
			errorCode = (ERROR_CODE)::WSAGetLastError();
			cout << "bind failed, code : " << errorCode << endl;
			throw (ERROR_CODE)errorCode;
		}

		lSockRet = listen(hsoListen, SOMAXCONN);
		if (lSockRet == SOCKET_ERROR)
		{
			errorCode = (ERROR_CODE)::WSAGetLastError();
			cout << "listen failed, code : " << errorCode << endl;
			throw (ERROR_CODE)errorCode;
		}

		// 리슨 소켓에 대한 IO 래퍼 클래스 생성
		pThreadpoolCallbackIoWrapperForListenSocket = new CThreadpoolCallbackIoWrapper((HANDLE)hsoListen, THREADPOOL_GROUP_PARAMETER((ThreadpoolGroup)1, 1, 10), errorCode);
		if (ERROR_CODE_NONE != errorCode)
		{
			cout << "creating CThreadpoolCallbackIoWrapper failed " << errorCode << endl;
			throw (ERROR_CODE)errorCode;
		}

		// 리슨 소켓에서 참조할 콜백 데이터 세팅
		pCallbackDataForListenSocket = new CCallbackDataForListenSocket(pThreadpoolCallbackIoWrapperForListenSocket, hsoListen);
		if (ERROR_CODE_NONE != errorCode)
		{
			cout << "creating CCallbackDataForListenSocket failed " << errorCode << endl;
			throw (ERROR_CODE)errorCode;
		}

		// IO 래퍼 클래스에 콜백 데이터 세팅
		if (FALSE == pThreadpoolCallbackIoWrapperForListenSocket->SetCallbackData(pCallbackDataForListenSocket, errorCode))
		{
			cout << "SetCallbackData failed " << errorCode << endl;
			throw (ERROR_CODE)errorCode;
		}

		// 소켓풀을 생성하고 비동기 입출력을 개시
		if (FALSE == pCallbackDataForListenSocket->SetChildSocketPool(NULL, FALSE, errorCode))
		{
			cout << "SetChildSocketPool failed " << errorCode << endl;
			throw (ERROR_CODE)errorCode;
		}

		cout << "========================== server started ==========================" << endl;

		/*
		char szBuff[512];
		while (true)
		{
		SOCKET sock = accept(hsoListen, NULL, NULL);
		if (sock == INVALID_SOCKET)
		{
		cout << "accept failed, code : " << WSAGetLastError() << endl;
		return;
		}

		ERROR_CODE errorCode = ERROR_CODE_NONE;
		CThreadpoolCallbackIoWrapper* pThreadpoolCallbackIoWrapper = new CThreadpoolCallbackIoWrapper((HANDLE)sock, THREADPOOL_GROUP_PARAMETER((ThreadpoolGroup)1, 1, 10), errorCode);

		cout << " ==> New client " << sock << " connected..." << endl;
		lSockRet = recv(sock, szBuff, sizeof(szBuff), 0);
		if (lSockRet == SOCKET_ERROR)
		{
		cout << "recv failed : " << WSAGetLastError() << endl;
		break;
		}
		if (lSockRet == 0)
		{
		cout << " ==> Client " << sock << " disconnected..." << endl;
		break;
		}

		szBuff[lSockRet] = 0;
		cout << " *** Client(" << sock << ") sent : " << szBuff << endl;

		Sleep(10);

		lSockRet = send(sock, szBuff, lSockRet, 0);
		if (lSockRet == SOCKET_ERROR)
		{
		cout << "send failed : " << WSAGetLastError() << endl;
		break;
		}


		for (int i = 0; i < 3; ++i)
		{
		ERROR_CODE errorCode = ERROR_CODE_NONE;
		if (FALSE == pThreadpoolCallbackWorkWrapper->ExecuteThreadpoolCallbackWork(new CCallbackDataWork(sock, szBuff, lSockRet), errorCode))
		{
		cout << "ExecuteThreadpoolCallbackWork failed.. error code : " << (int)errorCode << endl;
		break;
		}
		}

		}
		*/

		getchar();

		// closesocket(sock);
		// closesocket(hsoListen);

		// delete pThreadpoolCallbackWorkWrapper;

		delete pThreadpoolCallbackIoWrapperForListenSocket;
		delete pCallbackDataForListenSocket;
		::closesocket(hsoListen);

		cout << "========================== server terminated ==========================" << endl;

		getchar();
	}
	catch (ERROR_CODE errorCode)
	{
		if (NULL != pThreadpoolCallbackIoWrapperForListenSocket)
		{
			delete pThreadpoolCallbackIoWrapperForListenSocket;
		}

		if (NULL != pCallbackDataForListenSocket)
		{
			delete pCallbackDataForListenSocket;
		}

		if (INVALID_SOCKET != hsoListen)
		{
			::closesocket(hsoListen);
		}

		cout << "exception occrued " << errorCode << endl;
	}

	WSACleanup();

	getchar();

	return 0;
}