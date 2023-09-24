#pragma once
#include "GameObject.h"
#include "Player.h"
#include "Gray.h"
#include "Brown.h"
#include "Boss.h"

#include "Export_System.h"
#include "Export_Utility.h"
#include "Engine_Macro.h"

class   CMarket:
    public CGameObject
{
private:
    explicit CMarket();
    explicit CMarket(const CMarket& rhs);
    virtual ~CMarket();

public:
    virtual HRESULT Ready_GameObject() override;
    virtual _int	Update_GameObject(const _float& fTimeDelta) override;
    virtual void	LateUpdate_GameObject() override;
    void Render_GameObject() override;
    static CBrown* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
    HRESULT				Add_Component();
    virtual void		Free();


private:
    CPlayer* m_pPlayer = nullptr;
    CBrown* m_pBrown = nullptr;
    CGray* m_pGray = nullptr;
    CBoss* m_pBoss = nullptr;


// Get, Set �Լ� ---------------------------------------------------------

public:
    GETSET_EX2(CPlayer*, m_pPlayer, Player, GET, SET)
    GETSET_EX2(CBrown*, m_pBrown, Brown, GET, SET)
    GETSET_EX2(CGray*, m_pGray, Gray, GET, SET)
    GETSET_EX2(CBoss*, m_pBoss, Boss, GET, SET)

// ----------------------------------------------------------------------
};

