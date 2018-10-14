#pragma once

namespace ThreadpoolEngine
{
	using namespace ThreadpoolGroupManager;

	// TP_WORK, TP_WAIT, TP_TIMER, TP_IO 4���� �ݹ� ��ü�� �߻� Ÿ��
	interface IThreadpoolCallbackObject
	{
		//////////////////////////// ��ü ������ ������� ���ÿ� ����� �� ���� �Լ� ////////////////////////////
		// ThreadpoolGroupManager�� ����ϱ� ���� ������ ������ �߻�
		//
		// �ش� �ݹ� ��ü�� TP_CALLBACK_ENVIRON�� ���ε��ϵ��� �����ؾ� ��
		virtual BOOL BindThreadpoolCallbackObject(const PTP_CALLBACK_ENVIRON pTpCallbackEnviron, ERROR_CODE& errorCode) = 0;
		//
		// �ش� �ݹ� ��ü�� �����Ǵ� ���� �����ؾ� ��
		virtual BOOL ReleaseThreadpoolCallbackObject(BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode) = 0;
		//
		/////////////////////////////////////////////////////////////////////////////////////////////////////

		virtual ~IThreadpoolCallbackObject() { }
	};

	// TP_WORK, TP_WAIT, TP_TIMER, TP_IO 4������ ���
	template <typename PTP_OBJECT>
	class CThreadpoolCallbackObject : public IThreadpoolCallbackObject
	{
	protected:
		// �ݹ� ��ü
		PTP_OBJECT _pThreadpoolCallbackObject;

		// ������Ǯ �׷쿡 ���������� �߰��Ǿ����� ����
		BOOL _bIsGrouping;

		// �ݹ� �Լ��� ȣ��ǰ� �۾��� �ݹ� ������
		ICallbackData* _pCallbackData;

		// �ݹ� �Լ��� �����ϴ� �����帶�� �ݹ� ��ü�� �ν��Ͻ��� �����ϱ� ���� �޸� ����
		// ȣ��� �ݹ� �Լ� ���ο��� �ݹ� �Լ��� ������ �Ϸ�Ǳ⸦ ��ٸ��� ����� ���¿� ������ �ǹǷ� ȸ���ϱ� ���ؼ� �����
		__declspec(thread) static PTP_OBJECT _pThreadpoolCallbackObjectInstance;

		// �ݹ� �Լ��� �����ϴ� �����帶�� �ݹ� �ν��Ͻ� �����͸� �����ϱ� ���� �޸� ����
		// ȣ��� �ݹ� �Լ� ���ο��� �ݹ� �Լ��� ������ �Ϸ�Ǳ⸦ ��ٸ��� ����� ���¿� ������ �ǹǷ� ȸ���ϱ� ���ؼ� �����
		__declspec(thread) static PTP_CALLBACK_INSTANCE _pTpCallbackInstance;

	public:
		CThreadpoolCallbackObject();
		virtual ~CThreadpoolCallbackObject();

		PTP_OBJECT GetThreadpoolCallbackObject();

		// �ݹ� ��ü ������ ȣ�� �Ŀ� �� �Լ��� ȣ���ؾ� ��
		void InitializeThreadpoolCallbackObject(THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode);

		// �ݹ� ��ü�� �Ҹ��� ȣ�� ���� �� �Լ��� ȣ���ؾ� ��
		void FinalizeThreadpoolCallbackObject();

		// �ݹ� ������ ����� �ݹ� �Լ� ȣ��
		void CallbackThreadpoolCallbackObject(CALLBACK_DATA_PARAMETER callbackDataParameter, PTP_CALLBACK_INSTANCE pTpCallbackInstance);
	};
}

#include "ThreadpoolCallbackObject.inl"