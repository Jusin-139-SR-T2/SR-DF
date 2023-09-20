#include "stdafx.h"
#include "Player.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "DynamicCamera.h"
#include "CalculatorComponent.h"

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


#pragma region �ʱ� ���� ���� (���� ����)
    // �÷��̾�
    m_tPlayer_State.Set_State(STATE_PLAYER::IDLE);
    // �޼�
    m_tLeftHand_State.Set_State(STATE_LEFTHAND::HAND);
    // ������
    m_tRightHand_State.Set_State(STATE_RIGHTHAND::HAND);;
#pragma endregion


#pragma region �÷��̾��� ���� �߰�
    m_tPlayer_State.Add_Func(STATE_PLAYER::IDLE, &ThisClass::Idle);
    m_tPlayer_State.Add_Func(STATE_PLAYER::MOVE, &ThisClass::Move);
    m_tPlayer_State.Add_Func(STATE_PLAYER::RUN, &ThisClass::Run);
    m_tPlayer_State.Add_Func(STATE_PLAYER::DOWN, &ThisClass::Down);
    m_tPlayer_State.Add_Func(STATE_PLAYER::ATTACK, &ThisClass::Attack);
    m_tPlayer_State.Add_Func(STATE_PLAYER::DIE, &ThisClass::Die);
#pragma endregion

#pragma region �÷��̾��� �޼� ���� �߰�
    m_tLeftHand_State.Add_Func(STATE_LEFTHAND::HAND, &ThisClass::Left_Hand);
    m_tLeftHand_State.Add_Func(STATE_LEFTHAND::RIGHTER, &ThisClass::Left_Righter);
#pragma endregion

#pragma region �÷��̾��� ������ ���� �߰�
    m_tRightHand_State.Add_Func(STATE_RIGHTHAND::HAND, &ThisClass::Right_Hand);
    m_tRightHand_State.Add_Func(STATE_RIGHTHAND::GUN, &ThisClass::Right_Gun);
    m_tRightHand_State.Add_Func(STATE_RIGHTHAND::STEELPIPE, &ThisClass::Right_Steelpipe);
#pragma endregion

    //Tset
    m_eObjectType = OBJECT_TYPE::NONE;

    m_pBufferComp->Set_Vertex(1.7f, 0.f, 1.f);

    return S_OK;
}

_int CPlayer::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    // ���콺 ������
    Mouse_Move();
    // ���콺 �Է�
    Attack_Input(fTimeDelta);
    // Ű���� �Է�
    Keyboard_Input(fTimeDelta);

    // ���� ������Ʈ(üũ)
    State_Update(fTimeDelta);

    // ���� Ÿ��
    Height_On_Terrain();

    // �޼� ������On
    if (bLeftFrameOn)
    {
        // ���� �������� �ð�(������)���� ������Ű��
        m_fLeftFrame += 10.f * fTimeDelta;

        // ���� �������� �ִ� �����ӿ� ������ ���
        if (m_fLeftFrame >= m_fMaxFrame)
        {
            // ���� �������� 0���� �ʱ�ȭ
            m_fLeftFrame = 0;

            // �޼� ������ Off
            bLeftFrameOn = false;

            // ���� �ִ��������ε� �����Ͱ� �������� ���
            if (bRighter)
            {
                // (���� ������) �����͸� �����ִ� �̹����� ����
                m_fLeftFrame = 5.f;
            }

            // ������ ȸ�����̿��� ��
            if (bSpinOn)
            {
                // ȸ�� Off
                bSpinOn = false;

                // �������� �ٽ� ���ư���
                bGunOn = true;

                // �ִ� �������� ���� �������� �ٽ� �������
                m_fMaxFrame = 3.f;
            }
        }
    }

    // ������ ������
    if (bRightFrameOn)
    {
        m_fRightFrame += 10.f * fTimeDelta;

        if (m_fRightFrame >= m_fMaxFrame)
        {
            m_fRightFrame = 0;
            bRightFrameOn = false;

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
    // ���� �׽�Ʈ
    if (bTorch)
    {
        m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
        //SetUp_Material();
    }

    // ��� ����
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_WorldMatrix());
    // ���� ���� �ɼ�
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    // �޼� ��ġ ����
    m_pBufferComp->Set_Vertex(-3.5f, -0.9f, 0.f);
    m_pLeftHandComp->Render_Texture((_ulong)m_fLeftFrame);  // �޼� �ؽ�ó
    m_pBufferComp->Render_Buffer();             // �޼� ����
    
    // (�޼�)���� �� �ǵ����ֱ�
    m_pBufferComp->Set_Vertex(3.5f, 0.f, 0.f);

    // ������ ��ġ ����
    m_pBufferComp->Set_Vertex(0.2f, 0.f, 0.f);
    //if (m_fFrame < m_fMaxFrame) // ���� �������� Max�� �ƴ� ���
    {
        m_pRightHandComp->Render_Texture((_ulong)m_fRightFrame); // ������ �ؽ�ó
        m_pBufferComp->Render_Buffer();                            // ������ ����
    }
    // (������)���� �� �ǵ����ֱ�
    m_pBufferComp->Set_Vertex(-0.2f, 0.f, 0.f);

    // �ʱ� �׽�Ʈ ����
    //if (!bAttackOn)
    //{
    //    m_pRightHandTextureComp->Render_Texture(0);
    //    m_pBufferComp->Render_Buffer();
    //}
    //if (bAttackOn)
    //{
    //    if (bGunOn)
    //    {
    //        m_pAttackTextureComp->Render_Texture((_ulong)m_fFrame);
    //        m_pBufferComp->Render_Buffer();
    //    }
    //    else if (bSpinOn)
    //    {
    //        m_pAttackSpinTextureComp->Render_Texture((_ulong)m_fFrame);
    //        m_pBufferComp->Render_Buffer();
    //    }
    //}

    // ���� �׽�Ʈ
    if (bTorch)
        m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, false);

    m_pBufferComp->Set_Vertex(0.f, 0.9f, 0.f);

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CPlayer::Add_Component()
{
    NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculatorComponent>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);

    NULL_CHECK_RETURN(m_pColliderComp = Set_DefaultComponent_FromProto<CSphereColComp>(ID_STATIC, L"Comp_SphereCollider", L"Proto_SphereColComp"), E_FAIL);

    // �÷��̾� �޼� �ؽ�ó
    NULL_CHECK_RETURN(m_pLeftHandComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Comp_TextureLeft", L"Proto_PlayerLeftTextureComp"), E_FAIL);
    // �÷��̾� ������ �ؽ�ó
    NULL_CHECK_RETURN(m_pRightHandComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Comp_TextureRight", L"Proto_PlayerRightTextureComp"), E_FAIL);
    

    // Test (���� �� �̹���)
    // �޼� �̹���
    NULL_CHECK_RETURN(m_pLeftHandTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_TextureLeftHandTest", L"Proto_PlayerLeftHandTextureComp"), E_FAIL);
    // ������ �̹���
    NULL_CHECK_RETURN(m_pRightHandTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_TextureRightHandTest", L"Proto_PlayerRightHandTextureComp"), E_FAIL);
    // �������� �̹���
    NULL_CHECK_RETURN(m_pRightSteelPipeTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_TextureRightSteelPipeTest", L"Proto_PlayerRightSteelPipeTextureComp"), E_FAIL);
    // ������ �̹���
    NULL_CHECK_RETURN(m_pLeftZippoTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_TextureLeftZippoTest", L"Proto_PlayerLeftZippoTextureComp"), E_FAIL);
    // �÷��̾� ���� �̹���
    NULL_CHECK_RETURN(m_pAttackTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_TextureAttack", L"Proto_PlayerAttackTextureComp"), E_FAIL);
    // �÷��̾� �� ȸ�� �̹���
    NULL_CHECK_RETURN(m_pAttackSpinTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_TextureAttackTest", L"Proto_PlayerAttackTestTextureComp"), E_FAIL);
    
    // �ؽ�ó �Ŵ���
    m_pRightHandTextureComp->Receive_Texture(TEX_NORMAL, L"Player", L"Catch_Mafia");
    m_pRightHandTextureComp->Receive_Texture(TEX_NORMAL, L"Player", L"Broken_Mafia");

    return S_OK;
}

bool CPlayer::Keyboard_Input(const _float& fTimeDelta)
{
    _vec3	vLook;

    m_pTransformComp->Get_Info(INFO_LOOK, &vLook);
    vLook = { vLook.x, 0.f, vLook.z };

    _vec3 vInverse = (-m_pCamera->Get_At());

    m_pTransformComp->Compute_LootAtTarget(&vInverse);

    // �ٱ�
    if (Engine::Get_DIKeyState(DIK_LSHIFT) & 0x80)
    {
        fSpeed = 10.f;
    }

    // ����
    if (Engine::Get_DIKeyState(DIK_W) & 0x80)
    {
        D3DXVec3Normalize(&vLook, &vLook);
        m_pTransformComp->Move_Pos(&vLook, fTimeDelta, fSpeed);
        
        bMove = true;
        //return bMove;
    }

    // ����
    if (Engine::Get_DIKeyState(DIK_S) & 0x80)
    {
        D3DXVec3Normalize(&vLook, &vLook);
        m_pTransformComp->Move_Pos(&vLook, fTimeDelta, -fSpeed);

        bMove = true;
        //return bMove;
    }

    // ������
    if (Engine::Get_DIKeyState(DIK_D) & 0x80)
    {
        _vec3 vRight;
        m_pTransformComp->Get_Info(INFO_RIGHT, &vRight);
        D3DXVec3Normalize(&vRight, &vRight);
        m_pTransformComp->Move_Pos(&vRight, fTimeDelta, 5.f);

        bMove = true;
        //return bMove;
    }

    // ����
    if (Engine::Get_DIKeyState(DIK_A) & 0x80)
    {
        _vec3 vRight;
        m_pTransformComp->Get_Info(INFO_RIGHT, &vRight);
        D3DXVec3Normalize(&vRight, &vRight);
        m_pTransformComp->Move_Pos(&-vRight, fTimeDelta, 5.f);

        bMove = true;
        //return bMove;
    }

    // ����
    if (Engine::Get_DIKeyState(DIK_Q) & 0x80)
    {
        //bAttackOn = true;
        //bGunOn = true;
        //bSpinOn = false;
        m_fMaxFrame = 3.f;

        //Test
        m_eObjectType = OBJECT_TYPE::GUN;
    }

    // ��
    if (Engine::Get_DIKeyState(DIK_E) & 0x80)
    {
        //bAttackOn = true;
       // bGunOn = true;

        //Test
        m_eObjectType = OBJECT_TYPE::NONE;
    }

    // ���� (����)
    if (Engine::Get_DIKeyState(DIK_R) & 0x80)
    {
        bRightFrameOn = true;
        bSpinOn = true;
    }

    // ��������
    if (Engine::Get_DIKeyState(DIK_4) & 0x80)
    {
        //Test
        m_eObjectType = OBJECT_TYPE::STEELPIPE;
    }

    // ������ (���ͳ��� : �Ƹ��� Ű�� �ѹ��� ������ �ԷµǾ �׷���)
    if (Engine::Get_DIKeyState(DIK_V) & 0x80)
    {
        //Test
        if (!bRighter)  // �����Ͱ� �������� ���
        {
            bRighter = true;     // ������ ���ֱ�
            bLeftFrameOn = true; // ������ ���
        }
        else // �����Ͱ� �������� ���
        {
            bRighter = false; // �ӽ� �������� ���� �ذ�..
        }       
    }

    fSpeed = 5.f;

    return false;
}

bool CPlayer::Attack_Input(const _float& fTimeDelta)
{
    // ������
    //if (Engine::Get_DIKeyState(DIK_Q) & 0x80)
    //{

    //    bFootAttack = true;
    //    return bFootAttack;
    //}

    // ���콺 ��Ŭ��
    if (Engine::Get_DIMouseState(DIM_LB) & 0x80)
    {
        bRightFrameOn = true;

        bMouse_Button = true;
        return bMouse_Button;
    }

    // ���콺 ��Ŭ��
    if (Engine::Get_DIMouseState(DIM_RB) & 0x80)
    {

        bMouse_Button = true;
        return bMouse_Button;
    }

    return false;
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
    _vec3		vPos = m_pTransformComp->Get_Pos();

    CTerrainBufferComp* pTerrainBufferComp = dynamic_cast<CTerrainBufferComp*>(Engine::Get_Component(ID_STATIC, L"Environment", L"Terrain", L"Com_Buffer"));
    NULL_CHECK(pTerrainBufferComp);

    _float	fHeight = m_pCalculatorComp->Compute_HeightOnTerrain(&vPos, pTerrainBufferComp->Get_VtxPos());
                 
    m_pTransformComp->Set_Pos(vPos.x, fHeight + 1.5f, vPos.z);
}

// �� ���� Ȯ��
void CPlayer::Hand_Check()
{
    // �÷��̾ �ȶٰ����� ��
    if (m_tPlayer_State.Get_State() != STATE_PLAYER::RUN)
    {
        // �����͸� �� Ų ���
        if (!bRighter)
        {
            // �޼�  (��κ� NONE���� �ٲ�����)
            switch (m_eObjectType)
            {
                case CPlayer::OBJECT_TYPE::NONE:
                {
                    m_tLeftHand_State.Set_State(STATE_LEFTHAND::HAND);
                    break;
                }
                case CPlayer::OBJECT_TYPE::GUN:
                {
                    m_tLeftHand_State.Set_State(STATE_LEFTHAND::HAND);
                    break;
                }
                case CPlayer::OBJECT_TYPE::THOMPSON:
                {
                    m_tLeftHand_State.Set_State(STATE_LEFTHAND::HAND);
                    break;
                }
                case CPlayer::OBJECT_TYPE::STEELPIPE:
                {
                    m_tLeftHand_State.Set_State(STATE_LEFTHAND::HAND);
                    break;
                }
            }
        }
        else
        {
            // �޼� ������
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::RIGHTER);
        }
        // ������
        switch (m_eObjectType)
        {
            case CPlayer::OBJECT_TYPE::NONE:
            {
                m_tRightHand_State.Set_State(STATE_RIGHTHAND::HAND);
                break;
            }
            case CPlayer::OBJECT_TYPE::GUN:
            {
                m_tRightHand_State.Set_State(STATE_RIGHTHAND::GUN);
                break;
            }
            case CPlayer::OBJECT_TYPE::THOMPSON:
            {
                m_tRightHand_State.Set_State(STATE_RIGHTHAND::HAND); //
                break;
            }
            case CPlayer::OBJECT_TYPE::STEELPIPE:
            {
                m_tRightHand_State.Set_State(STATE_RIGHTHAND::STEELPIPE);
                break;
            }
        }
    }
    // �÷��̾ �ٰ� �ִ� ���
    else if (m_tPlayer_State.Get_State() == STATE_PLAYER::RUN)
    {
        // �����͸� �� ä�� �ٴ� ���
        if (bRighter)
        {
            // �޼� ������
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::RUN_RIHGTER);
            // ������ �ָ�
            m_tRightHand_State.Set_State(STATE_RIGHTHAND::RUN_HAND);
        }
        // �����͸� Ű�� ���� ���
        else
        {
            // ��� �� �ָ�
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::RUN_HAND);
            m_tRightHand_State.Set_State(STATE_RIGHTHAND::RUN_HAND);
        }
        
    }
}

void CPlayer::State_Update(float fTimeDelta)
{ 
    // �÷��̾ ������� ��
    if (!bDead)
    {
        // �÷��̾��� ����
        m_tPlayer_State.Get_StateFunc()(this, fTimeDelta);

        Hand_Check();

        // �÷��̾��� �޼� ����
        m_tLeftHand_State.Get_StateFunc()(this, fTimeDelta);

        // �÷��̾��� ������ ����
        m_tRightHand_State.Get_StateFunc()(this, fTimeDelta);
    }
    else
    {
        // �÷��̾� ����
    }
}

#pragma region �÷��̾��� ����
void CPlayer::Idle(float fTimeDelta)
{
    if (m_tPlayer_State.IsState_Entered())
    {

    }

    if (m_tPlayer_State.Can_Update())
    {
        
    }

    if (m_tPlayer_State.IsState_Exit())
    {

    }
}

void CPlayer::Move(float fTimeDelta)
{
    if (m_tPlayer_State.IsState_Entered())
    {

    }

    if (m_tPlayer_State.Can_Update())
    {

    }

    if (m_tPlayer_State.IsState_Exit())
    {

    }
}

void CPlayer::Run(float fTimeDelta)
{
    if (m_tPlayer_State.IsState_Entered())
    {

    }

    if (m_tPlayer_State.Can_Update())
    {

    }

    if (m_tPlayer_State.IsState_Exit())
    {

    }
}

void CPlayer::Down(float fTimeDelta)
{
    if (m_tPlayer_State.IsState_Entered())
    {

    }

    if (m_tPlayer_State.Can_Update())
    {

    }

    if (m_tPlayer_State.IsState_Exit())
    {

    }
}

void CPlayer::Attack(float fTimeDelta)
{
    if (m_tPlayer_State.IsState_Entered())
    {

    }

    if (m_tPlayer_State.Can_Update())
    {

    }

    if (m_tPlayer_State.IsState_Exit())
    {

    }
}

void CPlayer::Die(float fTimeDelta)
{
    if (m_tPlayer_State.IsState_Entered())
    {

    }

    if (m_tPlayer_State.Can_Update())
    {

    }

    if (m_tPlayer_State.IsState_Exit())
    {

    }
}

#pragma endregion

#pragma region �÷��̾��� �޼� ����

void CPlayer::Left_Hand(float fTimeDelta)
{
    if (m_tLeftHand_State.IsState_Entered())
    {
        m_pLeftHandComp = m_pLeftHandTextureComp;
    }

    if (m_tLeftHand_State.Can_Update())
    {

    }

    if (m_tLeftHand_State.IsState_Exit())
    {

    }
}

void CPlayer::Left_Gun(float fTimeDelta)
{
    if (m_tLeftHand_State.IsState_Entered())
    {

    }

    if (m_tLeftHand_State.Can_Update())
    {

    }

    if (m_tLeftHand_State.IsState_Exit())
    {

    }
}

void CPlayer::Left_Thompson(float fTimeDelta)
{
    if (m_tLeftHand_State.IsState_Entered())
    {

    }

    if (m_tLeftHand_State.Can_Update())
    {

    }

    if (m_tLeftHand_State.IsState_Exit())
    {

    }
}

void CPlayer::Left_Steelpipe(float fTimeDelta)
{
    if (m_tLeftHand_State.IsState_Entered())
    {
        // �� ����
    }

    if (m_tLeftHand_State.Can_Update())
    {

    }

    if (m_tLeftHand_State.IsState_Exit())
    {

    }
}

void CPlayer::Left_Righter(float fTimeDelta)
{
    if (m_tLeftHand_State.IsState_Entered())
    {
        // �޼��� �����ͷ� ����
        m_pLeftHandComp = m_pLeftZippoTextureComp;
        m_fMaxFrame = 6.f; // �ִ� ������ ����
    }

    if (m_tLeftHand_State.Can_Update())
    {
        // ���� �������� �ִ� �����Ӻ��� ũ�ų� ���� ���(�ִϸ��̼��� �� ������ ��)
        if (m_fLeftFrame >= m_fMaxFrame)
        {
            // ������ ��� Off
            bLeftFrameOn = false;
            m_fLeftFrame = m_fMaxFrame - 1.f; //�̰� �ȵ�
        }
    }

    if (m_tLeftHand_State.IsState_Exit())
    {

    }
}

#pragma endregion

#pragma region �÷��̾��� ������ ����

void CPlayer::Right_Hand(float fTimeDelta)
{
    if (m_tRightHand_State.IsState_Entered())
    {
        // ������ Hand��
        m_pRightHandComp = m_pRightHandTextureComp;
    }

    if (m_tRightHand_State.Can_Update())
    {
        
    }

    if (m_tRightHand_State.IsState_Exit())
    {

    }
}

void CPlayer::Right_Gun(float fTimeDelta)
{
    if (m_tRightHand_State.IsState_Entered())
    {
        // ���� �������� �ִ� �����Ӻ��� ũ�ų� ���� ���(�ִϸ��̼��� �� ������ ��)
        if (m_fRightFrame >= m_fMaxFrame)
        {
            // �� ȸ�� Off
            bSpinOn = false;
        }
        // �� ȸ���� �������� ���
        if (!bSpinOn)
        {
            // ������ ��
            m_pRightHandComp = m_pAttackTextureComp;
            m_fMaxFrame = 3.f; // �ִ� ������ ����
        }
        // �� ȸ���� �������� ���
        if (bSpinOn)
        {
            // ������ �� ȸ��
            m_pRightHandComp = m_pAttackSpinTextureComp;
            m_fMaxFrame = 4.f; // �ִ� ������ ����
        }
    }

    if (m_tRightHand_State.Can_Update())
    {

    }

    if (m_tRightHand_State.IsState_Exit())
    {

    }
}

void CPlayer::Right_Thompson(float fTimeDelta)
{
    if (m_tRightHand_State.IsState_Entered())
    {

    }

    if (m_tRightHand_State.Can_Update())
    {

    }

    if (m_tRightHand_State.IsState_Exit())
    {

    }
}

void CPlayer::Right_Steelpipe(float fTimeDelta)
{
    if (m_tRightHand_State.IsState_Entered())
    {
        m_pRightHandComp = m_pRightSteelPipeTextureComp;
    }

    if (m_tRightHand_State.Can_Update())
    {

    }

    if (m_tRightHand_State.IsState_Exit())
    {

    }
}

#pragma endregion