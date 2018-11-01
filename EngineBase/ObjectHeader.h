#pragma once

#include "Common.h"

namespace EngineBase
{
	// ��üǮ�� ��ü�� �����ϱ� ���� ��ü ���
	template <class Object>
	class CObjectHeader
	{
	public:
		// ��� ������ ���� ��ü ����� ����Ű�� ������
		CObjectHeader* _pNextUsableObjectHeader;

		// ������ ����� ��ü�� ������ ����Ű�� ������
		// �� ��� ���� �������� ��ü ������ ������
		Object* _pObject;
	};
}