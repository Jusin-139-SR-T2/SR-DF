#pragma once

#include "Base.h"
#include "BlackBoard.h"

BEGIN(Engine)

/// <summary>
/// 블랙보드를 참조하는 위크 포인터 개념의 객체
/// 따로 RefCount를 가지지 않는다.
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
		// 이미 만료된 것은 해제코드 작동을 안하니 중복 해제해도 문제 없음
		// RefCount 감소
		Safe_Release(m_pBlackBoard);
	}

private:
	void Set_BlackBoard(CBlackBoard* const pBlackBoard)
	{
		if (pBlackBoard->IsExpired())
			return;
		m_pBlackBoard = pBlackBoard;
		// 설정된 블랙보드의 수명을 연장시킨다.
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
			// 만료된 블랙보드는 자동으로 nullptr를 채워준다.
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
/// 블랙보드 매니저
/// 블랙보드를 추가하고 블랙보드를 얻어와준다.
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




#pragma region CBlackBoardMgr 구현부

template<typename T>
inline HRESULT CBlackBoardMgr::Get_BlackBoard(const wstring& strBoardName, FBlackBoardPtr<T>& pBlackBoard)
{
	auto iter = m_mapBlackBoard.find(strBoardName);
	if (iter == m_mapBlackBoard.end())
	{
#ifdef _DEBUG
		OutputDebugString(strBoardName.c_str());
		OutputDebugString(L" : 블랙보드 찾지 못함!\n");
#endif // _DEBUG
		return E_FAIL;
	}

	pBlackBoard.Set_BlackBoard(*iter);

	return S_OK;
}


#pragma endregion