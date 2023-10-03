#include "stdafx.h"
#include "FireWork.h"

CFireWork::CFireWork(LPDIRECT3DDEVICE9 pGraphicDev)
	:CPsystem(pGraphicDev)
{
}

CFireWork::~CFireWork()
{
}

HRESULT CFireWork::Ready_GameObject(_vec3 vOriginPos, _int numParticles)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	BoundingBox boundingBox; 
	boundingBox.vMin = _vec3(-50.0f, -50.0f, -50.0f);
	boundingBox.vMax = _vec3(150.0f, 50.0f, 150.0f);

	m_vOrigin = vOriginPos;			// �ý��� ������ ��ƼŬ�� ���۵Ǵ� ��.
	m_fSize = 1.f;					// �ý��� �� ��� ��ƼŬ�� ũ��
	m_dSize = 4096;					// ���ؽ� ���۰� ������ �� �ִ� ��ƼŬ�� ��- ���� ��ƼŬ �ý��� ���� ��ƼŬ ���ʹ� ������.
	m_dOffset = 0;					// ���ؽ� ���ۿ��� ���縦 ������ ��ƼŬ �� ���� �ܰ���� ������(����Ʈ�� �ƴ� ��ƼŬ ����)
	m_dBatchSize = 512;
	m_BoundingBox = boundingBox;

	for (int i = 0; i < numParticles; i++)
		AddParticle();

	//_tchar* pPath = L"./Resource/Texture/effect/1.png";
	//_tchar* pPath = L"./Resource/Texture/Particle/flare2.bmp";
	//_tchar* pPath = L"./Resource/Texture/Particle/snowball.bmp";
	_tchar* pPath = L"./Resource/Texture/Particle/heart.bmp";

	CPsystem::Ready_GameObject(pPath);

	return S_OK;
}

CFireWork* CFireWork::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vOriginPos, _int numParticles)
{
	CFireWork* pInstance = new CFireWork(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(vOriginPos, numParticles)))
	{
		Safe_Release(pInstance);

		MSG_BOX("Particle FireWork Create Failed");
		return nullptr;
	}

	return pInstance;
}

_int CFireWork::Update_GameObject(const _float& fTimeDelta)
{
	m_fTime += 1.f * fTimeDelta;

	for (auto& iter : m_ParticleList)
	{
		if (iter.bIsAlive)
		{
			iter.vPosition += iter.vVelocity * fTimeDelta;

			iter.fAge += fTimeDelta;

			if (iter.fAge > iter.fLifeTime)
				iter.bIsAlive = false;

			// �ٿ�� �ڽ� ���� 
			if (m_BoundingBox.vMax.x < iter.vPosition.x || m_BoundingBox.vMin.x > iter.vPosition.x ||
				m_BoundingBox.vMax.y < iter.vPosition.y || m_BoundingBox.vMin.y > iter.vPosition.y ||
				m_BoundingBox.vMax.z < iter.vPosition.z || m_BoundingBox.vMin.z > iter.vPosition.z)
			{
				// ��Ȱ��
				ResetParticle(&iter);
			}

			if (Engine::IsKey_Pressed(DIK_X))
			{
				m_fDieTime = 100.f;
				ResetParticle(&iter);
			}
		}
	}



	billboard();

	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

	return S_OK;
}

void CFireWork::LateUpdate_GameObject(void)
{
	SUPER::LateUpdate_GameObject();
}

void CFireWork::Render_GameObject()
{
	CPsystem::Render_GameObject();
}

void CFireWork::ResetParticle(Attribute* _attribute)
{
	// �ý��� ��õ(�Ҳ��� ������ ���) ������ ������, �ý����� ���� ��ƼŬ�� ��.
	// �ý��� ��õ�� ��ƼŬ�� �ʱ�ȭ�ϰ� ��ü ������ ������ �ӵ��� �����,
	// �ý��� ���� ��ƼŬ���� ������ �÷��� �ο�.
	// �� ��ƼŬ�� ������ ����.
	_attribute->bIsAlive = true;

	_attribute->vPosition = m_vOrigin;
	_vec3 vMin = _vec3(-1.0f, -1.0f, -1.f);
	_vec3 vMax = _vec3(1.0f, 1.0f, 1.f);

	Get_RandomVector(&_attribute->vVelocity, &vMin, &vMax);

	// ��ü�� ����� ���� ����ȭ
	D3DXVec3Normalize( &_attribute->vVelocity, &_attribute->vVelocity);

	_attribute->vVelocity *= 10.0f;

	_attribute->Color = D3DXCOLOR(	1.0f, // R
									Get_RandomFloat(0.0f, 1.0f), // G
									Get_RandomFloat(0.0f, 1.0f), // B
									1.0f); // A
	//Get_RandomFloat(0.0f, 1.0f) rgb
	_attribute->fAge = 0.0f;
	_attribute->fLifeTime = 2.0f; //2���� ������ ������. 

	//m_fDieTime = Get_RandomFloat(3.f, 10.f);
	//m_fMoveTime = Get_RandomFloat(.1f, .5f);
}

HRESULT CFireWork::Add_Component()
{
	NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);

	return S_OK;
}

void CFireWork::billboard()
{
	_matrix		matWorld, matView, matBill;

	matWorld = *m_pTransformComp->Get_Transform();

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixIdentity(&matBill);

	matBill._11 = matView._11;
	matBill._13 = matView._13;
	matBill._31 = matView._31;
	matBill._33 = matView._33;

	D3DXMatrixInverse(&matBill, 0, &matBill);

	m_pTransformComp->Set_WorldMatrixS(&(matBill * matWorld));
}

void CFireWork::Free()
{
	SUPER::Free();
}