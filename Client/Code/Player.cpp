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


#pragma region 초기 상태 세팅 (현재 상태)
    // 플레이어
    m_tPlayer_State.Set_State(STATE_PLAYER::IDLE);
    // 왼손
    m_tLeftHand_State.Set_State(STATE_LEFTHAND::HAND);
    // 오른손
    m_tRightHand_State.Set_State(STATE_RIGHTHAND::HAND);;
#pragma endregion


#pragma region 플레이어의 상태 추가
    m_tPlayer_State.Add_Func(STATE_PLAYER::IDLE, &ThisClass::Idle);
    m_tPlayer_State.Add_Func(STATE_PLAYER::MOVE, &ThisClass::Move);
    m_tPlayer_State.Add_Func(STATE_PLAYER::RUN, &ThisClass::Run);
    m_tPlayer_State.Add_Func(STATE_PLAYER::DOWN, &ThisClass::Down);
    m_tPlayer_State.Add_Func(STATE_PLAYER::ATTACK, &ThisClass::Attack);
    m_tPlayer_State.Add_Func(STATE_PLAYER::DIE, &ThisClass::Die);
#pragma endregion

#pragma region 플레이어의 왼손 상태 추가
    m_tLeftHand_State.Add_Func(STATE_LEFTHAND::HAND, &ThisClass::Left_Hand);
    m_tLeftHand_State.Add_Func(STATE_LEFTHAND::RIGHTER, &ThisClass::Left_Righter);
#pragma endregion

#pragma region 플레이어의 오른손 상태 추가
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

    // 마우스 움직임
    Mouse_Move();
    // 마우스 입력
    Attack_Input(fTimeDelta);
    // 키보드 입력
    Keyboard_Input(fTimeDelta);

    // 상태 업데이트(체크)
    State_Update(fTimeDelta);

    // 지형 타기
    Height_On_Terrain();

    // 왼손 프레임On
    if (bLeftFrameOn)
    {
        // 현재 프레임을 시간(프레임)마다 증가시키기
        m_fLeftFrame += 10.f * fTimeDelta;

        // 현재 프레임이 최대 프레임에 도달한 경우
        if (m_fLeftFrame >= m_fMaxFrame)
        {
            // 현재 프레임을 0으로 초기화
            m_fLeftFrame = 0;

            // 왼손 프레임 Off
            bLeftFrameOn = false;

            // 만약 최대프레임인데 라이터가 켜져있을 경우
            if (bRighter)
            {
                // (현재 프레임) 라이터를 켜져있는 이미지로 고정
                m_fLeftFrame = 5.f;
            }

            // 권총이 회전중이였을 때
            if (bSpinOn)
            {
                // 회전 Off
                bSpinOn = false;

                // 권총으로 다시 돌아가기
                bGunOn = true;

                // 최대 프레임을 권총 기준으로 다시 맞춰놓기
                m_fMaxFrame = 3.f;
            }
        }
    }

    // 오른손 프레임
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
    // 조명 테스트
    if (bTorch)
    {
        m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
        //SetUp_Material();
    }

    // 행렬 적용
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_WorldMatrix());
    // 랜더 상태 옵션
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    // 왼손 위치 설정
    m_pBufferComp->Set_Vertex(-3.5f, -0.9f, 0.f);
    m_pLeftHandComp->Render_Texture((_ulong)m_fLeftFrame);  // 왼손 텍스처
    m_pBufferComp->Render_Buffer();             // 왼손 버퍼
    
    // (왼손)설정 값 되돌려주기
    m_pBufferComp->Set_Vertex(3.5f, 0.f, 0.f);

    // 오른손 위치 설정
    m_pBufferComp->Set_Vertex(0.2f, 0.f, 0.f);
    //if (m_fFrame < m_fMaxFrame) // 현재 프레임이 Max가 아닐 경우
    {
        m_pRightHandComp->Render_Texture((_ulong)m_fRightFrame); // 오른손 텍스처
        m_pBufferComp->Render_Buffer();                            // 오른손 버퍼
    }
    // (오른손)설정 값 되돌려주기
    m_pBufferComp->Set_Vertex(-0.2f, 0.f, 0.f);

    // 초기 테스트 버전
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

    // 조명 테스트
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

    // 플레이어 왼손 텍스처
    NULL_CHECK_RETURN(m_pLeftHandComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Comp_TextureLeft", L"Proto_PlayerLeftTextureComp"), E_FAIL);
    // 플레이어 오른손 텍스처
    NULL_CHECK_RETURN(m_pRightHandComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Comp_TextureRight", L"Proto_PlayerRightTextureComp"), E_FAIL);
    

    // Test (갈아 낄 이미지)
    // 왼손 이미지
    NULL_CHECK_RETURN(m_pLeftHandTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_TextureLeftHandTest", L"Proto_PlayerLeftHandTextureComp"), E_FAIL);
    // 오른손 이미지
    NULL_CHECK_RETURN(m_pRightHandTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_TextureRightHandTest", L"Proto_PlayerRightHandTextureComp"), E_FAIL);
    // 쇠파이프 이미지
    NULL_CHECK_RETURN(m_pRightSteelPipeTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_TextureRightSteelPipeTest", L"Proto_PlayerRightSteelPipeTextureComp"), E_FAIL);
    // 라이터 이미지
    NULL_CHECK_RETURN(m_pLeftZippoTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_TextureLeftZippoTest", L"Proto_PlayerLeftZippoTextureComp"), E_FAIL);
    // 플레이어 공격 이미지
    NULL_CHECK_RETURN(m_pAttackTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_TextureAttack", L"Proto_PlayerAttackTextureComp"), E_FAIL);
    // 플레이어 총 회전 이미지
    NULL_CHECK_RETURN(m_pAttackSpinTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_TextureAttackTest", L"Proto_PlayerAttackTestTextureComp"), E_FAIL);
    
    // 텍스처 매니저
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

    // 뛰기
    if (Engine::Get_DIKeyState(DIK_LSHIFT) & 0x80)
    {
        fSpeed = 10.f;
    }

    // 전진
    if (Engine::Get_DIKeyState(DIK_W) & 0x80)
    {
        D3DXVec3Normalize(&vLook, &vLook);
        m_pTransformComp->Move_Pos(&vLook, fTimeDelta, fSpeed);
        
        bMove = true;
        //return bMove;
    }

    // 후진
    if (Engine::Get_DIKeyState(DIK_S) & 0x80)
    {
        D3DXVec3Normalize(&vLook, &vLook);
        m_pTransformComp->Move_Pos(&vLook, fTimeDelta, -fSpeed);

        bMove = true;
        //return bMove;
    }

    // 오른쪽
    if (Engine::Get_DIKeyState(DIK_D) & 0x80)
    {
        _vec3 vRight;
        m_pTransformComp->Get_Info(INFO_RIGHT, &vRight);
        D3DXVec3Normalize(&vRight, &vRight);
        m_pTransformComp->Move_Pos(&vRight, fTimeDelta, 5.f);

        bMove = true;
        //return bMove;
    }

    // 왼쪽
    if (Engine::Get_DIKeyState(DIK_A) & 0x80)
    {
        _vec3 vRight;
        m_pTransformComp->Get_Info(INFO_RIGHT, &vRight);
        D3DXVec3Normalize(&vRight, &vRight);
        m_pTransformComp->Move_Pos(&-vRight, fTimeDelta, 5.f);

        bMove = true;
        //return bMove;
    }

    // 권총
    if (Engine::Get_DIKeyState(DIK_Q) & 0x80)
    {
        //bAttackOn = true;
        //bGunOn = true;
        //bSpinOn = false;
        m_fMaxFrame = 3.f;

        //Test
        m_eObjectType = OBJECT_TYPE::GUN;
    }

    // 손
    if (Engine::Get_DIKeyState(DIK_E) & 0x80)
    {
        //bAttackOn = true;
       // bGunOn = true;

        //Test
        m_eObjectType = OBJECT_TYPE::NONE;
    }

    // 스핀 (장전)
    if (Engine::Get_DIKeyState(DIK_R) & 0x80)
    {
        bRightFrameOn = true;
        bSpinOn = true;
    }

    // 쇠파이프
    if (Engine::Get_DIKeyState(DIK_4) & 0x80)
    {
        //Test
        m_eObjectType = OBJECT_TYPE::STEELPIPE;
    }

    // 라이터 (몬스터나옴 : 아마도 키가 한번에 여러번 입력되어서 그런듯)
    if (Engine::Get_DIKeyState(DIK_V) & 0x80)
    {
        //Test
        if (!bRighter)  // 라이터가 꺼져있을 경우
        {
            bRighter = true;     // 라이터 켜주기
            bLeftFrameOn = true; // 프레임 재생
        }
        else // 라이터가 켜져있을 경우
        {
            bRighter = false; // 임시 방편으로 끄면 해결..
        }       
    }

    fSpeed = 5.f;

    return false;
}

bool CPlayer::Attack_Input(const _float& fTimeDelta)
{
    // 발차기
    //if (Engine::Get_DIKeyState(DIK_Q) & 0x80)
    //{

    //    bFootAttack = true;
    //    return bFootAttack;
    //}

    // 마우스 좌클릭
    if (Engine::Get_DIMouseState(DIM_LB) & 0x80)
    {
        bRightFrameOn = true;

        bMouse_Button = true;
        return bMouse_Button;
    }

    // 마우스 우클릭
    if (Engine::Get_DIMouseState(DIM_RB) & 0x80)
    {

        bMouse_Button = true;
        return bMouse_Button;
    }

    return false;
}

// 마우스 움직임
void CPlayer::Mouse_Move()
{
    // 마우스로 플레이어 회전
    // 상, 하
    if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_Y))
    {
        m_pTransformComp->Rotation(ROT_X, D3DXToRadian(dwMouseMove / 10.f));
    }
    // 좌, 우
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

// 손 상태 확인
void CPlayer::Hand_Check()
{
    // 플레이어가 안뛰고있을 때
    if (m_tPlayer_State.Get_State() != STATE_PLAYER::RUN)
    {
        // 라이터를 안 킨 경우
        if (!bRighter)
        {
            // 왼손  (대부분 NONE으로 바껴야함)
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
            // 왼손 라이터
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::RIGHTER);
        }
        // 오른손
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
    // 플레이어가 뛰고 있는 경우
    else if (m_tPlayer_State.Get_State() == STATE_PLAYER::RUN)
    {
        // 라이터를 켠 채로 뛰는 경우
        if (bRighter)
        {
            // 왼손 라이터
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::RUN_RIHGTER);
            // 오른손 주먹
            m_tRightHand_State.Set_State(STATE_RIGHTHAND::RUN_HAND);
        }
        // 라이터를 키지 않은 경우
        else
        {
            // 양손 다 주먹
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::RUN_HAND);
            m_tRightHand_State.Set_State(STATE_RIGHTHAND::RUN_HAND);
        }
        
    }
}

void CPlayer::State_Update(float fTimeDelta)
{ 
    // 플레이어가 살아있을 때
    if (!bDead)
    {
        // 플레이어의 상태
        m_tPlayer_State.Get_StateFunc()(this, fTimeDelta);

        Hand_Check();

        // 플레이어의 왼손 상태
        m_tLeftHand_State.Get_StateFunc()(this, fTimeDelta);

        // 플레이어의 오른손 상태
        m_tRightHand_State.Get_StateFunc()(this, fTimeDelta);
    }
    else
    {
        // 플레이어 죽음
    }
}

#pragma region 플레이어의 상태
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

#pragma region 플레이어의 왼손 상태

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
        // 손 없음
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
        // 왼손을 라이터로 변경
        m_pLeftHandComp = m_pLeftZippoTextureComp;
        m_fMaxFrame = 6.f; // 최대 프레임 지정
    }

    if (m_tLeftHand_State.Can_Update())
    {
        // 현재 프레임이 최대 프레임보다 크거나 같을 경우(애니메이션을 다 돌았을 때)
        if (m_fLeftFrame >= m_fMaxFrame)
        {
            // 프레임 재생 Off
            bLeftFrameOn = false;
            m_fLeftFrame = m_fMaxFrame - 1.f; //이거 안됨
        }
    }

    if (m_tLeftHand_State.IsState_Exit())
    {

    }
}

#pragma endregion

#pragma region 플레이어의 오른손 상태

void CPlayer::Right_Hand(float fTimeDelta)
{
    if (m_tRightHand_State.IsState_Entered())
    {
        // 오른손 Hand로
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
        // 현재 프레임이 최대 프레임보다 크거나 같을 경우(애니메이션을 다 돌았을 때)
        if (m_fRightFrame >= m_fMaxFrame)
        {
            // 총 회전 Off
            bSpinOn = false;
        }
        // 총 회전이 꺼져있을 경우
        if (!bSpinOn)
        {
            // 오른손 총
            m_pRightHandComp = m_pAttackTextureComp;
            m_fMaxFrame = 3.f; // 최대 프레임 지정
        }
        // 총 회전이 켜져있을 경우
        if (bSpinOn)
        {
            // 오른손 총 회전
            m_pRightHandComp = m_pAttackSpinTextureComp;
            m_fMaxFrame = 4.f; // 최대 프레임 지정
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