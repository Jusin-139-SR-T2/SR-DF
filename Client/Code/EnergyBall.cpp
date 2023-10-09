#include "stdafx.h"
#include "EnergyBall.h"

CEnergyBall::CEnergyBall(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev)
{
}

CEnergyBall::CEnergyBall(const CEnergyBall& rhs)
	: CGameObject(rhs)
{
}

CEnergyBall::~CEnergyBall()
{
}

CEnergyBall* CEnergyBall::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z, MonsterPhase _CurrPhase)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("LightBeam Create Failed");
		return nullptr;
	}

	pInstance->m_pTransformComp->Set_Pos(_x, _y, _z);
	pInstance->m_vOrigin = { _x, _y, _z };
	pInstance->m_eCurrPhase = _CurrPhase;

	return pInstance;
}

HRESULT CEnergyBall::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	srand((_uint)time(NULL));

	// �浹��
	m_pTransformComp->Readjust_Transform();
	m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // �浹 �ҷ����°� 
	pShape = dynamic_cast<FCollisionSphere*>(m_pColliderComp->Get_Shape());
	pShape->fRadius = 0.5f;

	// �̹��� 
	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"SlowThunder");

	// ������ �� ����ð� ����
	m_fFrame = 0;
	m_fFrameSpeed = 8.f;
	m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
	m_fFrameCnt = 0.f;

	// ũ������
	m_pTransformComp->Set_Scale({ 2.f, 2.f, 1.f });

	return S_OK;
}

_int CEnergyBall::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	m_fFrame += m_fFrameSpeed * fTimeDelta;

	if (m_fFrame > m_fFrameEnd)
	{
		m_fFrame = 0;

	}

	if (3 == m_fFrameCnt)
		Set_Dead();

	Billboard();

	//���� ������Ʈ �ڵ�
	m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // �ݶ��̴� ��ġ ������Ʈ 

	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

	return S_OK;
}

#pragma region �⺻���� 

void CEnergyBall::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CEnergyBall::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	m_pTextureComp->Render_Texture(_ulong(m_fFrame));
	m_pBufferComp->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CEnergyBall::Add_Component()
{
	NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_Effect_BeamTextureComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);

	// �ݶ��̴� ������Ʈ
	NULL_CHECK_RETURN(m_pColliderComp = Set_DefaultComponent_FromProto<CColliderComponent>(ID_DYNAMIC, L"Com_Collider", L"Proto_ColliderSphereComp"), E_FAIL);

	// ���� ���� ���
	m_pColliderComp->EnterToPhysics(0);

	// �浹 �Լ� ����
	m_pColliderComp->Set_Collision_Event<ThisClass>(this, &ThisClass::OnCollision);
	m_pColliderComp->Set_CollisionEntered_Event<ThisClass>(this, &ThisClass::OnCollisionEntered);
	m_pColliderComp->Set_CollisionExited_Event<ThisClass>(this, &ThisClass::OnCollisionExited);

	// �浹 ���̾�, ����ũ ����
	m_pColliderComp->Set_CollisionLayer(LAYER_PROJECTILE); // �� Ŭ������ ���� �浹���̾� 
	m_pColliderComp->Set_CollisionMask(LAYER_PLAYER | LAYER_MONSTER | LAYER_WALL); // ��� �浹�ؾ��ϴ� ���̾�� 

	return S_OK;
}

void CEnergyBall::Free()
{
	SUPER::Free();
}

HRESULT CEnergyBall::Billboard()
{
	// ���Ͱ� �÷��̾� �ٶ󺸴� ���� 
	CTransformComponent* m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
	NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

	_vec3 vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();

	D3DXVec3Normalize(&vDir, &vDir);

	_float rad = atan2f(vDir.x, vDir.z);

	m_pTransformComp->Set_RotationY(rad);

	return S_OK;
}

#pragma endregion

#pragma region �浹 
void CEnergyBall::OnCollision(CGameObject* pDst)
{
}

void CEnergyBall::OnCollisionEntered(CGameObject* pDst)
{
	OutputDebugString(L"��EnergyBall �浹 \n");/*
	m_bCollision = true;
	m_bOld = TRUE;
	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"FallingStone");
	m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());*/
}

void CEnergyBall::OnCollisionExited(CGameObject* pDst)
{
}
#pragma endregion
void CEnergyBall::Follow_Player(const _float fTimeDelta)
{
}
