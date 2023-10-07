#include "BlackBoardMgr.h"

IMPLEMENT_SINGLETON(CBlackBoardMgr);

CBlackBoardMgr::CBlackBoardMgr()
{
}

CBlackBoardMgr::~CBlackBoardMgr()
{
}

void CBlackBoardMgr::Free()
{
	for (auto itemPair : m_mapBlackBoard)
	{
		Safe_Release(itemPair.second);
	}
	m_mapBlackBoard.clear();
}

HRESULT CBlackBoardMgr::Ready_BlackBoard()
{


	return S_OK;
}

_int CBlackBoardMgr::Update_BlackBoard()
{
	// 만료시 삭제하는 블랙보드 삭제.
	for (auto iter = m_mapBlackBoard.begin(); iter != m_mapBlackBoard.end();)
	{
		if ((*iter).second->IsExpired())
		{
			Safe_Release((*iter).second);
			iter = m_mapBlackBoard.erase(iter);
		}
		else
			++iter;
	}

	return 0;
}

HRESULT CBlackBoardMgr::Add_BlackBoard(const wstring& strBoardName, CBlackBoard* pBlackBoard)
{
	auto iter = m_mapBlackBoard.emplace(strBoardName, pBlackBoard);
	if (!iter.second)
	{
#ifdef _DEBUG
		OutputDebugString(strBoardName.c_str());
		OutputDebugString(L" : 블랙보드 추가 못함!\n");
#endif // _DEBUG
		return E_FAIL;
	}

	return S_OK;
}
