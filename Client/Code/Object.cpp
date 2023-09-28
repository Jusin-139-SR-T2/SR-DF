#include "stdafx.h"
#include "Object.h"

CObject::CObject(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CObject::CObject(const CObject& rhs)
	: Base(rhs)
{
}

CObject::~CObject()
{
}

CObject* CObject::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("Object Create Failed");
		return nullptr;
	}

	return pInstance;
}

HRESULT CObject::Add_Component()
{
	NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculatorComponent>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_ObjectTextureComp"), E_FAIL);

	return S_OK;
}

HRESULT CObject::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);


	m_pTransformComp->Set_Pos({20.f, 1.f, 25.f});

	return S_OK;
}


_int CObject::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	// 기본함수 
	Height_On_Terrain();
	//Billboard();

	// ---------- 테스트 빌드 ------------------


	// ----------------------------------------


	Engine::Add_RenderGroup(RNEDER_ALPHATEST, this);
	return S_OK;
}

void CObject::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CObject::Render_GameObject()
{

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	//m_pTextureComp->Render_Texture(_ulong(m_fFrame)); // 오브젝트가 프레임쓸일 있으면 이걸로 
	m_pTextureComp->Render_Texture();
	m_pBufferComp->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}


void CObject::Free()
{
	SUPER::Free();
}

// 기본함수 ------------------------------------
void CObject::Height_On_Terrain()
{
	_vec3		vPos;
	m_pTransformComp->Get_Info(INFO_POS, &vPos);

	CTerrainBufferComp* pTerrainBufferComp = dynamic_cast<CTerrainBufferComp*>(Engine::Get_Component(ID_STATIC, L"Environment", L"Terrain", L"Com_Buffer"));
	NULL_CHECK(pTerrainBufferComp);

	_float	fHeight = m_pCalculatorComp->Compute_HeightOnTerrain(&vPos, pTerrainBufferComp->Get_VtxPos());

	m_pTransformComp->Set_Pos(vPos.x, fHeight + 0.5f, vPos.z);
}

void CObject::Billboard()
{ 
	////case1. 회전행렬 만들기 
	//_matrix		matWorld, matView, matBill, matScale, matChangeScale;

	//matWorld = *m_pTransformComp->Get_WorldMatrix();

	//m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);

	//_vec3 Pos = m_pTransformComp->m_vInfo[INFO_POS];

	//_vec3 vDir = vPlayerPos - m_pTransformComp->m_vInfo[INFO_POS];

	//D3DXVec3Normalize(&vDir, &vDir);

	//_float rad = atan2f(vDir.x, vDir.z);

	//// 회전행렬 생성
	//_matrix rotationMatrix;
	//D3DXMatrixRotationY(&rotationMatrix, rad);

	//m_pTransformComp->Set_WorldMatrixS(&(rotationMatrix * matWorld));

	 //case2. 빌보드 구성하기 
	//_matrix		matWorld, matView, matBill;

	//matWorld = *m_pTransformComp->Get_WorldMatrix();

	//m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	//D3DXMatrixIdentity(&matBill);

	//matBill._11 = matView._11;
	//matBill._13 = matView._13;
	//matBill._31 = matView._31;
	//matBill._33 = matView._33;

	//D3DXMatrixInverse(&matBill, 0, &matBill);

	//m_pTransformComp->Set_WorldMatrixS(&(matBill * matWorld));

	//m_pTransformComp->m_vScale.x = 0.4;
	//m_pTransformComp->m_vScale.y = 0.8;
}
// --------------------------------------------
