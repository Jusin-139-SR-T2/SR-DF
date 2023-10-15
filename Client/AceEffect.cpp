#include "stdafx.h"
#include "AceEffect.h"

CAceEffect::CAceEffect(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CAceEffect::CAceEffect(const CAceEffect& rhs)
	: Base(rhs)
{
}

CAceEffect::~CAceEffect()
{
}

HRESULT CAceEffect::Ready_GameObject()
{
	NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_MonsterTextureComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);

	return S_OK;
}

HRESULT CAceEffect::Add_Component()
{
	return S_OK;
}

_int CAceEffect::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	return 0;
}

void CAceEffect::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CAceEffect::Render_GameObject()
{
}

void CAceEffect::Free()
{
	SUPER::Free();
}

#pragma region 기본셋팅 

HRESULT CAceEffect::Billboard()
{
	m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
	NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

	_vec3 vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();
	D3DXVec3Normalize(&vDir, &vDir);

	_float rad = atan2f(vDir.x, vDir.z);
	m_pTransformComp->Set_RotationY(rad - D3DX_PI);

	return S_OK;
}

void CAceEffect::Height_On_Terrain(_float Height)
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
										 pTerrainBufferComp->Get_InvOffset() );

	m_pTransformComp->Set_Pos(vPos.x, fHeight + 1.f, vPos.z);
}

#pragma endregion

