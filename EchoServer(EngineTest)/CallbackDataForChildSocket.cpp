#include "stdafx.h"
#include "Export.h"
#include "CallbackDataForChildSocket.h"
#include "CallbackDataForListenSocket.h"

CCallbackDataForChildSocket::CCallbackDataForChildSocket(CThreadpoolCallbackIoWrapper* pThreadpoolCallbackWrapper, CCallbackDataForListenSocket* pCallbackDataForListenSocket, SOCKET childSocket, SOCK_ITEM* pSockItem)
{
	_pThreadpoolCallbackIoWrapper = pThreadpoolCallbackWrapper;
	_bDeleteThisObject = FALSE;
	_pCallbackDataForListenSocket = pCallbackDataForListenSocket;
	_childSocket = childSocket;
	_pSockItem = pSockItem;
}

CThreadpoolCallbackIoWrapper* CCallbackDataForChildSocket::GetThreadpoolCallbackIoWrapper()
{
	return _pThreadpoolCallbackIoWrapper;
}

SOCK_ITEM* CCallbackDataForChildSocket::GetSockItem()
{
	return _pSockItem;
}

void CCallbackDataForChildSocket::CallbackFunction(CALLBACK_DATA_PARAMETER callbackDataParameter)
{
	BYTE bySN = 0;
	ERROR_CODE errorCode = ERROR_CODE_NONE;

	SOCK_ITEM* pSockItem = (SOCK_ITEM*)(callbackDataParameter._pOverlapped);

	try
	{
		// 리소스 정리하기 위한 식별할 뭔가가 없음. 프로그램 종료 시 리소스 회수
		if (NULL == pSockItem)
		{
			errorCode = (ERROR_CODE)callbackDataParameter._ulIoResult;
			cout << "[SN : ?] [" << __FUNCTION__ << "] checking pSockItem null failed " << errorCode << endl;
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

		// 클라가 소켓을 닫음
		if (0 == callbackDataParameter._ulpNumberOfBytesTransferred)
		{
			cout << "[SN : " << (int)bySN << "] [" << __FUNCTION__ << "] client closed" << endl;

			// 정상적인 경우로 소켓을 재사용하도록 유도
			throw (ERROR_CODE)ERROR_CODE_NONE;
		}
		else if (0 < callbackDataParameter._ulpNumberOfBytesTransferred)
		{
			pSockItem->_buff[callbackDataParameter._ulpNumberOfBytesTransferred] = 0;

			cout << "[SN : " << (int)bySN << "] [" << __FUNCTION__ << "] received Data " << pSockItem->_buff << endl;

			int lSockRet = send(pSockItem->_sock, pSockItem->_buff, (int)callbackDataParameter._ulpNumberOfBytesTransferred, 0);
			if (lSockRet == SOCKET_ERROR)
			{
				errorCode = (ERROR_CODE)::WSAGetLastError();
				cout << "[SN : " << (int)bySN << "] [" << __FUNCTION__ << "] sending data failed " << std::hex << errorCode << endl;
				throw (ERROR_CODE)errorCode;
			}
		}
		

		if (FALSE == _pThreadpoolCallbackIoWrapper->ExecuteThreadpoolCallbackIo(errorCode))
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

	}
	catch (ERROR_CODE errorCodeCatched)
	{
		if (NULL != pSockItem)
		{
			// 소켓을 닫는 것도 아니고 입출력 취소를 한 경우도 아니면 소켓 재사용
			// 즉, 의도적으로 발생시킨 예외가 아니면 해당 소켓을 disconnect하고 accept 풀에 다시 넣음
			if (STATUS_LOCAL_DISCONNECT != (NTSTATUS)errorCodeCatched
				&& STATUS_CANCELLED != (NTSTATUS)errorCodeCatched)
			{
				errorCode = ERROR_CODE_NONE;
				if (FALSE == _pCallbackDataForListenSocket->SetChildSocketPool(pSockItem, TRUE, errorCode))
				{
					// 소켓풀에 넣는 작업이 실패하면 어쩔 수 없음, 그냥 방치해 뒀다가 프로그램 종료 시 리소스 회수
					// 실제 서비스에선 회수하는 별도 모듈이 있어야 함
					cout << "[SN : " << (int)bySN << "] [" << __FUNCTION__ << "] reusing socket failed " << std::hex << errorCode << endl;
				}
			}
		}
	}
}

BOOL CCallbackDataForChildSocket::DeleteCallbackDataAutomatically()
{
	return _bDeleteThisObject;
}

SOCKET CCallbackDataForChildSocket::GetChildSocket()
{
	return _childSocket;
}