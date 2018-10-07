#include "stdafx.h"
#include "Export.h"
#include "CallbackDataForListenSocket.h"
#include "CallbackDataForChildSocket.h"

CCallbackDataForListenSocket::CCallbackDataForListenSocket(CThreadpoolCallbackIoWrapper* pThreadpoolCallbackIoWrapper, SOCKET listenSocket)
{
	_pThreadpoolCallbackIoWrapper = pThreadpoolCallbackIoWrapper;
	_bDeleteThisObject = FALSE;
	_listenSocket = listenSocket;
	_callbackDataForChildSocketSet.clear();
	::InitializeCriticalSection(&_criticalSection);
}

CCallbackDataForListenSocket::~CCallbackDataForListenSocket()
{
	ERROR_CODE errorCode = ERROR_CODE_NONE;
	CCallbackDataForChildSocket* pCallbackDataForChildSocket = NULL;
	CThreadpoolCallbackIoWrapper* pThreadpoolCallbackIoWrapper = NULL;
	std::set<CCallbackDataForChildSocket*>::iterator itCursor = _callbackDataForChildSocketSet.begin();
	std::set<CCallbackDataForChildSocket*>::iterator itFinish = _callbackDataForChildSocketSet.end();
	for (; itCursor != itFinish; ++itCursor)
	{
		pCallbackDataForChildSocket = (*itCursor);
		pThreadpoolCallbackIoWrapper = pCallbackDataForChildSocket->GetThreadpoolCallbackIoWrapper();

		// RECV 상태인 비동기 입출력에 대해서 취소 요청
		pThreadpoolCallbackIoWrapper->CancelThreadpoolCallbackIo(FALSE, errorCode);
		delete pThreadpoolCallbackIoWrapper;
		delete pCallbackDataForChildSocket->GetSockItem();
		delete pCallbackDataForChildSocket;
	}

	::DeleteCriticalSection(&_criticalSection);
}

void CCallbackDataForListenSocket::CallbackFunction(CALLBACK_DATA_PARAMETER callbackDataParameter)
{
	CThreadpoolCallbackIoWrapper* pThreadpoolCallbackIoWrapper = NULL;
	CCallbackDataForChildSocket* pCallbackDataForChildSocket = NULL;
	ERROR_CODE errorCode = ERROR_CODE_NONE;
	BYTE bySN = 0;

	SOCK_ITEM* pSockItem = (SOCK_ITEM*)callbackDataParameter._pOverlapped;

	try
	{
		// 리소스 정리하기 위한 식별할 뭔가가 없음. 프로그램 종료 시 리소스 회수
		if (NULL == pSockItem)
		{
			errorCode = (ERROR_CODE)callbackDataParameter._ulIoResult;
			cout << "[SN : ?] [" << __FUNCTION__ << "] checking pSockItem null failed " << std::hex << errorCode << endl;
			throw (ERROR_CODE)errorCode;
		}

		bySN = pSockItem->_bySN;

		if (ERROR_CODE_NONE != callbackDataParameter._errorCode)
		{
			cout << "[SN : " << (int)bySN << "] [" << __FUNCTION__ << "] checking error code failed " << callbackDataParameter._errorCode << endl;
			throw (ERROR_CODE)callbackDataParameter._errorCode;
		}

		if (0 != pSockItem->Internal)
		{
			errorCode = (ERROR_CODE)pSockItem->Internal;
			cout << "[SN : " << (int)bySN << "] [" << __FUNCTION__ << "] checking pSockItem internal failed " << std::hex << errorCode << endl;
			throw (ERROR_CODE)errorCode;
		}

		// 재사용이 아닌 경우만 새로 세팅
		if (FALSE == pSockItem->_bReuse)
		{
			pThreadpoolCallbackIoWrapper = new CThreadpoolCallbackIoWrapper((HANDLE)pSockItem->_sock, THREADPOOL_GROUP_PARAMETER((ThreadpoolGroup)1, 1, 10), errorCode);
			if (ERROR_CODE_NONE != errorCode)
			{
				cout << "[SN : " << (int)bySN << "] [" << __FUNCTION__ << "] creating CThreadpoolCallbackIoWrapper failed " << std::hex << errorCode << endl;
				throw (ERROR_CODE)errorCode;
			}

			pCallbackDataForChildSocket = new CCallbackDataForChildSocket(pThreadpoolCallbackIoWrapper, this, pSockItem->_sock, pSockItem);
			if (FALSE == pThreadpoolCallbackIoWrapper->SetCallbackData(pCallbackDataForChildSocket, errorCode))
			{
				cout << "[SN : " << (int)bySN << "] [" << __FUNCTION__ << "] creating CCallbackDataForChildSocket failed " << std::hex << errorCode << endl;
				throw (ERROR_CODE)errorCode;
			}
		}
		// 재사용인 경우 기존에 사용하던 것 세팅
		else
		{
			pCallbackDataForChildSocket = FindCallbackDataForChildSocket(pSockItem->_sock);
			if (NULL == pCallbackDataForChildSocket)
			{
				cout << "[SN : " << (int)bySN << "] [" << __FUNCTION__ << "] FindCallbackDataForChildSocket failed " << endl;
				throw (ERROR_CODE)ERROR_CODE_DOES_NOT_EXIST;
			}

			pThreadpoolCallbackIoWrapper = pCallbackDataForChildSocket->GetThreadpoolCallbackIoWrapper();
		}

		if (FALSE == pThreadpoolCallbackIoWrapper->ExecuteThreadpoolCallbackIo(errorCode))
		{
			if (ERROR_IO_PENDING == errorCode)
			{
				errorCode = ERROR_CODE_NONE;
			}
			else
			{
				cout << "[SN : " << (int)bySN << "] [" << __FUNCTION__ << "] ExecuteThreadpoolCallbackIo failed " << std::hex << errorCode << endl;
				throw (ERROR_CODE)errorCode;
			}
		}

		DWORD dwFlags = 0;
		WSABUF wb;
		wb.buf = pSockItem->_buff, wb.len = sizeof(pSockItem->_buff);
		int nSockRet = WSARecv(pSockItem->_sock, &wb, 1, NULL, &dwFlags, pSockItem, NULL);
		if (nSockRet == SOCKET_ERROR)
		{
			if (STATUS_PENDING != pSockItem->Internal)
			{
				errorCode = (ERROR_CODE)pSockItem->Internal;
				cout << "[SN : " << (int)bySN << "] [" << __FUNCTION__ << "] receiving data failed " << std::hex << errorCode << endl;
				throw (ERROR_CODE)errorCode;
			}
		}

		// recv 상태로 세팅
		// pSockItem->_ioType = IO_TYPE_RECV;

		// 재사용이 아닌 경우만 새로 추가
		if (FALSE == pSockItem->_bReuse)
		{
			::EnterCriticalSection(&_criticalSection);
			_callbackDataForChildSocketSet.insert(pCallbackDataForChildSocket);
			::LeaveCriticalSection(&_criticalSection);

			// 소켓풀에 소켓 1개 추가
			SetChildSocketPool(NULL, FALSE, errorCode);
		}

		cout << "[SN : " << (int)bySN << "] [" << __FUNCTION__ << "] accepted client" << endl;
	}
	catch (ERROR_CODE errorCodeCatched)
	{
		BYTE bySN = 0;
		ERROR_CODE errorCode2 = ERROR_CODE_NONE;
		errorCode = ERROR_CODE_NONE;

		// pSockItem이 NULL이 아니고 재사용이 아닌 경우는 리소스 정리
		// _callbackDataForChildSocketSet에 넣기 전이므로 CCallbackDataForListenSocket 소멸자에서 정리되지 않으니 여기서 정리
		if (NULL != pSockItem)
		{
			if (FALSE == pSockItem->_bReuse)
			{
				if (NULL != pThreadpoolCallbackIoWrapper)
				{
					delete pThreadpoolCallbackIoWrapper;
				}

				if (NULL != pCallbackDataForChildSocket)
				{
					delete pCallbackDataForChildSocket;
				}
			}

			if (STATUS_LOCAL_DISCONNECT != (NTSTATUS)errorCodeCatched
				&& STATUS_CANCELLED != (NTSTATUS)errorCodeCatched)
			{
				if (FALSE == SetChildSocketPool(pSockItem, FALSE, errorCode))
				{
					// 소켓풀에 넣는 작업이 실패하면 어쩔 수 없음, 그냥 방치해 뒀다가 프로그램 종료 시 리소스 회수
					// 실제 서비스에선 회수하는 별도 모듈이 있어야 함
					cout << "[SN : " << (int)bySN << "] [" << __FUNCTION__ << "] reusing socket failed " << std::hex << errorCode << endl;
				}
			}
		}
		
		//if (ERROR_CODE_NONE < errorCodeCatched)
		//{
		//	if (NULL != pSockItem)
		//	{
		//		bySN = pSockItem->_bySN;
		//
		//		// AcceptEx에서 자식 소켓으로 시도한 비동기 accept가 실패한 경우
		//		if (STATUS_CONNECTION_RESET == (NTSTATUS)errorCodeCatched)
		//		{
		//			if (TRUE == SetChildSocketPool(pSockItem, errorCode2))
		//			{
		//				cout << "Listen socket callback function(1) occurs " << errorCodeCatched << ". But continue. SN : " << (int)bySN << endl;
		//				return;
		//			}
		//		}
		//		// AcceptEx로 등록한 자식 소켓을 닫았을 경우
		//		else if (STATUS_LOCAL_DISCONNECT == (NTSTATUS)errorCodeCatched)
		//		{
		//
		//		}
		//		// 이 경우는 말이 안 됨, 아직 accept도 안했는데;
		//		else if (STATUS_REMOTE_DISCONNECT == (NTSTATUS)errorCodeCatched)
		//		{
		//
		//		}
		//		// AcceptEx로 등록한 자식 소켓에 대해 취소했을 경우?
		//		else if (STATUS_CANCELLED == (NTSTATUS)errorCodeCatched)
		//		{
		//
		//		}
		//		// 그 외 예상치 못한 부분
		//		else
		//		{
		//
		//		}
		//
		//		::closesocket(pSockItem->_sock);
		//		delete pSockItem;
		//	}
		//
		//	cout << "Listen socket callback function(1) occurs " << errorCodeCatched << ", " << errorCode2 << ". SN : " << (int)bySN << endl;
		//	return;
		//}
		//else
		//{
		//	if (NULL != pSockItem)
		//	{
		//		bySN = pSockItem->_bySN;
		//
		//		if (TRUE == SetChildSocketPool(pSockItem, errorCode2))
		//		{
		//			cout << "Listen socket callback function(2) occurs " << errorCodeCatched << ". But continue. SN : " << (int)bySN << endl;
		//			return;
		//		}
		//	}
		//}
		//
		//cout << "Listen socket callback function(2) occurs " << errorCodeCatched << ", " << errorCode2 << ". SN : " << (int)bySN << endl;
	}

}

BOOL CCallbackDataForListenSocket::DeleteCallbackDataAutomatically()
{
	return _bDeleteThisObject;
}

BOOL CCallbackDataForListenSocket::SetChildSocketPool(SOCK_ITEM* pSockItemOld, BOOL bSetReuse, ERROR_CODE& errorCode)
{
	static BYTE bySN = 0;

	SOCK_ITEM* pSockItem = pSockItemOld;

	// pSockItem이 NULL이면 소켓풀 생성
	if (NULL == pSockItem)
	{
		++bySN;

		SOCKET childSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (INVALID_SOCKET == childSocket)
		{
			errorCode = (ERROR_CODE)::WSAGetLastError();
			cout << "[SN : " << (int)bySN << "] [" << __FUNCTION__ << "] creating socket failed " << std::hex << errorCode << endl;
			return FALSE;
		}

		pSockItem = new SOCK_ITEM(bySN, childSocket);

		cout << "[SN : " << (int)bySN << "] [" << __FUNCTION__ << "] created new socket" << endl;
	}
	// NULL이 아니면 소켓 재사용
	else
	{
		bySN = pSockItem->_bySN;

		LPFN_DISCONNECTEX pfnDisconnectEx =
			(LPFN_DISCONNECTEX)GetSockExtAPI(pSockItem->_sock, WSAID_DISCONNECTEX);
		BOOL bIsOK = pfnDisconnectEx(pSockItem->_sock, NULL, TF_REUSE_SOCKET, 0);
		if (FALSE == bIsOK)
		{
			errorCode = (ERROR_CODE)::WSAGetLastError();
			cout << "[SN : " << (int)bySN << "] [" << __FUNCTION__ << "] disconnecting socket failed " << std::hex << errorCode << endl;
			return FALSE;
		}

		// reuse 세팅해야할 때만 세팅, 리슨 소켓에서 accept들어왔는데 실패한 경우 기존에 재사용 하지 않았다면 재사용 세팅을 하지 말아야 함
		if (TRUE == bSetReuse)
		{
			pSockItem->_bReuse = TRUE;
		}

		cout << "[SN : " << (int)bySN << "] [" << __FUNCTION__ << "] reuse socket" << endl;
	}

	// 비동기 입출력 함수 호출 전에 IO 시작
	if (FALSE == _pThreadpoolCallbackIoWrapper->ExecuteThreadpoolCallbackIo(errorCode))
	{
		if (ERROR_IO_PENDING == errorCode)
		{
			errorCode = ERROR_CODE_NONE;
		}
		else
		{
			cout << "[SN : " << (int)bySN << "] [" << __FUNCTION__ << "] ExecuteThreadpoolCallbackIo failed " << std::hex << errorCode << endl;

			// 새로 생성한 경우만 리소스 정리
			if (NULL == pSockItemOld)
			{
				::closesocket(pSockItem->_sock);
				delete pSockItem;
			}

			return FALSE;
		}
	}

	LPFN_ACCEPTEX pfnAcceptEx = (LPFN_ACCEPTEX)::GetSockExtAPI(_listenSocket, WSAID_ACCEPTEX);

	BOOL bIsOK = pfnAcceptEx
	(
		_listenSocket, pSockItem->_sock, pSockItem->_buff, 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16,
		NULL, (LPOVERLAPPED)pSockItem
	);

	if (bIsOK == FALSE)
	{
		// if (WSAGetLastError() != WSA_IO_PENDING)
		if (STATUS_PENDING != pSockItem->Internal)
		{
			errorCode = (ERROR_CODE)::WSAGetLastError();

			cout << "[SN : " << (int)bySN << "] [" << __FUNCTION__ << "] accept failed " << std::hex << errorCode << endl;

			// 새로 생성한 경우만 리소스 정리
			if (NULL == pSockItemOld)
			{
				::closesocket(pSockItem->_sock);
				delete pSockItem;
			}

			return FALSE;
		}
	}

	// accept 상태로 세팅
	// pSockItem->_ioType = IO_TYPE_ACCEPT;

	return TRUE;
}

CCallbackDataForChildSocket* CCallbackDataForListenSocket::FindCallbackDataForChildSocket(SOCKET childSocket)
{
	std::set<CCallbackDataForChildSocket*>::iterator itCursor = _callbackDataForChildSocketSet.begin();
	std::set<CCallbackDataForChildSocket*>::iterator itFinish = _callbackDataForChildSocketSet.end();
	for (; itCursor != itFinish; ++itCursor)
	{
		if ((*itCursor)->GetChildSocket() == childSocket)
		{
			return (*itCursor);
		}
	}

	return NULL;
}