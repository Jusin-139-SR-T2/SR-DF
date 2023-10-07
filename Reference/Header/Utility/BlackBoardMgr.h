#pragma once

#include "Base.h"
#include "BlackBoard.h"

BEGIN(Engine)

/// <summary>
/// �����带 �����ϴ� ��ũ ������ ������ ��ü
/// ���� RefCount�� ������ �ʴ´�.
/// </summary>
template <typename T>
class ENGINE_DLL FBlackBoardPtr
{
	friend class CBlackBoardMgr;
private:
	explicit FBlackBoardPtr()
		: m_pBlackBoard(nullptr)
	{}
	virtual ~FBlackBoardPtr()
	{
		// �̹� ����� ���� �����ڵ� �۵��� ���ϴ� �ߺ� �����ص� ���� ����
		// RefCount ����
		Safe_Release(m_pBlackBoard);
	}

private:
	void Set_BlackBoard(CBlackBoard* const pBlackBoard)
	{
		if (pBlackBoard->IsExpired())
			return;
		m_pBlackBoard = pBlackBoard;
		// ������ �������� ������ �����Ų��.
		pBlackBoard->AddRef();
	}

public:
	T* Get_BlackBoard()
	{
		if (m_pBlackBoard == nullptr)
			return nullptr;

		if (m_pBlackBoard->IsExpired())
		{
			Safe_Release(m_pBlackBoard);
			// ����� ������� �ڵ����� nullptr�� ä���ش�.
			return m_pBlackBoard = nullptr;
		}

		return m_pBlackBoard;
	}

private:
	_bool		IsAvailable() { return m_pBlackBoard->IsExpired(); }

private:
	CBlackBoard* m_pBlackBoard;
};



/// <summary>
/// ������ �Ŵ���
/// �����带 �߰��ϰ� �����带 �����ش�.
/// </summary>
class ENGINE_DLL CBlackBoardMgr : public CBase
{
	DERIVED_CLASS_SINGLETON(CBase, CBlackBoardMgr)
private:
	explicit CBlackBoardMgr();
	virtual ~CBlackBoardMgr();

private:
	virtual	void	Free();

public:
	HRESULT			Ready_BlackBoard();
	_int			Update_BlackBoard();

public:
	HRESULT			Add_BlackBoard(const wstring& strBoardName, CBlackBoard* pBlackBoard);
	template <typename T>
	HRESULT			Get_BlackBoard(const wstring& strBoardName, FBlackBoardPtr<T>& pBlackBoard);

private:
	_unmap<wstring, CBlackBoard*>			m_mapBlackBoard;

};

END




#pragma region CBlackBoardMgr ������

template<typename T>
inline HRESULT CBlackBoardMgr::Get_BlackBoard(const wstring& strBoardName, FBlackBoardPtr<T>& pBlackBoard)
{
	auto iter = m_mapBlackBoard.find(strBoardName);
	if (iter == m_mapBlackBoard.end())
	{
#ifdef _DEBUG
		OutputDebugString(strBoardName.c_str());
		OutputDebugString(L" : ������ ã�� ����!\n");
#endif // _DEBUG
		return E_FAIL;
	}

	pBlackBoard.Set_BlackBoard(*iter);

	return S_OK;
}


#pragma endregion