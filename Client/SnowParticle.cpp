#include "stdafx.h"
#include "SnowParticle.h"

//CSnowParticle::CSnowParticle(LPDIRECT3DDEVICE9 pGraphicDev)
//	:CPsystem(pGraphicDev)
//{
//}
//
//CSnowParticle::~CSnowParticle()
//{
//}
//
//HRESULT CSnowParticle::Ready_GameObject(_vec3 vOriginPos, _int numParticles)
//{
//	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
//
//	srand(_ulong(time(NULL)));
//	BoundingBox boundingBox;
//	boundingBox.vMin = _vec3(-100.0f, -100.0f, -100.0f);
//	boundingBox.vMin = _vec3(100.0f, 100.0f, 100.0f);
//
//	m_vOrigin = vOriginPos;
//	m_fSize = 0.1f;
//	m_vbSize = 2048;  // ���ؽ� ���۰� ������ �� �ִ� ��ƼŬ�� ��- ���� ��ƼŬ �ý��� ���� ��ƼŬ ���ʹ� ������.
//	m_vbOffset = 0; // ���� ������
//	m_vbBatchSize = 512;
//	m_BoundingBox = boundingBox;  
//
//	for (int i = 0; i < numParticles; i++)
//		AddParticle();
//
//	_tchar* pPath = L"./Resource/Texture/Particle/snowball.bmp";
//
//	CPsystem::Ready_GameObject(pPath);
//
//	return S_OK;
//}
//
//_int CSnowParticle::Update_GameObject(const _float& fTimeDelta)
//{
//	m_fTime += 1.f * fTimeDelta;
//
//	for (auto& iter : m_ParticleList)
//	{
//		if (iter.bIsAlive)
//		{
//			if (m_fTime < m_fMoveTime)
//				iter.vPosition += iter.vVelocity * fTimeDelta;
//			else
//				iter.vPosition += (iter.vVelocity / 2) * fTimeDelta;
//
//			iter.fAge += fTimeDelta;
//
//			if (iter.fAge > iter.fLifeTime)
//				iter.bIsAlive = false;
//
//			// ��� ������ ����°�?
//			if (m_BoundingBox.vMax.x < iter.vPosition.x || m_BoundingBox.vMin.x > iter.vPosition.x ||
//				m_BoundingBox.vMax.y < iter.vPosition.y || m_BoundingBox.vMin.y > iter.vPosition.y ||
//				m_BoundingBox.vMax.z < iter.vPosition.z || m_BoundingBox.vMin.z > iter.vPosition.z)
//			{
//				// ��Ȱ��
//				ResetParticle(&iter);
//			}
//		}
//	}
//
//	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);
//
//	return 0;
//}
//
//void CSnowParticle::LateUpdate_GameObject(void)
//{
//	SUPER::LateUpdate_GameObject();
//}
//
//void CSnowParticle::Render_GameObject()
//{
//	m_pGraphicDev->SetRenderState(D3DRS_FOGENABLE, FALSE);
//
//	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
//	
//	CPsystem::Render_GameObject();
//
//	m_pGraphicDev->SetRenderState(D3DRS_FOGENABLE, TRUE);
//}
//
//void CSnowParticle::ResetParticle(Attribute* _attribute)
//{
//	_attribute->bIsAlive = true;
//
//	// GetRandomVector - �ּ��� min�� �ִ��� max�� ���ǵ� ���� ���� ���� ���͸� ���.
//	// �������� ��ġ ������ ���� ������ x, z ��ǥ�� ��´�.
//	Get_RandomVector(&_attribute->vPosition, &m_BoundingBox.vMin, &m_BoundingBox.vMax);
//
//	// ���� (y��ǥ) �� �׻� ��� ������ �ֻ��.
//	_attribute->vPosition.y = m_BoundingBox.vMax.y;
//
//	// GetRandowFloat - [ lowBound, highBound ] ������ �ִ� ������ float�� ����.
//	 // �����̴� �Ʒ������� �������� �ణ ������ ���Ѵ�.
//	_attribute->vVelocity.x = Get_RandomFloat(0.0f, 1.0f) * -3.0f;
//	_attribute->vVelocity.y = Get_RandomFloat(0.0f, 1.0f) * -10.0f;
//	_attribute->vVelocity.z = 0.0f;
//
//	// ����� ������
//	_attribute->Color = DXCOLOR_WHITE;
//}
//
//HRESULT CSnowParticle::Add_Component()
//{
//	NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
//
//	return S_OK;
//}
//
//CSnowParticle* CSnowParticle::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vOriginPos, int numParticles)
//{
//	CSnowParticle* pInstance = new CSnowParticle(pGraphicDev);
//
//	if (FAILED(pInstance->Ready_GameObject(vOriginPos, numParticles)))
//	{
//		Safe_Release(pInstance);
//
//		MSG_BOX("Particle Snow Create Failed");
//		return nullptr;
//	}
//
//	return pInstance;
//}
//
//void CSnowParticle::Free()
//{
//}
