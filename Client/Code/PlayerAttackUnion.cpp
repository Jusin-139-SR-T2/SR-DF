#include "PlayerAttackUnion.h"
#include "stdafx.h"
#include "Player.h"
#include "AceMonster.h"

//����
#include "AceBoss.h"
// ����
#include "Gray.h"
#include "Brown.h"

CPlayerAttackUnion::CPlayerAttackUnion(LPDIRECT3DDEVICE9 pGraphicDev)
    :Base(pGraphicDev)
{
}

CPlayerAttackUnion::CPlayerAttackUnion(const CPlayerAttackUnion& rhs)
    : Base(rhs)
{
}

CPlayerAttackUnion::~CPlayerAttackUnion()
{
}

HRESULT CPlayerAttackUnion::Ready_GameObject()
{
    srand((_uint)time(NULL));

    return S_OK;
}

_int CPlayerAttackUnion::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    return S_OK;
}

void CPlayerAttackUnion::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CPlayerAttackUnion::Render_GameObject()
{
}

void CPlayerAttackUnion::Free()
{
    SUPER::Free();
}

#pragma region ����

void CPlayerAttackUnion::Height_On_Terrain(_float Height)
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

void CPlayerAttackUnion::Billboard()
{
    // �ش� ������ �÷��̾ �ٶ󺸴� ���� 
    _vec3 vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();

    D3DXVec3Normalize(&vDir, &vDir);

    _float rad = atan2f(vDir.x, vDir.z);

    m_pTransformComp->Set_RotationY(rad);
}

HRESULT CPlayerAttackUnion::Update_PlayerPos()
{
    m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
    NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

    return S_OK;
}


void CPlayerAttackUnion::Recoil_Player(const _float& fTimeDelta, _float fSpeed)
{
    // ����ü(�Ѿ�)�� �÷��̾� �ٶ󺸴� ����
    _vec3 Dir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();
    D3DXVec3Normalize(&Dir, &Dir);

    m_pPlayerTransformcomp->Move_Pos(&Dir, fTimeDelta, fSpeed);
}

void CPlayerAttackUnion::Change_PlayerHp(_float pAttack)
{
    // + - �Ѵ� �����ϰ� �ٲ� 
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"GameLogic", L"Player"));
    PlayerHp = pPlayer->Get_PlayerHP();

    PlayerHp.Cur += pAttack;

    if (PlayerHp.Cur <= 0)
        PlayerHp.Cur = 0.f;
    if (PlayerHp.IsMax())
        PlayerHp.Cur = PlayerHp.Max;

    pPlayer->Set_PlayerHP(PlayerHp);
}

void CPlayerAttackUnion::Change_MonsterHp(_float pAttack, CGameObject* _AttackTarget, PLAYER_ATTACK_STATE _AttackState)
{
    CAceGameObject* pAceObj = dynamic_cast<CAceGameObject*>(_AttackTarget);

    if (pAceObj == nullptr)
        return;

    if (Check_Relation(pAceObj, this) == ERELATION::HOSTILE)
    {
        // Ÿ�� ����
        CAceMonster* pMonster = dynamic_cast<CAceMonster*>(_AttackTarget);

        // ���ݹ��� ����(Ÿ��)�� ���� ���
        if (nullptr != pMonster)
        {
            // ���� ü�� �޾ƿ���
            MonsterHp = pMonster->Get_MonsterHP();

            // ���� ü�� ���
            MonsterHp.Cur += pAttack;

            if (MonsterHp.Cur <= 0)
                MonsterHp.Cur = 0.f;
            if (MonsterHp.IsMax())
                MonsterHp.Cur = MonsterHp.Max;

            pMonster->Set_MonsterHP(MonsterHp); // ����� ���� ������ ü������ �־���
            pMonster->Set_Player_AttackState(_AttackState); // ���ݹ��� ������ �־���
        }
    }
}


#pragma endregion