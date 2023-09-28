#include "stdafx.h"
#include "Terrain.h"

#include "Export_System.h"
#include "Export_Utility.h"

using namespace Engine;

CTerrain::CTerrain(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CTerrain::CTerrain(const CTerrain& rhs)
    : Base(rhs)
{
}

CTerrain::~CTerrain()
{
}

CTerrain* CTerrain::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);

        MSG_BOX("Player Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CTerrain::Free()
{
    SUPER::Free();
}


HRESULT CTerrain::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_pTransformComp->Set_ScaleX(1.f);

    return S_OK;
}

_int CTerrain::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    Engine::Add_RenderGroup(RENDER_NONALPHA, this);

    return 0;
}

void CTerrain::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CTerrain::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    SetUp_Material();
    m_pTextureComp->Render_Texture(0);
    m_pBufferComp->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
   m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
}

HRESULT CTerrain::Add_Component()
{
    NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CTerrainBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_TerrainBufferComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_TerrainTextureComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);

#pragma region ������Ʈ
    //CComponent* pComponent = nullptr;

    // ���� ������Ʈ
    /*pComponent = m_pBufferCom = dynamic_cast<CTerrainBufferComp*>(Engine::Clone_Proto(L"Proto_TerrainComp"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });*/
    //NULL_CHECK_RETURN(m_pBufferCom = Set_DefaultComponent_FromProto<CTerrainBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_TerrainComp"));

    //// �ؽ�ó ������Ʈ
    //pComponent = m_pTextureCom = dynamic_cast<CTextureComponent*>(Engine::Clone_Proto(L"Proto_TerrainTexture"));
    //NULL_CHECK_RETURN(pComponent, E_FAIL);
    //m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });
    //NULL_CHECK_RETURN(m_pTextureCom = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_TerrainTexture"));

    //// Ʈ������ ������Ʈ
    //pComponent = m_pTransformCom = dynamic_cast<CTransformComponent*>(Engine::Clone_Proto(L"Proto_Transform"));
    //NULL_CHECK_RETURN(pComponent, E_FAIL);
    //m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });
    //NULL_CHECK_RETURN(m_pTransformCom = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"));
#pragma endregion

    return S_OK;
}

HRESULT CTerrain::SetUp_Material()
{
    D3DMATERIAL9			tMtrl;
    ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

    //ambient + diffuse*intensity +specular

    tMtrl.Diffuse = { 1.f, 1.f, 1.f, 1.f }; //���ݻ� - ��ü�� ���� ���� ��������
    //tMtrl.Diffuse = { 0.5f, 0.5f, 0.1f, 0.1f };
    tMtrl.Specular = { 1.f, 1.f, 1.f, 1.f }; //���ݻ� - ���̶���Ʈ ǥ�� 
    //tMtrl.Specular = { 0.1f, 0.1f, 0.1f, 0.1f }; //���ݻ�
    tMtrl.Ambient = { 0.1f, 0.1f, 0.1f, 1.f }; //�������� - ���̾����� �̰� �����Ѵٴ� �����Ͽ� ���� ���� 
    tMtrl.Emissive = { 0.f, 0.f, 0.f, 0.f }; //��ü �߱� 
    tMtrl.Power = 0.f;

    m_pGraphicDev->SetMaterial(&tMtrl);

    return S_OK;
}
