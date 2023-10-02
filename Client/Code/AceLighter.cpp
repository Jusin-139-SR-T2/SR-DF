#include "stdafx.h"
#include "AceLighter.h"

CAceLighter::CAceLighter(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CAceLighter::CAceLighter(const CAceLighter& rhs)
	: Base(rhs)
{
}

CAceLighter::~CAceLighter()
{
}

CAceLighter* CAceLighter::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);

        MSG_BOX("Lighter Create Failed");
        return nullptr;
    }

    return pInstance;
}

HRESULT CAceLighter::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	return S_OK;
}

_int CAceLighter::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    Height_On_Terrain();

    if (Engine::IsKey_Pressed(DIK_V))
    {
        m_bLightOn = !m_bLightOn;
    }

    Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

    return S_OK;
}

void CAceLighter::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CAceLighter::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
 
    if (m_bLightOn)
    {
        m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
        //m_pGraphicDev->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
        Create_Light();
    }
    else
    {
        m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
        // ����Ʈ�� ��Ȱ��ȭ
        m_pGraphicDev->LightEnable(2, FALSE); // ����Ʈ �ε��� 2�� ��Ȱ��ȭ

        // ����Ʈ ����
        //m_pGraphicDev->SetLight(2, nullptr); // ����Ʈ�� ����
    }
    
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE); // �޸����� 
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

    //m_pGraphicDev->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    if (m_bLightOn)
        m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
}

void CAceLighter::Height_On_Terrain()
{
    _vec3		vPos;
    m_pTransformComp->Get_Info(INFO_POS, &vPos);

    CTerrainBufferComp* pTerrainBufferComp = dynamic_cast<CTerrainBufferComp*>(Engine::Get_Component(ID_STATIC, L"Environment", L"Terrain", L"Com_Buffer"));
    NULL_CHECK(pTerrainBufferComp);

    _float	fHeight = m_pCalculatorComp->Compute_HeightOnTerrain(&vPos, pTerrainBufferComp->Get_VtxPos());

    m_pTransformComp->Set_Pos(vPos.x, fHeight + 1.f, vPos.z);
}

HRESULT CAceLighter::Add_Component()
{
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculatorComponent>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);
    
    return S_OK;
}

void CAceLighter::Free()
{
    SUPER::Free();
}

HRESULT CAceLighter::Create_Light()
{
    m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
    NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

    ZeroMemory(&AceLight, sizeof(D3DLIGHT9)); // �ʱ�ȭ 

    // �� �������ֱ� 
    AceLight.Type = D3DLIGHT_POINT; // ���������� ����
    AceLight.Position = m_pPlayerTransformcomp->Get_Pos();
    AceLight.Range = 10.f; // ������ �޴� ��ü�� ���� �� �Ÿ� 
    AceLight.Diffuse = D3DXCOLOR(DXCOLOR_DARK_ORANGE); // ������ �ݻ�� �� ��µǴ� ���� �ֵ� ��
    AceLight.Specular = D3DXCOLOR(DXCOLOR_BLACK); // Ư�� �������θ� �ݻ�Ǵ� �� (ī�޶� ���� �޶���)
    AceLight.Ambient = D3DXCOLOR(DXCOLOR_DARK_ORANGE); // ������ ��ġ�� �����ϰ� �Ȱ��� ������ ��� ������ �ݻ�Ǵ� ��
    // POINT ������ ������ ������ ����(DIRECTION) ���� 

    // ���� - �Ÿ��� ���� ���� ���谪
    AceLight.Attenuation0 = 0.4f;
    AceLight.Attenuation1 = 0.4f;
    AceLight.Attenuation2 = 0.0f; // 2�� ���谪 

    // ���� ���
    FAILED_CHECK_RETURN(Engine::Ready_Light(m_pGraphicDev, &AceLight, 2), E_FAIL);

    //���� ����
    D3DMATERIAL9			SpotMtrl;
    ZeroMemory(&SpotMtrl, sizeof(D3DMATERIAL9));

    SpotMtrl.Diffuse = D3DXCOLOR(DXCOLOR_BLACK); //���ݻ� - ��ü�� ���� ���� ��������
    SpotMtrl.Specular = D3DXCOLOR(DXCOLOR_BLACK); //V���ݻ� - ���̶���Ʈ ǥ��
    SpotMtrl.Ambient = D3DXCOLOR(DXCOLOR_BLACK);
    SpotMtrl.Emissive = { 0.f, 0.f, 0.f, 0.f }; //��ü �߱� 
    SpotMtrl.Power = 10.f;

    m_pGraphicDev->SetMaterial(&SpotMtrl);

    return S_OK;
}

