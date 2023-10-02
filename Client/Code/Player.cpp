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
    m_pTransformComp->Set_Pos({10.f, 0.f, 10.f});

    // �޼�
    m_fSizeX = 300;
    m_fSizeY = 300;

    m_fX = -200.f; // ������ġ 
    m_fY = -280.f;

    m_pLeftHandComp->Set_Pos({ m_fX, m_fY, 0.f });	// �̹��� ��ġ
    m_pLeftHandComp->Set_Scale({ m_fSizeX, m_fSizeY, 1.f });						// �̹��� ũ��

    // ������
    m_fSizeX = 300;
    m_fSizeY = 300;

    m_fX = -m_fX; // ������ġ

    m_pRightHandComp->Set_Pos({ m_fX, m_fY, 0.f });	// �̹��� ��ġ
    m_pRightHandComp->Set_Scale({ m_fSizeX, m_fSizeY, 1.f });						// �̹��� ũ��
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
    m_eObjectType = OBJECT_TYPE::TWO_HAND; // �ʱ���� : ��� �ָ�
    m_eObjectName = OBJECT_NAME::NONE; // �ʱ���� : ����

    // �÷��̾� ���� (�ʱⰪ)
    m_ePlayerState = STATE_PLAYER::NONE;

    // ������ ���� (�ʱⰪ)
    bLeftFrameOn = true;
    bRightFrameOn = true;

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

    // �뽬
    Dash(fTimeDelta);

    // ������ ����
    FrameManage(fTimeDelta);

    // ���� �׷� ����, ������� : ���� �׽�Ʈ
    Engine::Add_RenderGroup(RENDER_UI, this);

    // ���� �׷� ���
    //RENDER_PRIORITY, RENDER_ALPHATEST, RENDER_NONALPHA, RENDER_ALPHA, RENDER_UI, RENDER_END

    return 0;
}

void CPlayer::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CPlayer::Render_GameObject()
{
#pragma region �ɼ�
    // ��� ����
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());

    // ���� ���� �ɼ�
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
#pragma endregion

#pragma region �޼�
    // �޼� ��ġ ����
    m_pLeftHandComp->Readjust_Transform();
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pLeftHandComp->Get_Transform());

    // �޼� ��� ����
    if (bLeftHandOn)
    {
        // �޼� �ؽ�ó ���
        m_pLeftHandComp->Render_Texture((_ulong)m_fLeftFrame, true);  // �޼� �ؽ�ó ���
        m_pBufferComp->Render_Buffer();                         // �޼� ���� 
    }
#pragma endregion

#pragma region ������
    // ������ ��ġ ����
    m_pRightHandComp->Readjust_Transform();
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pRightHandComp->Get_Transform());

    // ������ ��� ����
    if (bRightHandOn)
    {
        m_pRightHandComp->Render_Texture((_ulong)m_fRightFrame, true);    // ������ �ؽ�ó ���
        m_pBufferComp->Render_Buffer();                             // ������ ����
    }
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

#pragma region �ɼ� ���� 

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
    // �޼�
    m_pLeftHandComp->Receive_Texture(TEX_NORMAL, L"Player_Multi", L"Left_Hand");
    // ������
    m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player_Multi", L"Right_Hand");

    return S_OK;
}

bool CPlayer::Keyboard_Input(const _float& fTimeDelta)
{
    _vec3	vLook;

    m_pTransformComp->Get_Info(INFO_LOOK, &vLook);
    vLook = { vLook.x, 0.f, vLook.z };

    _vec3 vInverse = (-m_pCamera->Get_At());

    m_pTransformComp->Compute_LootAtTarget(&vInverse);

#pragma region Ű�Է�
    // ����
    if (Engine::IsKey_Pressing(DIK_W))
    {
        // �ٱ�
        if (Engine::IsKey_Pressing(DIK_LSHIFT))
        {
            // ���� �ӵ� Up
            fStraightSpeed = 10.f;
            // �÷��̾� ���� : �޸���
            m_ePlayerState = STATE_PLAYER::RUN;
        }

        D3DXVec3Normalize(&vLook, &vLook);
        m_pTransformComp->Move_Pos(&vLook, fTimeDelta, fStraightSpeed);
        // ���� �ӵ� ����
        fStraightSpeed = 5.f;
    }

    // �ٱ�
    if (Engine::IsKey_Released(DIK_LSHIFT))
    {
        // �÷��̾� ���� : �ʱ�ȭ
        m_ePlayerState = STATE_PLAYER::NONE;
    }

    // ����
    if (Engine::IsKey_Pressing(DIK_S))
    {
        D3DXVec3Normalize(&vLook, &vLook);
        m_pTransformComp->Move_Pos(&vLook, fTimeDelta, -fSpeed);

        // S + Shift �� �뽬
        if (Engine::IsKey_Pressed(DIK_LSHIFT))
        {
            fDownDash = 0.f;    // �뽬 ����(ī�޶�) ����
            fDash = 20.f;       // �뽬 �� ����
            bDashOn = true;     // �뽬 On/Off
            m_eDashDir = DOWN;  // �뽬 ����
        }
    }

    // ������
    if (Engine::IsKey_Pressing(DIK_D))
    {
        _vec3 vRight;
        m_pTransformComp->Get_Info(INFO_RIGHT, &vRight);
        D3DXVec3Normalize(&vRight, &vRight);
        m_pTransformComp->Move_Pos(&vRight, fTimeDelta, fSpeed);

        // fSpeed = 5.f , fDash = 20.f
        // D + Shift ���� �뽬
        if (Engine::IsKey_Pressed(DIK_LSHIFT))
        {
            fDownDash = 0.f;    // �뽬 ����(ī�޶�) ����
            fDash = 20.f;       // �뽬 �� ����
            bDashOn = true;     // �뽬 On/Off
            m_eDashDir = RIGHT; // �뽬 ����
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
            fDownDash = 0.f;    // �뽬 ����(ī�޶�) ����
            fDash = 20.f;       // �뽬 �� ����
            bDashOn = true;     // �뽬 On/Off
            m_eDashDir = LEFT;  // �뽬 ����
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
        bLeftFrameOn = true;    // ������ ���

        if (!bRighter)  // �����Ͱ� �������� ���
        {
            bRighter = true;        // ������ ���ֱ�
            m_fLeftMaxFrame = 6.f;  // �ִ� ������ ����
            m_eLeftState = STATE_LEFTHAND::RIGHTER; // �޼� ���� �����ͷ�
        }
        else // �����Ͱ� �������� ���
        {
            bBackRighter = true; // ������ �ǵ�����On
        }
    }

    // ������Ʈ ������ (������Ʈ�� ���� ��츸 ���డ��)
    if (Engine::IsKey_Pressed(DIK_F) && m_eObjectName != OBJECT_NAME::NONE)
    {
        // �÷��̾� ���� ������Ʈ ������ ��
        m_ePlayerState = STATE_PLAYER::THROW_AWAY;
        bRightFrameOn = true;
    }

    // ������Ʈ ��ȣ�ۿ� // �ҿ� �߰� 
    /*if (Engine::IsKey_Pressed(DIK_E) && Picking_On_Object())
    {
        int a = 0;
    }*/
#pragma endregion

    // �÷��̾� �⺻ �ӵ�
    fSpeed = 5.f;

    return false;
}

void CPlayer::FrameManage(const _float& fTimeDelta)
{
#pragma region �޼� ������On
    if (bLeftFrameOn)
    {
        // ���带 ���� ���
        if (!bRighter && bShieldOn)
        {
            m_fLeftFrame = m_fLeftMaxFrame; // ���� ���
        }
        else
        {
            // ������ �ǵ����Ⱑ �������� ���
            if (!bBackRighter)
            {
                // ���� �������� �ð�(������)���� ������Ű��
                m_fLeftFrame += fLeftFrameSpeed * fTimeDelta;

                // ����� �ָ� ���� �ϰ��
                if (bLeftHandFist && bRightHandFist)
                {
                    // �޼� �������� ������ �����Ӻ��� Ŭ ���
                    if (m_fLeftFrame > m_fLeftMaxFrame)
                    {
                        bLeftPunch = false;  // �޼� �ָ� Off
                        bRightPunch = true;    // ������ �ָ� On
                    }
                }

                // ���� �������� �ִ� �����ӿ� ������ ���
                if (m_fLeftFrame >= m_fLeftMaxFrame)
                {
                    // ���� �ִ��������ε� �����Ͱ� �������� ���
                    if (bRighter)
                    {
                        // (���� ������) �����͸� �����ִ� �̹����� ����
                        m_fLeftFrame = 5.f;
                    }
                    else // �����Ͱ� ���������� ���
                    {
                        // ���� ������ �ʱ�ȭ
                        m_fLeftFrame = 0.f;

                        // �޼� ������ Off
                        bLeftFrameOn = false;
                    }
                }
            }
            else // ������ �ǵ�����On
            {
                // ���� �������� �ð�(������)���� ���ҽ�Ű��
                m_fLeftFrame -= fLeftFrameSpeed * fTimeDelta;

                // �޼� �������� 0�� �������� ���
                if (m_fLeftFrame <= 0.f)
                {
                    m_fLeftFrame = 0.f;
                    bRighter = false;
                    bLeftFrameOn = false;
                    bBackRighter = false;
                }
            }
        }
    }
#pragma endregion

#pragma region ������ ������On (�޼հ� ����)
    if (bRightFrameOn)
    {
        // ���带 ���� ���
        if (bShieldOn)
        {
            m_fRightFrame = m_fRightMaxFrame; // ���� ���
        }
        else // ���带 ������ ���
        {
            // ������ ������ ����
            m_fRightFrame += fRightFrameSpeed * fTimeDelta;

            // ��¡ ������ ������, ���� �������� Ǯ ��¡ �����ӿ� �������� ���
            if (bChargeAttack &&
                m_fRightFrame >= fFullChage)
            {
                // ���� �������� Ǯ ��¡�� ����
                m_fRightFrame = fFullChage;
            }

            // �÷��̾� ������ �� �������� �� ������ ���
            if (m_ePlayerState == STATE_PLAYER::KICK &&
                m_fRightFrame > m_fRightMaxFrame)
            {
                m_ePlayerState = STATE_PLAYER::NONE; // �÷��̾� ���� �ʱ�ȭ
                Two_Hand(); // �� �ָ����� ���ư��� (���߿� �������·� ���ư��� �ؾ���)
            }

            // ������ �������� �ִ� �����ӿ� �������� ���
            if (m_fRightFrame >= m_fRightMaxFrame)
            {
                // ���� �ִ��������ε� ������ ���� ���
                if (m_ePlayerState == STATE_PLAYER::THROW_AWAY)
                {
                    // (���� ������) �������� Throw �̹����� ����
                    m_fRightFrame = m_fRightMaxFrame;

                    // �ð����� �ؽ�ó ���� �ð��� ä���ش�.
                    fTextureChangeTime += fCurChangeTime * fTimeDelta;
                }
                else // ������ ���� �ƴ� ���
                {
                    // �÷��̾� ���°� ��¡�� ���
                    if (m_ePlayerState == STATE_PLAYER::CHARGING)
                    {
                        m_ePlayerState = STATE_PLAYER::NONE; // �÷��̾� ���� : �ʱ�ȭ
                        fChageTime = 0.f; // ��¡ �ð� �ʱ�ȭ
                    }
                    else
                    {
                        // ����� �ָ� ���� �ϰ��
                        if (bLeftHandFist && bRightHandFist)
                        {
                            // ������ �������� �޼� �����Ӻ��� Ŭ ���
                            if (m_fRightFrame > m_fRightMaxFrame)
                            {
                                bRightPunch = false;  // ������ �ָ� Off
                                bLeftPunch = true;    // �޼� �ָ� On
                            }
                        }
                    }

                    // ������ ������ �ʱ�ȭ
                    m_fRightFrame = 0;
                    bRightFrameOn = false; // ������ ������Off

                    // ������ ȸ�����̿��� ��
                    if (bSpinOn)
                    {
                        // ȸ�� Off
                        bSpinOn = false;

                        // �������� �ٽ� ���ư���
                        bGunOn = true;

                        // �ִ� �������� ���� �������� �ٽ� �������
                        m_fRightMaxFrame = 4.f;
                    }
                }

            }
        }
    }
#pragma endregion
}

void CPlayer::Charge(const _float& fTimeDelta)
{
    if (m_eObjectName == OBJECT_NAME::THOMPSON)
    {
        fChageTime = fFullChargeTime;
    }
    // ��¡ ��ȯ �ð� ����
    fChageTime += 10.f * fTimeDelta;

    // �ð� �ʰ��� ��¡���
    if (fChageTime >= fFullChargeTime)
    {

        switch (m_eObjectName)
        {
        case CPlayer::OBJECT_NAME::NONE:
        {
            m_ePlayerState = STATE_PLAYER::CHARGING; // �÷��̾� ���� : ��¡
            fFullChage = 0.f;       // Ǯ��¡ ������
            bChargeAttack = true;   // ��¡����On
            bRightFrameOn = true;   // ������ ������On
            break;
        }
        case CPlayer::OBJECT_NAME::GUN:
        {

            break;
        }
        case CPlayer::OBJECT_NAME::THOMPSON:
        {
            bRightFrameOn = true;
            break;
        }
        case CPlayer::OBJECT_NAME::STEELPIPE: // ��������
        {
            m_ePlayerState = STATE_PLAYER::CHARGING; // �÷��̾� ���� : ��¡
            fFullChage = 1.f;       // Ǯ��¡ ������
            bChargeAttack = true;   // ��¡����On
            bRightFrameOn = true;   // ������ ������On
            break;
        }
        case CPlayer::OBJECT_NAME::BEERBOTLE:
        {

            break;
        }
        case CPlayer::OBJECT_NAME::FRYINGPAN: // ��������
        {
            m_ePlayerState = STATE_PLAYER::CHARGING; // �÷��̾� ���� : ��¡
            fFullChage = 1.f;       // Ǯ��¡ ������
            bChargeAttack = true;   // ��¡����On
            bRightFrameOn = true;   // ������ ������On
            break;
        }
        }
    }
}

_bool CPlayer::Picking_On_Object()
{
    CRcBufferComp* pObjectBufferCom = dynamic_cast<CRcBufferComp*>(Engine::Get_Component(ID_STATIC, L"GameLogic", L"Food1", L"Proto_RcTexBufferComp"));
    NULL_CHECK_RETURN(pObjectBufferCom, _vec3());
    CTransformComponent* pObjectTransCom = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Food1", L"Proto_TransformComp"));
    NULL_CHECK_RETURN(pObjectTransCom, _vec3());

    _vec3 PickingPos = m_pCalculatorComp->Picking_On_Object(g_hWnd, pObjectBufferCom, pObjectTransCom);
    
    if (PickingPos)
        return true;
    else
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

    // ���콺 ��Ŭ�� (������ ���� ��)
    if (Engine::IsMouse_Pressing(DIM_LB))
    {
        Charge(fTimeDelta);
    }

    // ��¡�� �������� ��
    if (!bChargeAttack)
    {
        // ���콺 ��Ŭ�� (���� ��)
        if (Engine::IsMouse_Pressed(DIM_LB))
        {
            // ��� �� �ָԻ����� ���
            if (bLeftHandFist && bRightHandFist)
            {
                if (bLeftPunch)
                {
                    bLeftFrameOn = true;
                }
                if (bRightPunch)
                {
                    bRightFrameOn = true;
                }
            }
            else // ������
            {
                bRightFrameOn = true;
            }
        }
    }

    // ���콺 ��Ŭ�� (�� ��)
    if (Engine::IsMouse_Released(DIM_LB))
    {
        bChargeAttack = false;  // ��¡����Off
    }

    // ���콺 ��Ŭ��
    if (Engine::IsMouse_Pressing(DIM_RB))
    {
        // ���� ���� �϶�
        if (bShield)
        {
            // ���� �������� 0�� ��
            if (m_fRightFrame == 0)
            {
                // ����On
                bShieldOn = true;
                // ������ ������On
                bRightFrameOn = true;

                // �޼��� �ָ��� ���
                if (bLeftHandFist)
                    bLeftFrameOn = true; // �޼� ������On
            }
        }
    }
    else
    {
        // ����Off
        bShieldOn = false;
    }
#pragma endregion

    return false;
}

// ���콺 ������
void CPlayer::Mouse_Move()
{
#pragma region 1��Ī
    if (m_pCamera->Get_One())
    {
        //m_pTransformComp->Rotate(ROT_X, m_pCamera->Get_At());

        //���콺�� �÷��̾� ȸ��
        //��, ��
        if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_Y))
        {
            m_pTransformComp->Rotate(ROT_X, D3DXToRadian(dwMouseMove / 10.f));
        }
        // ��, ��
        if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_X))
        {
            m_pTransformComp->Rotate(ROT_Y, D3DXToRadian(dwMouseMove / 10.f));
        }
    }
#pragma endregion

#pragma region 3��Ī ���ʹϾ�
    if (m_pCamera->Get_Three())
    {
        _vec3   CameraToPlayer;

        m_pTransformComp->Get_Info(INFO_POS, &m_vPlayerPos);

        CameraToPlayer = m_vPlayerPos - m_pCamera->Get_Eye();

        // ī�޶� �ٶ󺸴� At
        //m_vCameraAt = m_pCamera->Get_At();

        // �÷��̾ �ٶ󺸴� Look
        //m_pTransformComp->Get_Info(INFO_LOOK, &m_vPlayerLook);

        // ī�޶�� �÷��̾ �ٶ󺸴� ���̸� �Ȱ��� �����ش�.
        CameraToPlayer.y = 0.f;
        //m_vPlayerLook.y = 0.f;

        // ī�޶�� �÷��̾� ���͸� ����ȭ
        D3DXVec3Normalize(&CameraToPlayer, &CameraToPlayer);
        D3DXVec3Normalize(&vLook, &vLook);

        float fAngle;


        if (CameraToPlayer.x > 0)
        {
            // ������ �̿��� �� ���� ���ΰ��� �����ش�.
            fDot = D3DXVec3Dot(&CameraToPlayer, &vLook);
            // �� ���� ������ ���� ��� (����)
            fAngle = acosf(fDot);
        }
        else
        {
            //vLook = vLook * -1;
            // ������ �̿��� �� ���� ���ΰ��� �����ش�.
            fDot = D3DXVec3Dot(&CameraToPlayer, &(vLook * -1));
            // �� ���� ������ ���� ��� (����)
            fAngle = acosf(fDot);

            fAngle = fAngle + 3.141592f;
        }

        m_pTransformComp->Set_ScaleY(fAngle);
    }
#pragma endregion

#pragma region 1��Ī ���ʹϾ� ȸ�� (���)
    //dwMouseMoveX = Engine::Get_DIMouseMove(DIMS_X);
    //dwMouseMoveY = Engine::Get_DIMouseMove(DIMS_Y);

    //D3DXQUATERNION quatPitch, quatYaw, quatRoll;

    //// �÷��̾��� Up ���͸� �����´�.
    //m_pTransformComp->Get_Info(INFO_UP, &m_vUp);

    //// �÷��̾��� Look ���͸� �����´�.
    //m_pTransformComp->Get_Info(INFO_LOOK, &m_vLook);

    //// ������ �÷��̾��� Up, Look ���ͷ� Right���� ���ϱ�
    //D3DXVec3Cross(&m_vRight, &m_vUp, &m_vLook);

    //// ���� ȸ�� (Y�� ȸ��)
    //if (dwMouseMoveX != 0)
    //{
    //    // ���콺 X �̵��� ���� Y�� ������ ȸ���ϴ� ���ʹϾ� ���
    //    D3DXQuaternionRotationAxis(&quatYaw, &m_vUp, D3DXToRadian(static_cast<_float>(dwMouseMoveX) * 1.f));

    //    // ���� ���ʹϾ� ���ο� ȸ���� ���Ͽ� ������Ų��.
    //    m_quaternion *= quatYaw;

    //    // ������ ȸ�� ���ʹϾ� ����ȭ
    //    D3DXQuaternionNormalize(&m_quaternion, &m_quaternion);
    //}

    //// ���� ȸ�� (X�� ȸ��)
    //if (dwMouseMoveY != 0)
    //{
    //    // ���콺 X �̵��� ���� Y�� ������ ȸ���ϴ� ���ʹϾ� ���
    //    D3DXQuaternionRotationAxis(&quatPitch, &m_vRight, D3DXToRadian(static_cast<_float>(dwMouseMoveY) * 1.f));

    //    // ���� ���ʹϾ� ���ο� ȸ���� ���Ͽ� ������Ų��.
    //    m_quaternion *= quatPitch;

    //    // ������ ȸ�� ���ʹϾ� ����ȭ
    //    D3DXQuaternionNormalize(&m_quaternion, &m_quaternion);
    //}

    //// ���ʹϾ��� ����Ͽ� ��ȯ ����� ����ϱ�
    //D3DXMATRIX matRotation;
    //D3DXMatrixRotationQuaternion(&matRotation, &m_quaternion);

    //// ����� �÷��̾��� ��ȯ�� �����ϱ�.
    //m_pTransformComp->Set_WorldMatrix(matRotation);   
#pragma endregion
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

void CPlayer::Dash(const _float& fTimeDelta)
{
    _vec3	vLook;

    m_pTransformComp->Get_Info(INFO_LOOK, &vLook);
    vLook = { vLook.x, 0.f, vLook.z };

    _vec3 vRight;
    m_pTransformComp->Get_Info(INFO_RIGHT, &vRight);
    D3DXVec3Normalize(&vRight, &vRight);

    _vec3 vPos;
    m_pTransformComp->Get_Info(INFO_POS, &vPos);

    _vec3 vSpeed = { 0.f, -fDownDash, 0.f };

    // �뽬 ����
    if (bDashOn)
    {
        // �뽬 ���� �ִ�ġ
        if (fDownDash >= 40.f)
        {
            bDashChange = true; // ���� ����
        }

        if (bDashChange)
        {
            fDownDash--; // �ٿ� ��ġ ����
        }
        else
        {
            fDownDash++; // �ٿ� ��ġ ����
        }

        // �뽬 ����
        m_pTransformComp->Set_MovePos(fTimeDelta, vSpeed);

        // �뽬 �ӵ��� ���ǵ庸�� ���� ��츸 �۵�
        if (fDash > fSpeed)
        {
            // �뽬 ���� �� �̵�
            switch (m_eDashDir)
            {
            case CPlayer::LEFT:
                m_pTransformComp->Move_Pos(&-vRight, fTimeDelta, fDash);
                break;
            case CPlayer::RIGHT:
                m_pTransformComp->Move_Pos(&vRight, fTimeDelta, fDash);
                break;
            case CPlayer::DOWN:
                m_pTransformComp->Move_Pos(&vLook, fTimeDelta, -fDash);
                break;
            }
            fDash--; // �뽬 �ӵ� ����
        }
        else // �뽬 �ӵ��� ���ǵ�� ������
        {

        }

        if (fDownDash <= 0.f)
        {
            bDashChange = false;
            bDashOn = false; // �뽬 ����
        }
    }

}

#pragma region ��� �ָ� (�⺻ ����)
void CPlayer::Two_Hand()
{
    // �ִ� ������ ����
    //m_fLeftMaxFrame = 2.f;
    //m_fRightMaxFrame = 2.f;

    bChargingReady = true; // ��¡ ����
    m_eObjectName = OBJECT_NAME::NONE; // ������Ʈ ����

    // ������ �ָ�
    m_tRightHand_State.Set_State(STATE_RIGHTHAND::HAND);
    bRightHandFist = true; // ������ �ָ� ����On

    // �����͸� ������ ���
    if (!bRighter)
    {
        // �޼� �ָ�
        m_tLeftHand_State.Set_State(STATE_LEFTHAND::HAND);
        bLeftHandFist = true; // �޼� �ָ� ����On
    }

}
#pragma endregion

#pragma region ��� ������Ʈ
void CPlayer::Two_Object()
{
    // ��� �ָ� ����Off
    bRightHandFist = false;
    bLeftHandFist = false;

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
    // ��� �ָ� ����Off
    bRightHandFist = false;
    bLeftHandFist = false;

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
        // �÷��̾ ������Ʈ�� ���������� ���
        if (m_ePlayerState == STATE_PLAYER::THROW_AWAY)
        {
            // ������Ʈ �ʱ�ȭ (����)
            m_eObjectType = OBJECT_TYPE::NONE;
            m_eObjectName = OBJECT_NAME::NONE;
        }

        if (bRighter) // �÷��̾ �����͸� Ų ���
        {
            // �޼� ������
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::RIGHTER);
        }

#pragma region ������Ʈ Ÿ�� üũ
        if (m_ePlayerState != STATE_PLAYER::KICK) // �÷��̾ �����⸦ ���� ���
        {
            // ������Ʈ�� NONE�� ���
            if (m_eObjectType == OBJECT_TYPE::NONE ||
                m_eObjectName == OBJECT_NAME::NONE)
            {
                m_eObjectType = OBJECT_TYPE::TWO_HAND; // �Ǽ�
            }

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

void CPlayer::Throw_Away(float fTimeDelta)
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
        // �÷��̾ ������ �� �ϰ��
        if (m_ePlayerState == STATE_PLAYER::THROW_AWAY)
        {
            bLeftHandOn = false; // �޼�Off
        }
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
            if (bRighter)
            {
                // ������ ��� Off
                //bLeftFrameOn = false;
                //m_fLeftFrame = m_fLeftMaxFrame - 1.f;
            }
            else // �����Ͱ� ������ ���
            {
                if (m_fLeftFrame <= 0) // ��� �������� ���
                {
                    // �޼� �ָ�
                    m_tLeftHand_State.Set_State(STATE_LEFTHAND::HAND);
                }
            }
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
        bShield = false; // ��� �Ұ�
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
        fRightFrameSpeed = 10.f;// ������ �ӵ� ���� (���� �ӵ�)
        bShield = true;         // ��� ����

        // �ָ� �۾� ������
        //bRightFrameOn = false;   // ������ ������ Off
        //bRightHandFist = true;  // ������ �ָԻ���On
    }

    if (m_tRightHand_State.Can_Update())
    {
        // �÷��̾ ��¡ ������ ���
        if (m_ePlayerState == STATE_PLAYER::CHARGING)
        {
            // ��¡ �ؽ�ó�� ����
            m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player_Multi", L"RightHand_Charging");
            m_fRightMaxFrame = 2.f;  // �ִ� ������ ����
            fRightFrameSpeed = 5.f;  // ������ �ӵ� ���� (���� �ӵ�)
            bShield = false;         // ��� �Ұ���
        }
        // �÷��̾ ������ �� �ϰ��
        if (m_ePlayerState == STATE_PLAYER::THROW_AWAY)
        {
            // ������ �ؽ�ó�� ����
            m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player_Multi", L"UnderThrow_RightHand");
            m_fRightMaxFrame = 0.f;  // �ִ� ������ ����
            fRightFrameSpeed = 4.f;  // ������ �ӵ� ���� (���� �ӵ�)
            bShield = false;         // ��� �Ұ���
        }

        // ���� �ؽ�ó ���� �ð��� �ִ� ����ð� ���� ũ�ų� ���� ���(�����ϰ� ���� �ð��� ���� ��)
        if (fTextureChangeTime >= fMaxChangeTime)
        {
            // �÷��̾� ���� �ʱ�ȭ
            m_ePlayerState = STATE_PLAYER::NONE;
            fTextureChangeTime = 0.f; // �ؽ�ó ����ð� �ʱ�ȭ
        }
    }

    if (m_tRightHand_State.IsState_Exit())
    {

    }
}

void CPlayer::Right_RunHand(float fTimeDelta)
{
    if (m_tRightHand_State.IsState_Entered())
    {
        // ������ �ٴ� ������ ����
        m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player_Single", L"Right_RunHand");
        bShield = false; // ��� �Ұ�
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
        fRightFrameSpeed = 10.f;// ������ �ӵ� ���� (���� �ӵ�)
        bShield = false;        // ��� �Ұ�
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
        fRightFrameSpeed = 35.f;// ������ �ӵ� ���� (���� �ӵ�)
        bShield = false;        // ��� �Ұ�
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
        fRightFrameSpeed = 10.f;// ������ �ӵ� ���� (���� �ӵ�)
        bShield = true;         // ��� ����
    }

    if (m_tRightHand_State.Can_Update())
    {
        // �÷��̾ ��¡ ������ ���
        if (m_ePlayerState == STATE_PLAYER::CHARGING)
        {
            // ��¡ �ؽ�ó�� ����
            m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player_Multi", L"Steel_Pipe_Charging");
            m_fRightMaxFrame = 5.f; // �ִ� ������ ����
            fRightFrameSpeed = 8.f;// ������ �ӵ� ���� (���� �ӵ�)
            bShield = false;         // ��� �Ұ���
        }
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
        fRightFrameSpeed = 10.f;// ������ �ӵ� ���� (���� �ӵ�)
        bShield = false;        // ��� �Ұ�
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
        fRightFrameSpeed = 10.f;// ������ �ӵ� ���� (���� �ӵ�)
        bShield = true;         // ��� ����
    }

    if (m_tRightHand_State.Can_Update())
    {
        // �÷��̾ ��¡ ������ ���
        if (m_ePlayerState == STATE_PLAYER::CHARGING)
        {
            // ��¡ �ؽ�ó�� ����
            m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player_Multi", L"FryingPan_Charging");
            m_fRightMaxFrame = 4.f; // �ִ� ������ ����
            fRightFrameSpeed = 9.f;// ������ �ӵ� ���� (���� �ӵ�)
            bShield = false;         // ��� �Ұ���
        }
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
        fRightFrameSpeed = 7.f; // ������ �ӵ� ���� (���� �ӵ�)
        bShield = false;        // ��� �Ұ�
    }

    if (m_tRightHand_State.Can_Update())
    {

    }

    if (m_tRightHand_State.IsState_Exit())
    {

    }
}

#pragma endregion