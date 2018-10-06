#pragma once

#include <Windows.h>

#include <iostream>

#include <map>
#include <set>
#include <list>
#include <queue>

/*
// ������Ʈ ���ο��� ����ϴ� ����/���� ���� �ڵ�
// ������Ʈ ���ο��� ������ ���� �ڵ�� ������ ����ϸ� ����� DWORD GetLastError �ڵ�� ���
enum ERROR_CODE : __int64
{
	// ���� �ƴ� (����)
	ERROR_CODE_NONE
	= 0, 

	// ���� ���� //
	// �̹� ����
	ERROR_CODE_ALREADY_EXISTS
	= -1, 

	// �������� ����
	ERROR_CODE_DOES_NOT_EXIST
	= -2, 

	// ��ȿ���� ���� ����
	ERROR_CODE_INVALID_ACCESS
	= -3, 

	// CThreadpoolCallbackWork ���� ���� �ڵ� //
	// �ݹ� ��ü�� �̹� ���ε� �Ǿ���
	ERROR_CODE_THREADPOOL_CALLBACK_WORK_ALREADY_BOUND
	= -101,	

	// �ݹ� ��ü�� �ݹ� �Լ��� ������
	ERROR_CODE_THREADPOOL_CALLBACK_WORK_CALLBACK_FUNCTION_RUNNING
	= -102, 

	// �ݹ� ��ü�� �ݹ� �Լ��� �Ѱ��� �ݹ� �����Ͱ� ���õ��� ����
	ERROR_CODE_THREADPOOL_CALLBACK_WORK_CALLBACK_DATA_NOT_SET
	= -103,
	
	// �ݹ� ��ü�� ���ε��� ���� ����
	ERROR_CODE_THREADPOOL_CALLBACK_WORK_NOT_BOUND
	= -104, 

	// �ݹ� ��ü�� NULL�� ����
	ERROR_CODE_THREADPOOL_CALLBACK_WORK_PTP_WORK_IS_NULL
	= -105, 

	// �ݹ� �Լ��� ȣ��Ǳ� ���� �ݹ� ��ü�� ������
	ERROR_CODE_THREADPOOL_CALLBACK_WORK_RELEASE_CALLBACK_OBJECT
	= -106,


	// CThreadpoolCallbackIo ���� ���� �ڵ� //
	// �ݹ� ��ü�� �̹� ���ε� �Ǿ���
	ERROR_CODE_THREADPOOL_CALLBACK_IO_ALREADY_BOUND
	= -401,

	// �ݹ� ��ü�� �ݹ� �Լ��� ������
	ERROR_CODE_THREADPOOL_CALLBACK_IO_CALLBACK_FUNCTION_RUNNING
	= -402,

	// �ݹ� ��ü�� �ݹ� �Լ��� �Ѱ��� �ݹ� �����Ͱ� ���õ��� ����
	ERROR_CODE_THREADPOOL_CALLBACK_IO_CALLBACK_DATA_NOT_SET
	= -403,

	// �ݹ� ��ü�� ���ε��� ���� ����
	ERROR_CODE_THREADPOOL_CALLBACK_IO_NOT_BOUND
	= -404,

	// �ݹ� ��ü�� NULL�� ����
	ERROR_CODE_THREADPOOL_CALLBACK_IO_PTP_IO_IS_NULL
	= -405,

	// IOCP�� ���ε��� ��ġ�� ��ȿ���� ����
	ERROR_CODE_THREADPOOL_CALLBACK_IO_INVALID_DEVICE
	= -410,

	// �ݹ� �����Ͱ� �̹� ���õǾ���
	ERROR_CODE_THREADPOOL_CALLBACK_IO_CALLBACK_DATA_ALREADY_SET
	= -411,
};
*/