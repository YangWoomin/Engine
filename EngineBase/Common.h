#pragma once

#include <Windows.h>

#include <iostream>

#include <mutex>

#include <map>
#include <set>
#include <list>
#include <queue>
#include <vector>
#include <deque>

namespace EngineBase
{
	// ������Ʈ ���ο��� ����ϴ� ����/���� ���� �ڵ�
	// ������Ʈ ���ο��� ������ ���� �ڵ�� INT_MIN �̸��� ������ ���
	// ����� DWORD GetLastError, int WSAGetLastError,  LONG NTSTATUS �ڵ�� ���
	enum ERROR_CODE : __int64
	{
		// ���� �ƴ� (����)
		ERROR_CODE_NONE
		= 0,

		// ������Ʈ ������ ���� �ڵ� ���ذ�
		ERROR_CODE_BASE
		= MINLONGLONG, // ~(0x7fffffffffffffff)

		// �� ���� ������ ����
		ERROR_CODE_HEAP_CREATION_FAILED
		= ERROR_CODE_BASE + 1,

		// �޸� ���� �̹� �Ҵ��
		ERROR_CODE_MEMORY_BLOCK_ALREADY_ALLOCATED
		= ERROR_CODE_BASE + 2,

		// �޸� ���� �Ҵ���� ����
		ERROR_CODE_MEMORY_BLOCK_NEVER_ALLOCATED
		= ERROR_CODE_BASE + 3,

		// ��ȿ���� ���� ��ü ����
		ERROR_CODE_INVALID_OBJECT_COUNT
		= ERROR_CODE_BASE + 4,

		// ��� ������ ��ü�� ����
		ERROR_CODE_NO_USABLE_OBJECT
		= ERROR_CODE_BASE + 5,

		// ��üǮ�� ���� ���ִ� ����
		ERROR_CODE_FULL_OBJECTS_IN_MEMORY_BLOCK
		= ERROR_CODE_BASE + 6,

		// ��ȿ���� ���� ��ü�� ��ȯ
		ERROR_CODE_INVALID_OBJECT_RETURN
		= ERROR_CODE_BASE + 7,

		// ::HeapFree �Լ� ȣ�⿡ ����
		ERROR_CODE_FAILURE_IN_HEAP_FREE_FUNCTION
		= ERROR_CODE_BASE + 8,

		// ���� ���� ���� �ڵ� �ִ�
		ERROR_CODE_MAX
		= ERROR_CODE_BASE + MAXINT
	};
}