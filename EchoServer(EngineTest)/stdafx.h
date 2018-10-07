// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: reference additional headers your program requires here
#include <map>
#include <set>

#include "Winsock2.h"
#include "Mswsock.h"
#include "iostream"
using namespace std;

//enum IO_TYPE : BYTE
//{
//	IO_TYPE_NONE	= 0,
//	IO_TYPE_ACCEPT	= 1,
//	IO_TYPE_RECV	= 2,
//	IO_TYPE_SEND	= 3,
//};

struct SOCK_ITEM : OVERLAPPED
{
	BYTE		_bySN;
	SOCKET		_sock;
	char		_buff[512];
	BOOL		_bReuse;
	//IO_TYPE		_ioType;

	SOCK_ITEM(BYTE bySN, SOCKET sock)
	{
		memset(this, 0, sizeof(*this));
		_bySN = bySN;
		_sock = sock;
		_bReuse = FALSE;
		//_ioType = IO_TYPE_NONE;
	}
};

#ifndef STATUS_LOCAL_DISCONNECT
#	define STATUS_LOCAL_DISCONNECT	((NTSTATUS)0xC000013BL)	//ERROR_NETNAME_DELETED
#endif
#ifndef STATUS_REMOTE_DISCONNECT
#	define STATUS_REMOTE_DISCONNECT	((NTSTATUS)0xC000013CL)	//ERROR_NETNAME_DELETED
#endif
#ifndef STATUS_CONNECTION_RESET
#	define STATUS_CONNECTION_RESET	((NTSTATUS)0xC000020DL)	//ERROR_NETNAME_DELETED
#endif
#ifndef STATUS_CANCELLED
#	define STATUS_CANCELLED			((NTSTATUS)0xC0000120L)	//ERROR_OPERATION_ABORTED
#endif

#pragma comment(lib, "Ws2_32.lib")

#pragma comment(lib, "ThreadpoolEngine.lib")

PVOID GetSockExtAPI(SOCKET sock, GUID guidFn);