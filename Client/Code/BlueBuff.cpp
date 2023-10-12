#include "stdafx.h"
#include "BlueBuff.h"

CBlueBuff::CBlueBuff(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CBlueBuff::CBlueBuff(const CBlueBuff& rhs)
	: Base(rhs)
{
}

CBlueBuff::~CBlueBuff()
{
}

CBlueBuff* CBlueBuff::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z, MonsterPhase _CurrPhase, CGameObject* pOwner)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("BlueBuff Create Failed");
		return nullptr;
	}

	pInstance->m_pTransformComp->Set_Pos(_x, _y, _z);
	pInstance->Set_Owner(pOwner);

	return pInstance;
}

HRESULT CBlueBuff::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	// �̹��� 
	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"Buff");

	// ������ �� ����ð� ����
	m_tFrame.fFrame = 0;
	m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
	m_tFrame.fFrameSpeed = 8.f;

	// ũ������
	m_pTransformComp->Set_Scale({ 2.f, 2.f, 1.f });

	return S_OK;
}

_int CBlueBuff::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	Update_PlayerPos();

	CTransformComponent* m_pBossTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Boss", L"Com_Transform"));
	m_pTransformComp->Set_Pos(m_pBossTransformcomp->Get_Pos());

	m_tFrame.fFrame += fTimeDelta * m_tFrame.fFrameSpeed;

	if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
	{
		m_tFrame.fFrame = 0;
	}

	if (m_pOwner->Get_IsDead()) // ������ ������ ������ ����� 
		Set_Dead();

	Billboard();

	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

	return S_OK;
}

#pragma region �⺻���� 

void CBlueBuff::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CBlueBuff::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	m_pTextureComp->Render_Texture(_ulong(m_tFrame.fFrame));
	m_pBufferComp->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CBlueBuff::Add_Component()
{
	NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_Effect_BeamTextureComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);

	//��� �浹 ���� 

	return S_OK;
}

void CBlueBuff::Free()
{
	SUPER::Free();
}

#pragma endregion