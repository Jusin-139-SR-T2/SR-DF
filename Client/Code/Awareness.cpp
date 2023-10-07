#include "stdafx.h"
#include "Awareness.h"

CAwareness::CAwareness(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev)
{
}

CAwareness::CAwareness(const CAwareness& rhs)
	: CGameObject(rhs)
{
}

CAwareness::~CAwareness()
{
}

HRESULT CAwareness::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"Awareness");

	//m_pTransformComp->Set_Pos(15.f, 2.f, 15.f);
	m_pTransformComp->Set_Scale({ 0.5f, 0.5f, 1.f });

	m_fFrame = 0;
	m_fFrameSpeed = 2.f;


	m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());

	//m_pTransformComp->Set_Pos(m_pBossTransformcomp->Get_Pos());
	return S_OK;
}

_int CAwareness::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	m_fFrame += m_fFrameSpeed * fTimeDelta;

	if (m_fFrame >= m_fFrameEnd)
	{
		m_fFrame = m_fFrameEnd - 1.f;
	}

	Billboard();

	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

	return S_OK;
}

void CAwareness::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CAwareness::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	m_pTextureComp->Render_Texture(_ulong(m_fFrame));
	m_pBufferComp->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

CAwareness* CAwareness::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("LightBeam Create Failed");
		return nullptr;
	}

	// 생성할때 몬스터 위치 로 생성하기 위해 Create에서 초기위치를 잡아줌 
	pInstance->m_pTransformComp->Set_Pos(_x, _y, _z);

	return pInstance;
}

HRESULT CAwareness::Add_Component()
{
	NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_EffectTextureComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);

	return S_OK;
}

void CAwareness::Free()
{
	SUPER::Free();
}

HRESULT CAwareness::Billboard()
{
	_matrix		matWorld;

	matWorld = *m_pTransformComp->Get_Transform();
	
	CTransformComponent* m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));

	NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

	m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);

	_vec3 Pos = m_pTransformComp->Get_Pos();

	_vec3 vDir = vPlayerPos - m_pTransformComp->Get_Pos();

	D3DXVec3Normalize(&vDir, &vDir);

	_float rad = atan2f(vDir.x, vDir.z);

	// 회전행렬 생성
	_matrix rotationMatrix;
	D3DXMatrixRotationY(&rotationMatrix, rad);

	m_pTransformComp->Set_WorldMatrixS(&(rotationMatrix * matWorld));
}
