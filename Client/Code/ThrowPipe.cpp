#include "stdafx.h"
#include "ThrowPipe.h"

CThrowPipe::CThrowPipe(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CThrowPipe::CThrowPipe(const CThrowPipe& rhs)
	: Base(rhs)
{
}

CThrowPipe::~CThrowPipe()
{
}

CThrowPipe* CThrowPipe::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z, CGameObject* pOwner)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("ThrowPipe Create Failed");
		return nullptr;
	}

	// �����Ҷ� ���� ��ġ �� �����ϱ� ���� Create���� �ʱ���ġ�� ����� 
	pInstance->m_pTransformComp->Set_Pos(_x, _y, _z);
	pInstance->Set_Owner(pOwner);

	return pInstance;
}


HRESULT CThrowPipe::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	// �浹
	m_pTransformComp->Readjust_Transform();
	FCollisionSphere* pShape = dynamic_cast<FCollisionSphere*>(m_pColliderComp->Get_Shape());
	pShape->fRadius = 1.5f;

	// ����
	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Projectile", L"ThrowPipe");
	m_pTransformComp->Set_Scale({ 0.3f, 0.7f, 1.f });

	//���� �� ����
	m_tFrame.fFrame = 0;
	m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
	m_tFrame.fFrameSpeed = 10.f;

	m_fMoveSpeed = 7.f;
	_bLoop = false;
	
	return S_OK;
}

_int CThrowPipe::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	//Billboard(fTimeDelta);

	if (!_bLoop) // �ѹ� ���� �������θ� ���󰡼� �÷��̾ �� ���Ҽ��ֵ��� 
	{
		// �÷��̾� ������ ���ư������� ���⺤�� ��� 
		m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
		NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

		//���� : �繰�� �÷��̾ �ٶ󺸴� ���� 
		m_vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();
	
		D3DXVec3Normalize(&m_vDir, &m_vDir); //����ȭ �ؼ� ���ǵ�� ����

		_bLoop = true;
	}

	m_fFrame += m_fFrameSpeed * fTimeDelta;

	if (m_fFrame > m_fFrameEnd)
	{
		m_fFrame = 0.f;
	}

	m_pTransformComp->Move_Pos(&m_vDir, fTimeDelta, m_fMoveSpeed);
	
	m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // �浹 �ҷ����°� 

	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

	return S_OK;
}

void CThrowPipe::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CThrowPipe::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	m_pTextureComp->Render_Texture(_ulong(m_fFrame));
	m_pBufferComp->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CThrowPipe::Add_Component()
{
	NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_ProjectileTextureComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
	
	// -------------------- �浹 ��Ʈ --------------------------
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
	m_pColliderComp->Set_CollisionMask(LAYER_PLAYER); // ��� �浹�ؾ��ϴ� ���̾�� 

	return S_OK;
}

void CThrowPipe::Free()
{
	SUPER::Free();
}

void CThrowPipe::Billboard(const _float& fTimeDelta)
{// ���Ͱ� �÷��̾� �ٶ󺸴� ���� 
	_vec3 vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();

	D3DXVec3Normalize(&vDir, &vDir);

	_float rad = atan2f(vDir.x, vDir.z);

	m_pTransformComp->Set_RotationY(rad);

}

void CThrowPipe::OnCollision(CGameObject* pDst)
{


}

void CThrowPipe::OnCollisionEntered(CGameObject* pDst)
{
	OutputDebugString(L"Gray�� ����ü�� �浹���� \n");

	Set_Dead(); //����ü�� ����� 
}

void CThrowPipe::OnCollisionExited(CGameObject* pDst)
{
}
