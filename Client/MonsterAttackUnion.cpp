#include "stdafx.h"
#include "MonsterAttackUnion.h"
#include "Player.h"

CMonsterAttackUnion::CMonsterAttackUnion(LPDIRECT3DDEVICE9 pGraphicDev)
    :Base(pGraphicDev)
{
}

CMonsterAttackUnion::CMonsterAttackUnion(const CMonsterAttackUnion& rhs)
    : Base(rhs)
{
}

CMonsterAttackUnion::~CMonsterAttackUnion()
{
}

HRESULT CMonsterAttackUnion::Ready_GameObject()
{
    srand((_uint)time(NULL));

    return S_OK;
}

_int CMonsterAttackUnion::Update_GameObject(const _float& fTimeDelta)
{
    m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
    NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

    vPlayerPos = m_pPlayerTransformcomp->Get_Pos();

    SUPER::Update_GameObject(fTimeDelta);

    return S_OK;
}

void CMonsterAttackUnion::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CMonsterAttackUnion::Render_GameObject()
{
}

void CMonsterAttackUnion::Free()
{
    SUPER::Free();
}

#pragma region 공통셋팅 파트 

void CMonsterAttackUnion::Height_On_Terrain(_float Height)
{
    _vec3		vPos;
    m_pTransformComp->Get_Info(INFO_POS, &vPos);

    CTerrainBufferComp* pTerrainBufferComp = dynamic_cast<CTerrainBufferComp*>(Engine::Get_Component(ID_STATIC, L"Environment", L"Terrain", L"Com_Buffer"));
    NULL_CHECK(pTerrainBufferComp);

    _float	fHeight = m_pCalculatorComp->Compute_HeightOnTerrain(&vPos,
        pTerrainBufferComp->Get_VtxPos(),
        pTerrainBufferComp->Get_VertexCountX() + 1U,
        pTerrainBufferComp->Get_VertexCountZ() + 1U,
        pTerrainBufferComp->Get_Scale(),
        pTerrainBufferComp->Get_InvOffset());

    m_pTransformComp->Set_Pos(vPos.x, fHeight + Height, vPos.z);
}

void CMonsterAttackUnion::Billboard()
{
    // 몬스터가 플레이어 바라보는 벡터 
    _vec3 vDir = vPlayerPos - m_pTransformComp->Get_Pos();

    D3DXVec3Normalize(&vDir, &vDir);

    _float rad = atan2f(vDir.x, vDir.z);

    m_pTransformComp->Set_RotationY(rad);
}


void CMonsterAttackUnion::Knockback_Player(const _float& fTimeDelta, _float fSpeed)
{
    // 공격체가 플레이어 바라보는 방향 
    // 애초에 플레이어 뒤를 가리키는 벡터이므로 순수 속력만 넣어주면된다. 

    _vec3 Dir = vPlayerPos - m_pTransformComp->Get_Pos();
    D3DXVec3Normalize(&Dir, &Dir);

    m_pPlayerTransformcomp->Move_Pos(&Dir, fTimeDelta, fSpeed);
}

void CMonsterAttackUnion::Change_PlayerHp(_float pAttack)
{
    // + - 둘다 가능하게 바꿈 
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"GameLogic", L"Player"));
    PlayerHp = pPlayer->Get_PlayerHP();

    PlayerHp.Cur += pAttack;

    if (PlayerHp.Cur <= 0)
        PlayerHp.Cur = 0.f;
    if (PlayerHp.IsMax())
        PlayerHp.Cur = PlayerHp.Max;

    pPlayer->Set_PlayerHP(PlayerHp);
}

#pragma endregion