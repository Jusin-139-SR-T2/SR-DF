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

#pragma region �÷��̾� ũ�� �� ��ġ ���� (�ʱ� ��)
    m_pTransformComp->m_vScale.x = 0.3f;
    m_pTransformComp->m_vScale.y = 0.4f;

    m_pTransformComp->m_vInfo[INFO_POS] = { 15.f, 10.f, 10.f };
#pragma endregion


#pragma region �ʱ� ���� ���� (���� ����)  
    m_tPlayer_State.Set_State(STATE_PLAYER::IDLE);           // �÷��̾� 
    m_tLeftHand_State.Set_State(STATE_LEFTHAND::HAND);       // �޼�  
    m_tRightHand_State.Set_State(STATE_RIGHTHAND::HAND);;    // ������
#pragma endregion


#pragma region �÷��̾��� ���� �߰�
    m_tPlayer_State.Add_Func(STATE_PLAYER::IDLE, &ThisClass::Idle);         // �⺻ (����)
    m_tPlayer_State.Add_Func(STATE_PLAYER::MOVE, &ThisClass::Move);         // ������
    m_tPlayer_State.Add_Func(STATE_PLAYER::RUN, &ThisClass::Run);           // �޸���
    m_tPlayer_State.Add_Func(STATE_PLAYER::DOWN, &ThisClass::Down);         // �ɱ�
    m_tPlayer_State.Add_Func(STATE_PLAYER::ATTACK, &ThisClass::Attack);     // ����
    m_tPlayer_State.Add_Func(STATE_PLAYER::KICK, &ThisClass::Kick);         // ������
    m_tPlayer_State.Add_Func(STATE_PLAYER::DIE, &ThisClass::Die);           // ����
#pragma endregion


#pragma region �÷��̾��� �޼� ���� �߰�
    m_tLeftHand_State.Add_Func(STATE_LEFTHAND::NONE, &ThisClass::Left_None);                // �޼� ����
    m_tLeftHand_State.Add_Func(STATE_LEFTHAND::HAND, &ThisClass::Left_Hand);                // �޼� �ָ�
    m_tLeftHand_State.Add_Func(STATE_LEFTHAND::RUN_RIHGTER, &ThisClass::Left_RunRighter);   // �޼� ������ ��� �ٰ�����
    m_tLeftHand_State.Add_Func(STATE_LEFTHAND::OPEN_HAND, &ThisClass::Left_OpenHand);       // ������ ������Ʈ
    m_tLeftHand_State.Add_Func(STATE_LEFTHAND::RUN_HAND, &ThisClass::Left_RunHand);         // �ٰ�����
    m_tLeftHand_State.Add_Func(STATE_LEFTHAND::RIGHTER, &ThisClass::Left_Righter);          // ������
#pragma endregion


#pragma region �÷��̾��� ������ ���� �߰�
    m_tRightHand_State.Add_Func(STATE_RIGHTHAND::NONE, &ThisClass::Right_None);             // ������ ����
    m_tRightHand_State.Add_Func(STATE_RIGHTHAND::HAND, &ThisClass::Right_Hand);             // ������ �ָ�
    m_tRightHand_State.Add_Func(STATE_RIGHTHAND::GUN, &ThisClass::Right_Gun);               // ����
    m_tRightHand_State.Add_Func(STATE_RIGHTHAND::RUN_HAND, &ThisClass::Right_RunHand);      // �޸���
    m_tRightHand_State.Add_Func(STATE_RIGHTHAND::THOMPSON, &ThisClass::Right_Thompson);     // �轼 �����
    m_tRightHand_State.Add_Func(STATE_RIGHTHAND::STEELPIPE, &ThisClass::Right_Steelpipe);   // ��������
    m_tRightHand_State.Add_Func(STATE_RIGHTHAND::BEERBOTLE, &ThisClass::Right_BeerBotle);   // ���ֺ�
    m_tRightHand_State.Add_Func(STATE_RIGHTHAND::FRYINGPAN, &ThisClass::Right_FryingPan);   // ��������
    m_tRightHand_State.Add_Func(STATE_RIGHTHAND::KICK, &ThisClass::Right_Kick);             // ������
#pragma endregion


    // Tset (������Ʈ �޾ƿ��°�)
    m_eObjectType = OBJECT_TYPE::NONE; // �ʱ���� : Ÿ��x
    m_eObjectName = OBJECT_NAME::NONE; // �ʱ���� : �̸�x

    // �÷��̾� ���� (�ʱⰪ)
    m_ePlayerState = STATE_PLAYER::NONE;

    // ������ ���� (�ʱⰪ)
    bLeftFrameOn = true;
    bRightFrameOn = true;

    // ���� ����
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

#pragma region �޼� ������On
    if (bLeftFrameOn)
    {
        // ���� �������� �ð�(������)���� ������Ű��
        m_fLeftFrame += 10.f * fTimeDelta;

        // ���� �������� �ִ� �����ӿ� ������ ���
        if (m_fLeftFrame >= m_fLeftMaxFrame)
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
        }
    }
#pragma endregion

#pragma region ������ ������On (�޼հ� ����)
    if (bRightFrameOn)
    {
        m_fRightFrame += 10.f * fTimeDelta;

        // �÷��̾� ������ �� �������� �� ������ ���
        if (m_ePlayerState == STATE_PLAYER::KICK &&
            m_fRightFrame > m_fRightMaxFrame)
        {
            m_ePlayerState = STATE_PLAYER::NONE; // �÷��̾� ���� �ʱ�ȭ
            Two_Hand(); // �� �ָ����� ���ư��� (���߿� �������·� ���ư��� �ؾ���)
        }

        if (m_fRightFrame >= m_fRightMaxFrame)
        {
            m_fRightFrame = 0;
            bRightFrameOn = false;

            // ������ ȸ�����̿��� ��
            if (bSpinOn)
            {
                // ȸ�� Off
                bSpinOn = false;

                // �������� �ٽ� ���ư���
                bGunOn = true;

                // �ִ� �������� ���� �������� �ٽ� �������
                m_fLeftMaxFrame = 4.f;
            }
        }
    }
#pragma endregion

    // ���� �׷� ����, ������� : ���� �׽�Ʈ
    Engine::Add_RenderGroup(RNEDER_ALPHATEST, this);

    // ���� �׷� ���
    //RENDER_PRIORITY, RNEDER_ALPHATEST, RENDER_NONALPHA, RENDER_ALPHA, RENDER_UI, RENDER_END

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

#pragma region �ɼ�
    // ��� ����
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_WorldMatrix());
    // ���� ���� �ɼ�
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
#pragma endregion

#pragma region �޼�
    // �޼� ��ġ ����
    m_pBufferComp->Set_Vertex(-3.5f, -0.9f, 0.f);

    // �޼� ��� ����
    if (bLeftHandOn)
    {
        // �޼� �ؽ�ó ���
        m_pLeftHandComp->Render_Texture((_ulong)m_fLeftFrame);  // �޼� �ؽ�ó ���
        m_pBufferComp->Render_Buffer();             // �޼� ���� 
    }

    // (�޼�)���� �� �ǵ����ֱ�
    m_pBufferComp->Set_Vertex(3.5f, 0.f, 0.f);
#pragma endregion

#pragma region ������
    // ������ ��ġ ����
    m_pBufferComp->Set_Vertex(0.2f, 0.f, 0.f);

    // ������ ��� ����
    if (bRightHandOn)
    {
        m_pRightHandComp->Render_Texture((_ulong)m_fRightFrame); // ������ �ؽ�ó ���
        m_pBufferComp->Render_Buffer();                            // ������ ����
    }

    // (������)���� �� �ǵ����ֱ�
    m_pBufferComp->Set_Vertex(-0.2f, 0.f, 0.f);
#pragma endregion

#pragma region �ʱ� �׽�Ʈ ����
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
#pragma endregion

#pragma region ���� �׽�Ʈ
    if (bTorch)
        m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, false);

    m_pBufferComp->Set_Vertex(0.f, 0.9f, 0.f);

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
#pragma endregion

}

HRESULT CPlayer::Add_Component()
{
#pragma region ������Ʈ
    // ���� ������Ʈ
    NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
    // ��� ������Ʈ
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    // ����Ÿ�� ������Ʈ
    NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculatorComponent>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);
    // �޼� ������Ʈ
    NULL_CHECK_RETURN(m_pLeftHandComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_TextureLeftHandTest", L"Proto_PlayerLeftTextureComp"), E_FAIL);
    // ������ ������Ʈ
    NULL_CHECK_RETURN(m_pRightHandComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_TextureRightHandTest", L"Proto_PlayerRightTextureComp"), E_FAIL);
#pragma endregion

    // �ؽ�ó �Ŵ���
    // �޼�                           �븻??
    m_pLeftHandComp->Receive_Texture(TEX_NORMAL, L"Player_Multi", L"Left_Hand");
    // ������
    m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player_Multi", L"Right_Hand");

    return S_OK;
}

// @@@�뽬 �۾���@@@
bool CPlayer::Keyboard_Input(const _float& fTimeDelta)
{
    _vec3	vLook;

    m_pTransformComp->Get_Info(INFO_LOOK, &vLook);
    vLook = { vLook.x, 0.f, vLook.z };

    _vec3 vInverse = (-m_pCamera->Get_At());

    m_pTransformComp->Compute_LootAtTarget(&vInverse);

#pragma region Ű�Է�
    // �ٱ�
    if (Engine::IsKey_Pressing(DIK_LSHIFT))
    {
        // ���� �ӵ� Up
        fStraightSpeed = 10.f;
        // �÷��̾� ���� : �޸���
        m_ePlayerState = STATE_PLAYER::RUN;
    }

    // �ٱ�
    if (Engine::IsKey_Released(DIK_LSHIFT))
    {
        // ���� �ӵ� ����
        fStraightSpeed = 5.f;
        // �÷��̾� ���� : �ʱ�ȭ
        m_ePlayerState = STATE_PLAYER::NONE;
    }

    // ����
    if (Engine::IsKey_Pressing(DIK_W))
    {
        D3DXVec3Normalize(&vLook, &vLook);
        m_pTransformComp->Move_Pos(&vLook, fTimeDelta, fStraightSpeed);

        bMove = true;
        //return bMove;
    }

    // ����
    if (Engine::IsKey_Pressing(DIK_S))
    {
        D3DXVec3Normalize(&vLook, &vLook);
        m_pTransformComp->Move_Pos(&vLook, fTimeDelta, -fSpeed);

        // S + Shift �� �뽬
        if (Engine::IsKey_Pressed(DIK_LSHIFT))
        {
            m_pTransformComp->Move_Pos(&vLook, fTimeDelta, -fDash);
        }
    }

    // ������
    if (Engine::IsKey_Pressing(DIK_D))
    {
        _vec3 vRight;
        m_pTransformComp->Get_Info(INFO_RIGHT, &vRight);
        D3DXVec3Normalize(&vRight, &vRight);
        m_pTransformComp->Move_Pos(&vRight, fTimeDelta, fSpeed);

        // D + Shift ���� �뽬
        if (Engine::IsKey_Pressed(DIK_LSHIFT))
        {
            m_pTransformComp->Move_Pos(&vRight, fTimeDelta, fDash);
        }
    }

    // ����
    if (Engine::IsKey_Pressing(DIK_A))
    {
        _vec3 vRight;
        m_pTransformComp->Get_Info(INFO_RIGHT, &vRight);
        D3DXVec3Normalize(&vRight, &vRight);
        m_pTransformComp->Move_Pos(&-vRight, fTimeDelta, fSpeed);

        // A + Shift ���� �뽬
        if (Engine::IsKey_Pressed(DIK_LSHIFT))
        {
            m_pTransformComp->Move_Pos(&-vRight, fTimeDelta, fDash);
        }
    }

    // ��
    if (Engine::IsKey_Pressed(DIK_1))
    {
        //Test
        m_eObjectType = OBJECT_TYPE::TWO_HAND;
        m_eObjectName = OBJECT_NAME::NONE;
    }

    // ����
    if (Engine::IsKey_Pressed(DIK_2))
    {
        //Test
        m_eObjectType = OBJECT_TYPE::RIGHT_OBJECT;
        m_eObjectName = OBJECT_NAME::GUN;
    }

    // �轼 �����
    if (Engine::IsKey_Pressed(DIK_3))
    {
        //Test
        m_eObjectType = OBJECT_TYPE::TWO_OBJECT;
        m_eObjectName = OBJECT_NAME::THOMPSON;
    }

    // ��������
    if (Engine::IsKey_Pressed(DIK_4))
    {
        //Test
        m_eObjectType = OBJECT_TYPE::RIGHT_OBJECT;
        m_eObjectName = OBJECT_NAME::STEELPIPE;
    }

    // ���ֺ�
    if (Engine::IsKey_Pressed(DIK_5))
    {
        //Test
        m_eObjectType = OBJECT_TYPE::RIGHT_OBJECT;
        m_eObjectName = OBJECT_NAME::BEERBOTLE;
    }

    // ��������
    if (Engine::IsKey_Pressed(DIK_6))
    {
        //Test
        m_eObjectType = OBJECT_TYPE::RIGHT_OBJECT;
        m_eObjectName = OBJECT_NAME::FRYINGPAN;
    }

    // ���� (����)
    if (Engine::IsKey_Pressed(DIK_R) && m_eObjectName == OBJECT_NAME::GUN)
    {
        bRightFrameOn = true;
        bSpinOn = true;
    }

    // ������
    if (Engine::IsKey_Pressed(DIK_Q))
    {
        m_ePlayerState = STATE_PLAYER::KICK;
        bRightFrameOn = true;
    }

    // ������
    if (Engine::IsKey_Pressed(DIK_V) && m_ePlayerState != STATE_PLAYER::RUN)
    {
        //Test
        if (!bRighter)  // �����Ͱ� �������� ���
        {
            bRighter = true;        // ������ ���ֱ�
            bLeftFrameOn = true;    // ������ ���
            m_fLeftMaxFrame = 6.f;  // �ִ� ������ ����
        }
        else // �����Ͱ� �������� ���
        {
            m_fLeftFrame = 0.f;
            bRighter = false;
            bLeftFrameOn = false;
            //Test
        }
    }
#pragma endregion

    // �÷��̾� �⺻ �ӵ�
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

#pragma region ���콺
    // ���콺 ��Ŭ��
    if (Engine::Get_DIMouseState(DIM_LB) & 0x80)
    {
        bMouse_Button = true;

        bRightFrameOn = true;

        // �ָ� �����ư��� ���� �۾� ������
        //// ��� �� �ָԻ����� ���
        //if (bLeftHandFist && bRightHandFist)
        //{
        //    // ��� ������ ��� 0�ϰ��
        //    if (m_fRightFrame == 0.f &&
        //        m_fLeftFrame == 0.f)
        //    {
        //        m_fRightFrame++;
        //    }

        //    // ������ �������� 1���� Ŭ ���
        //    if (m_fRightFrame > 1)
        //    {
        //        m_fRightFrame--;
        //        m_fLeftFrame++;
        //    }

        //    // �޼� �������� 1���� Ŭ ���
        //    if (m_fLeftFrame > 1)
        //    {
        //        m_fLeftFrame--;
        //        m_fRightFrame++;
        //    }
        //}


        //// �޼� �ָԻ��°� �������� ���
        //if (bLeftHandFist)
        //{
        //    m_fRightFrame = 0.f;

        //    // ������ �ָԻ��°� �������� ���
        //    if (bRightHandFist)
        //    {
        //        bRightHandFist = false; // �������� ��
        //        m_fRightFrame++;
        //        m_fLeftFrame--;
        //        return 0;
        //    }

        //    m_fLeftFrame++;

        //}

        return bMouse_Button;
    }

    // ���콺 ��Ŭ��
    if (Engine::Get_DIMouseState(DIM_RB) & 0x80)
    {
        bMouse_Button = true;
        return bMouse_Button;
    }
#pragma endregion

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

#pragma region ��� �ָ� (�⺻ ����)
void CPlayer::Two_Hand()
{
    // �ִ� ������ ����
    //m_fLeftMaxFrame = 2.f;
    //m_fRightMaxFrame = 2.f;

    // ��¡ ����
    bChargingReady = true;

    // ������ �ָ�
    m_tRightHand_State.Set_State(STATE_RIGHTHAND::HAND);

    // �����͸� ������ ���
    if (!bRighter)
        // �޼� �ָ�
        m_tLeftHand_State.Set_State(STATE_LEFTHAND::HAND);

}
#pragma endregion

#pragma region ��� ������Ʈ
void CPlayer::Two_Object()
{
    // �����Ͱ� �����ִ� ���·� ��� ������Ʈ�� ������ ���
    if (bRighter)
    {
        // �޼� ���� ������ �ʱ�ȭ
        m_fLeftFrame = 0.f;
        bRighter = false;       // ������ ����
    }

    bRighterSwitch = false; // ������ ����ġ ����

    switch (m_eObjectName)
    {
        // �轼 �����
    case CPlayer::OBJECT_NAME::THOMPSON:
    {
        // ������ �轼 ����� (���)
        m_tRightHand_State.Set_State(STATE_RIGHTHAND::THOMPSON);
        // �޼� ����
        m_tLeftHand_State.Set_State(STATE_LEFTHAND::NONE);
        break;
    }
    }
}
#pragma endregion

#pragma region �Ѽ� ������Ʈ
void CPlayer::Right_Object()
{
    // ������
    switch (m_eObjectName)
    {
        // ����
    case CPlayer::OBJECT_NAME::GUN:
    {
        // ������ ���� (�Ѽ�)
        m_tRightHand_State.Set_State(STATE_RIGHTHAND::GUN);
        // �޼� ����
        m_eLeftState = STATE_LEFTHAND::NONE;
        // ��¡ �Ұ�
        bChargingReady = false;
        break;
    }
    // ��������
    case CPlayer::OBJECT_NAME::STEELPIPE:
    {
        // ������ ��������
        m_tRightHand_State.Set_State(STATE_RIGHTHAND::STEELPIPE);
        // �޼� ���� �ڵ�
        m_eLeftState = STATE_LEFTHAND::OPEN_HAND;
        // ��¡ ����
        bChargingReady = false;
        break;
    }
    // ���ֺ�
    case CPlayer::OBJECT_NAME::BEERBOTLE:
    {
        // ������ ���ֺ�
        m_tRightHand_State.Set_State(STATE_RIGHTHAND::BEERBOTLE);
        // �޼� ���� �ڵ�
        m_eLeftState = STATE_LEFTHAND::OPEN_HAND;
        // ��¡ �Ұ�
        bChargingReady = false;
        break;
    }
    // ��������
    case CPlayer::OBJECT_NAME::FRYINGPAN:
    {
        // ������ ��������
        m_tRightHand_State.Set_State(STATE_RIGHTHAND::FRYINGPAN);
        // �޼� ���� �ڵ�
        m_eLeftState = STATE_LEFTHAND::OPEN_HAND;
        // ��¡ ����
        bChargingReady = false;
        break;
    }
    }

    // �޼�
    if (!bRighter) // �����͸� ������ ���
    {
        switch (m_eLeftState) // ���� �޼� ����
        {
        case CPlayer::STATE_LEFTHAND::NONE: // �޼� ����
        {
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::NONE);
            break;
        }
        case CPlayer::STATE_LEFTHAND::HAND: // �޼� �ָ�
        {
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::HAND);
            break;
        }
        case CPlayer::STATE_LEFTHAND::OPEN_HAND: // �޼� ���� �ڵ�
        {
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::OPEN_HAND);
            break;
        }
        }
    }
    //else // �����͸� ���� ���
    //{
    //    // �޼� ������
    //    m_tLeftHand_State.Set_State(STATE_LEFTHAND::RIGHTER);
    //}
}
#pragma endregion

// �� ���� Ȯ��
void CPlayer::Hand_Check()
{
    // �޼� ������ ��� On
    bLeftHandOn = true;
    bRightHandOn = true;

    // �÷��̾ �ȶٰ��ִ� ���
    if (m_ePlayerState != STATE_PLAYER::RUN)
    {
        if (bRighter) // �÷��̾ �����͸� Ų ���
        {
            // �޼� ������
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::RIGHTER);
        }

#pragma region ������Ʈ Ÿ�� üũ
        if (m_ePlayerState != STATE_PLAYER::KICK) // �÷��̾ �����⸦ ���� ���
        {
            switch (m_eObjectType)
            {
                case CPlayer::OBJECT_TYPE::TWO_HAND:        // ��� �ָ�
                {
                    Two_Hand();
                    break;
                }
                case CPlayer::OBJECT_TYPE::TWO_OBJECT:      // ��� ������Ʈ (�μ� ����)
                {
                    Two_Object();
                    break;
                }
                case CPlayer::OBJECT_TYPE::RIGHT_OBJECT:    // �Ѽ� ������Ʈ (�Ѽ� ����)
                {
                    Right_Object();
                    break;
                }
            }
        }
        else if (m_ePlayerState == STATE_PLAYER::KICK) // �÷��̾ �����⸦ �� ���
        {
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::NONE); // �޼�Off
            m_tRightHand_State.Set_State(STATE_RIGHTHAND::KICK); // ������ ������
        }
#pragma endregion
    }
#pragma region �÷��̾ �ٰ� �ִ� ���
    else if (m_ePlayerState == STATE_PLAYER::RUN)
    {
        // �����͸� �� ä�� �ٴ� ���
        if (bRighter)
        {
            // �޼� ������ �޸���
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::RUN_RIHGTER);
            // ������ �Ǽ� �޸���
            m_tRightHand_State.Set_State(STATE_RIGHTHAND::RUN_HAND);
        }
        // �����͸� Ű�� ���� ���
        else
        {
            // ��� �� �Ǽ� �޸���
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::RUN_HAND);
            m_tRightHand_State.Set_State(STATE_RIGHTHAND::RUN_HAND);
        }
    }
#pragma endregion

}

void CPlayer::State_Update(float fTimeDelta)
{
    // �÷��̾ ������� ��
    if (!bDead)
    {
        // �÷��̾��� ����
        m_tPlayer_State.Get_StateFunc()(this, fTimeDelta);

        // �÷��̾� �� ���� üũ
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

void CPlayer::Kick(float fTimeDelta)
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

void CPlayer::Left_None(float fTimeDelta)
{
    if (m_tLeftHand_State.IsState_Entered())
    {
        // �޼� ��� Off
        bLeftHandOn = false;
        // �޼� ���� ������ �ʱ�ȭ
        //m_fLeftFrame = 0.f;
    }

    if (m_tLeftHand_State.Can_Update())
    {

    }

    if (m_tLeftHand_State.IsState_Exit())
    {

    }

}

void CPlayer::Left_Hand(float fTimeDelta)
{
    if (m_tLeftHand_State.IsState_Entered())
    {
        // �⺻ �޼� ���
        m_pLeftHandComp->Receive_Texture(TEX_NORMAL, L"Player_Multi", L"Left_Hand");
        m_fLeftMaxFrame = 2.f;  // �ִ� ������ ����
        bRighter = false;       // ������ Off

        // �ָ� �۾� ������
        //bLeftFrameOn = false;   // �޼� ������ Off
        //bLeftHandFist = true;   // �޼� �ָԻ���On
    }

    if (m_tLeftHand_State.Can_Update())
    {

    }

    if (m_tLeftHand_State.IsState_Exit())
    {

    }
}

// �޼� �޸��� ������.ver
void CPlayer::Left_RunRighter(float fTimeDelta)
{
    if (m_tLeftHand_State.IsState_Entered())
    {
        // ������On (�ִ�������) �״��, ���� �� �Ʒ� �����̰� ������ֱ�
    }

    if (m_tLeftHand_State.Can_Update())
    {

    }

    if (m_tLeftHand_State.IsState_Exit())
    {

    }
}

// �Ѽ� ������Ʈ�϶� �޼� ���� �ڵ�
void CPlayer::Left_OpenHand(float fTimeDelta)
{
    if (m_tLeftHand_State.IsState_Entered())
    {
        // �޼� ���� �ڵ�� ����
        m_pLeftHandComp->Receive_Texture(TEX_NORMAL, L"Player_Single", L"OpenHand");
    }

    if (m_tLeftHand_State.Can_Update())
    {

    }

    if (m_tLeftHand_State.IsState_Exit())
    {

    }
}

void CPlayer::Left_RunHand(float fTimeDelta)
{
    if (m_tLeftHand_State.IsState_Entered())
    {
        // �޼� �ٴ� ������ ����
        m_pLeftHandComp->Receive_Texture(TEX_NORMAL, L"Player_Single", L"Left_RunHand");
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
        m_pLeftHandComp->Receive_Texture(TEX_NORMAL, L"Player_Multi", L"Righter");
        m_fLeftMaxFrame = 6.f; // �ִ� ������ ����
    }

    if (m_tLeftHand_State.Can_Update())
    {
        // ���� �������� �ִ� �����Ӻ��� ũ�ų� ���� ���(�ִϸ��̼��� �� ������ ��)
        if (m_fLeftFrame >= m_fLeftMaxFrame)
        {
            // ������ ��� Off
            bLeftFrameOn = false;
            m_fLeftFrame = m_fLeftMaxFrame - 1.f; //�̰� �ȵ�
        }

        if (!bRighter)
        {
            // �޼� �ָ�
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::HAND);
        }
    }

    if (m_tLeftHand_State.IsState_Exit())
    {

    }
}

#pragma endregion

#pragma region �÷��̾��� ������ ����

void CPlayer::Right_None(float fTimeDelta)
{
    if (m_tRightHand_State.IsState_Entered())
    {
        // ������ ��� Off
        bRightHandOn = false;
        // ������ ���� ������ �ʱ�ȭ
        m_fRightFrame = 0.f;
    }

    if (m_tRightHand_State.Can_Update())
    {

    }

    if (m_tRightHand_State.IsState_Exit())
    {

    }
}

void CPlayer::Right_Hand(float fTimeDelta)
{
    if (m_tRightHand_State.IsState_Entered())
    {
        // �⺻ ������ ���
        m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player_Multi", L"Right_Hand");
        m_fRightMaxFrame = 2.f; // �ִ� ������ ����

        // �ָ� �۾� ������
        //bRightFrameOn = false;   // ������ ������ Off
        //bRightHandFist = true;  // ������ �ָԻ���On
    }

    if (m_tRightHand_State.Can_Update())
    {

    }

    if (m_tRightHand_State.IsState_Exit())
    {

    }
}

void CPlayer::Right_RunHand(float fTimeDelta)
{
    if (m_tRightHand_State.IsState_Entered())
    {
        // �޼� �ٴ� ������ ����
        m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player_Single", L"Right_RunHand");
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
        // ������ ��
        m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player_Multi", L"Gun");
        m_fRightMaxFrame = 5.f; // �ִ� ������ ����
    }

    if (m_tRightHand_State.Can_Update())
    {
        // ���� �������� �ִ� �����Ӻ��� ũ�ų� ���� ���(�ִϸ��̼��� �� ������ ��)
        if (m_fRightFrame >= m_fRightMaxFrame)
        {
            // �� ȸ�� Off
            bSpinOn = false;
        }
        // �� ȸ���� �������� ���
        if (!bSpinOn)
        {
            // ������ ��
            m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player_Multi", L"Gun");
            m_fRightMaxFrame = 3.f; // �ִ� ������ ����
        }
        // �� ȸ���� �������� ���
        if (bSpinOn)
        {
            // ������ �� ȸ��
            m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player_Multi", L"Gun_Spin");
            m_fRightMaxFrame = 4.f; // �ִ� ������ ����
        }
    }

    if (m_tRightHand_State.IsState_Exit())
    {

    }
}

void CPlayer::Right_Thompson(float fTimeDelta)
{
    if (m_tRightHand_State.IsState_Entered())
    {
        // ������ �轼 �����
        m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player_Multi", L"Thompson");
        m_fRightMaxFrame = 4.f; // �ִ� ������ ����
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
        // ������ ��������
        m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player_Multi", L"Steel_Pipe");
        m_fRightMaxFrame = 4.f; // �ִ� ������ ����
    }

    if (m_tRightHand_State.Can_Update())
    {

    }

    if (m_tRightHand_State.IsState_Exit())
    {

    }
}

void CPlayer::Right_BeerBotle(float fTimeDelta)
{
    if (m_tRightHand_State.IsState_Entered())
    {
        // ������ ���ֺ�
        m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player_Multi", L"BeerBottle");
        m_fRightMaxFrame = 5.f; // �ִ� ������ ����
    }

    if (m_tRightHand_State.Can_Update())
    {

    }

    if (m_tRightHand_State.IsState_Exit())
    {

    }
}

void CPlayer::Right_FryingPan(float fTimeDelta)
{
    if (m_tRightHand_State.IsState_Entered())
    {
        // ������ ��������
        m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player_Multi", L"FryingPan");
        m_fRightMaxFrame = 5.f; // �ִ� ������ ����
    }

    if (m_tRightHand_State.Can_Update())
    {

    }

    if (m_tRightHand_State.IsState_Exit())
    {

    }
}

void CPlayer::Right_Kick(float fTimeDelta)
{
    if (m_tRightHand_State.IsState_Entered())
    {
        // ������ ������
        m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player_Multi", L"Kick");
        m_fRightMaxFrame = 4.f; // �ִ� ������ ����
    }

    if (m_tRightHand_State.Can_Update())
    {

    }

    if (m_tRightHand_State.IsState_Exit())
    {

    }
}

#pragma endregion