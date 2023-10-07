#include "stdafx.h"
#include "BlackBoard_Player.h"

#include "Player.h"

CBlackBoard_Player::CBlackBoard_Player()
    : Base()
    , m_fHP(GAUGE<_float>(100.f)), m_fStamina(GAUGE<_float>(100.f))
{
}

CBlackBoard_Player::CBlackBoard_Player(const CBlackBoard_Player& rhs)
    : Base(rhs)
{
}

CBlackBoard_Player::~CBlackBoard_Player()
{
    Free();
}

void CBlackBoard_Player::Free()
{
    
}

HRESULT CBlackBoard_Player::Ready_BlackBoard(const FBlackBoard_PlayerInit& tInit)
{
    m_pPlayer = tInit.pPlayer;

    return S_OK;
}
