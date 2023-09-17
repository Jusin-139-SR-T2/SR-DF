#include "stdafx.h"
#include "Player.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "DynamicCamera.h"
#include "Calculator.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
    : Base(rhs)
{
}

CPlayer::~CPlayer()
{
}

HRESULT CPlayer::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_pTransformComp->m_vScale.x = 0.3f;
    m_pTransformComp->m_vScale.y = 0.4f;

    m_pTransformComp->m_vInfo[INFO_POS] = { 15.f, 10.f, 10.f };

    // �ʱ� ���� ���� (���� ����)
    m_tState.Set_State(STATE_RIGHTHAND::IDLE);

    m_tState.Add_Func(STATE_RIGHTHAND::IDLE, &ThisClass::Idle);
    m_tState.Add_Func(STATE_RIGHTHAND::HAND, &ThisClass::HandAttack);
    m_tState.Add_Func(STATE_RIGHTHAND::GUN, &ThisClass::GunAttack);

    m_pBufferComp->Set_Vertex(1.7f, 0.f, 1.f);

    return S_OK;
}

_int CPlayer::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    // �÷��̾� ���콺 ����
    Mouse_Move();

    Height_On_Terrain();
    Key_Input(fTimeDelta);

    if (bFrameOn)
    {
        m_fFrame += 10.f * fTimeDelta;

        if (m_fFrame > m_fMaxFrame)
        {
            m_fFrame = 0;
            bFrameOn = false;

            if (bSpinOn)
            {
                bSpinOn = false;
                bGunOn = true;
                m_fMaxFrame = 3.f;
            }
        }
    }

    Engine::Add_RenderGroup(RENDER_ALPHA, this);

    return 0;
}

void CPlayer::LateUpdate_GameObject()
{   
    SUPER::LateUpdate_GameObject();
}

void CPlayer::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_WorldMatrix());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pBufferComp->Set_Vertex(-3.5f, -0.7f, 0.f);
    m_pLeftHandTextureComp->Render_Texture(0);
    m_pBufferComp->Render_Buffer();

    m_pBufferComp->Set_Vertex(3.5f, 0.f, 0.f);

    if (!bAttackOn)
    {
        m_pRightHandTextureComp->Render_Texture(0);
        m_pBufferComp->Render_Buffer();
    }
    if (bAttackOn)
    {
        if (bGunOn)
        {
            m_pAttackTextureComp->Render_Texture((_ulong)m_fFrame);
            m_pBufferComp->Render_Buffer();
        }
        else if (bSpinOn)
        {
            m_pAttackSpinTextureComp->Render_Texture((_ulong)m_fFrame);
            m_pBufferComp->Render_Buffer();
        }
    }


    m_pBufferComp->Set_Vertex(0.f, 0.7f, 0.f);

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CPlayer::Add_Component()
{
    NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcTex>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransform>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculator>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);

    NULL_CHECK_RETURN(m_pColliderComp = Set_DefaultComponent_FromProto<CSphereColComp>(ID_STATIC, L"Comp_SphereCollider", L"Proto_SphereColComp"), E_FAIL);

    // �÷��̾� �޼� �ؽ�ó
    NULL_CHECK_RETURN(m_pLeftHandTextureComp = Set_DefaultComponent_FromProto<CTexture>(ID_STATIC, L"Comp_TextureLeftHand", L"Proto_PlayerLeftHandTextureComp"), E_FAIL);
    // �÷��̾� ������ �ؽ�ó
    NULL_CHECK_RETURN(m_pRightHandTextureComp = Set_DefaultComponent_FromProto<CTexture>(ID_STATIC, L"Comp_TextureRightHand", L"Proto_PlayerRightHandTextureComp"), E_FAIL);
    // �÷��̾� ���� �ؽ�ó
    NULL_CHECK_RETURN(m_pAttackTextureComp = Set_DefaultComponent_FromProto<CTexture>(ID_STATIC, L"Com_TextureAttack", L"Proto_PlayerAttackTextureComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pAttackSpinTextureComp = Set_DefaultComponent_FromProto<CTexture>(ID_STATIC, L"Com_TextureAttackTest", L"Proto_PlayerAttackTestTextureComp"), E_FAIL);

    return S_OK;
}

void CPlayer::Key_Input(const _float& fTimeDelta)
{
    _vec3	vLook;

    m_pTransformComp->Get_Info(INFO_LOOK, &vLook);
    vLook = { vLook.x, 0.f, vLook.z };

    _vec3 vInverse = (-m_pCamera->Get_At());

    m_pTransformComp->Compute_LootAtTarget(&vInverse);

    // ����
    if (Engine::Get_DIKeyState(DIK_W) & 0x80)
    {
        D3DXVec3Normalize(&vLook, &vLook);
        m_pTransformComp->Move_Pos(&vLook, fTimeDelta, 5.f);
    }

    // ����
    if (Engine::Get_DIKeyState(DIK_S) & 0x80)
    {
        D3DXVec3Normalize(&vLook, &vLook);
        m_pTransformComp->Move_Pos(&vLook, fTimeDelta, -5.f);
    }

    // ������
    if (Engine::Get_DIKeyState(DIK_D) & 0x80)
    {
        m_pTransformComp->Rotation(ROT_Y, D3DXToRadian(90.f * fTimeDelta));
    }

    // ����
    if (Engine::Get_DIKeyState(DIK_A) & 0x80)
    {
        m_pTransformComp->Rotation(ROT_Y, D3DXToRadian(-90.f * fTimeDelta));
    }

    // ����
    if (Engine::Get_DIKeyState(DIK_Q) & 0x80)
    {
        bAttackOn = true;
        bGunOn = true;
        bSpinOn = false;
        m_fMaxFrame = 3.f;
    }

    if (Engine::Get_DIKeyState(DIK_E) & 0x80)
    {
        bAttackOn = true;
        bGunOn = true;
    }

    // ���� (����)
    if (Engine::Get_DIKeyState(DIK_R) & 0x80)
    {
        bFrameOn = true;
        bSpinOn = true;
        bGunOn = false;
        m_fMaxFrame = 4.f;
    }

    // ���콺 ��Ŭ��
    if (Engine::Get_DIMouseState(DIM_LB) & 0x80)
    {
        bFrameOn = true;
    }

    // ���콺 ��Ŭ��
    if (Engine::Get_DIMouseState(DIM_RB) & 0x80)
    {
        
    }
}

// ���콺 ������
void CPlayer::Mouse_Move()
{
    // ���콺�� �÷��̾� ȸ��
    // ��, ��
    if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_Y))
    {
        m_pTransformComp->Rotation(ROT_X, D3DXToRadian(dwMouseMove / 10.f));
    }
    // ��, ��
    if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_X))
    {
        m_pTransformComp->Rotation(ROT_Y, D3DXToRadian(dwMouseMove / 10.f));
    }
}

CPlayer* CPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
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

void CPlayer::Free()
{
    SUPER::Free();
}

void CPlayer::Height_On_Terrain()
{
    _vec3		vPos;
    m_pTransformComp->Get_Info(INFO_POS, &vPos);

    CTerrainTexComponent* pTerrainBufferComp = dynamic_cast<CTerrainTexComponent*>(Engine::Get_Component(ID_STATIC, L"Environment", L"Terrain", L"Com_Buffer"));
    NULL_CHECK(pTerrainBufferComp);

    _float	fHeight = m_pCalculatorComp->Compute_HeightOnTerrain(&vPos, pTerrainBufferComp->Get_VtxPos());
                 
    m_pTransformComp->Set_Pos(vPos.x, fHeight + 1.5f, vPos.z);
}

#pragma region �ൿ��
void CPlayer::Idle(float fTimeDelta)
{

}

// �⺻ ���� (�ָ�)
void CPlayer::HandAttack(float fTimeDelta)
{

}

// ����
void CPlayer::GunAttack(float fTimeDelta)
{

}
#pragma endregion