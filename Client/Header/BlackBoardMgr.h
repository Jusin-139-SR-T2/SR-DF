#pragma once
#include "Base.h"
#include "BlackBoard.h"

class CBlackBoardMgr :
    public CBase
{
	DERIVED_CLASS_SINGLETON(CBase, CBlackBoardMgr)
private:
	explicit CBlackBoardMgr();
	virtual ~CBlackBoardMgr();

private:
	virtual		void	Free();

public:
	HRESULT			Ready_BlackBoard(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	_unmap<wstring, CBlackBoard*>			m_umapBlackBoard;

};

