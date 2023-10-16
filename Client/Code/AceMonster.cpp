#include "stdafx.h"
#include "AceMonster.h"

CAceMonster::CAceMonster(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CAceMonster::CAceMonster(const CAceMonster& rhs)
    : Base(rhs)
{
}

CAceMonster::~CAceMonster()
{
}

HRESULT CAceMonster::Ready_GameObject()
{
	srand((_uint)time(NULL));

    NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculatorComponent>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_MonsterTextureComp"), E_FAIL);

    return S_OK;
}

_int CAceMonster::Update_GameObject(const _float& fTimeDelta)
{
	Get_PlayerPos();

    SUPER::Update_GameObject(fTimeDelta);

    return S_OK;
}

void CAceMonster::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CAceMonster::Free()
{
    SUPER::Free();
}

#pragma region 공통셋팅 파트 

_float CAceMonster::Calc_Distance()
{
    // 몬스터가 플레이어 바라보는 벡터 -> 길이 계산용  
    _float fDistance = D3DXVec3Length(&(m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos()));

    return fDistance;
}

_bool CAceMonster::Detect_Player()
{
	_vec3 vMonsterLook;

	m_pTransformComp->Get_Info(INFO_LOOK, &vMonsterLook);

	//몬스터가 플레이어 바라보는 벡터 
	_vec3 MonToPlayer = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();
	_float currdistance = D3DXVec3Length(&MonToPlayer);

	//정규화
	D3DXVec3Normalize(&MonToPlayer, &MonToPlayer);

	// 시야범위 우선체크 
	if (currdistance > m_tStat.fAttackDistance)
		return false;

	// 내적하여 RADIAN구하기 
	_float fradian = acos(D3DXVec3Dot(&vMonsterLook, &MonToPlayer)) * 180 / D3DX_PI;

	// 시야범위 + 시야각도 = 포착 
	if (fradian < m_tStat.fAttackDistance * 2 && currdistance < m_tStat.fAttackFov)
		return true;
	else
		return false;
}

void CAceMonster::Billboard(const _float& fTimeDelta)
{
	// 몬스터가 플레이어 바라보는 벡터 
	_vec3 vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();

	D3DXVec3Normalize(&vDir, &vDir);

	_float rad = atan2f(vDir.x, vDir.z);

	m_pTransformComp->Set_RotationY(rad);
}

void CAceMonster::Height_On_Terrain()
{
	_vec3		vPos;
	m_pTransformComp->Get_Info(INFO_POS, &vPos);

	CTerrainBufferComp* pTerrainBufferComp = dynamic_cast<CTerrainBufferComp*>(Engine::Get_Component(ID_STATIC, L"Terrain", L"Terrain", L"Com_Buffer"));
	NULL_CHECK(pTerrainBufferComp);

	_float	fHeight = m_pCalculatorComp->Compute_HeightOnTerrain(&vPos,
		pTerrainBufferComp->Get_VtxPos(),
		pTerrainBufferComp->Get_VertexCountX() + 1U,
		pTerrainBufferComp->Get_VertexCountZ() + 1U,
		pTerrainBufferComp->Get_Scale(),
		pTerrainBufferComp->Get_InvOffset());

	m_pTransformComp->Set_Pos(vPos.x, fHeight + 1.3f, vPos.z);
}

HRESULT CAceMonster::Get_PlayerPos()
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"GameLogic", L"Player"));
	ePlayerRighthand = pPlayer->Get_PlayerRightHand();
	m_bPlayerAttakBool = pPlayer->Get_PlayerAttackBool();
	m_vPos = m_pTransformComp->Get_Pos();

	m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
	NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

	m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);

	return S_OK;
}

#pragma endregion