#include "stdafx.h"
#include "Player.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "DynamicCamera.h"
#include "CalculatorComponent.h"
#include "ColliderComponent.h"
#include "PlayerLighter.h"
#include "Management.h"
#include "BlackBoard_Player.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
    Set_ObjectName(L"Player");
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

#pragma region 블랙보드

    Engine::Add_BlackBoard(L"Player", CBlackBoard_Player::Create());

#pragma endregion


#pragma region 플레이어 크기 및 위치 설정 (초기 값)
    m_pTransformComp->Set_Pos({ 10.f, 0.f, 10.f });
    m_pTransformComp->Readjust_Transform();
    //m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // 충돌 불러오는곳 
    FCollisionSphere* pShape = dynamic_cast<FCollisionSphere*>(m_pColliderComp->Get_Shape());
    pShape->fRadius = 0.5f;

    /*FCollisionBox* pShape = dynamic_cast<FCollisionBox*>(m_pColliderComp->Get_Shape());
    pShape->fRadius = 5.f;*/

    // 왼손
    m_fSizeX = 300;
    m_fSizeY = 300;

    m_fX = -200.f; // 중점위치 
    m_fY = -280.f;
    
    m_pLeftHandComp->Set_Pos({ -300.f, -363.f, 0.f });	// 이미지 위치
    m_pLeftHandComp->Set_Scale({ 1495, 1588, 1.f });						// 이미지 크기

    // 오른손
    m_eRIGHTState = CPlayer::STATE_RIGHTHAND::NONE;
    m_fSizeX = 300;
    m_fSizeY = 300;

    m_fX = -m_fX; // 중점위치

    m_pRightHandComp->Set_Pos({ 0.f, 0.f, 0.f });	// 이미지 위치
    m_pRightHandComp->Set_Scale({ m_fSizeX, m_fSizeY, 1.f });						// 이미지 크기
#pragma endregion

#pragma region 초기 상태 세팅 (현재 상태)  
    m_tPlayer_State.Set_State(STATE_PLAYER::IDLE);           // 플레이어 
    m_tLeftHand_State.Set_State(STATE_LEFTHAND::HAND);       // 왼손  
    m_tRightHand_State.Set_State(STATE_RIGHTHAND::HAND);;    // 오른손
#pragma endregion


#pragma region 플레이어의 상태 추가
    m_tPlayer_State.Add_Func(STATE_PLAYER::IDLE, &ThisClass::Idle);         // 기본 (정지)
    m_tPlayer_State.Add_Func(STATE_PLAYER::MOVE, &ThisClass::Move);         // 움직임
    m_tPlayer_State.Add_Func(STATE_PLAYER::RUN, &ThisClass::Run);           // 달리기
    m_tPlayer_State.Add_Func(STATE_PLAYER::DOWN, &ThisClass::Down);         // 앉기
    m_tPlayer_State.Add_Func(STATE_PLAYER::ATTACK, &ThisClass::Attack);     // 공격
    m_tPlayer_State.Add_Func(STATE_PLAYER::KICK, &ThisClass::Kick);         // 발차기
    m_tPlayer_State.Add_Func(STATE_PLAYER::DIE, &ThisClass::Die);           // 죽음
#pragma endregion


#pragma region 플레이어의 왼손 상태 추가
    m_tLeftHand_State.Add_Func(STATE_LEFTHAND::NONE, &ThisClass::Left_None);                // 왼손 없음
    m_tLeftHand_State.Add_Func(STATE_LEFTHAND::HAND, &ThisClass::Left_Hand);                // 왼손 주먹
    m_tLeftHand_State.Add_Func(STATE_LEFTHAND::RUN_RIHGTER, &ThisClass::Left_RunRighter);   // 왼손 라이터 들고 뛰고있음
    m_tLeftHand_State.Add_Func(STATE_LEFTHAND::OPEN_HAND, &ThisClass::Left_OpenHand);       // 오른손 오브젝트
    m_tLeftHand_State.Add_Func(STATE_LEFTHAND::RUN_HAND, &ThisClass::Left_RunHand);         // 뛰고있음
    m_tLeftHand_State.Add_Func(STATE_LEFTHAND::RIGHTER, &ThisClass::Left_Righter);          // 라이터
#pragma endregion


#pragma region 플레이어의 오른손 상태 추가
    m_tRightHand_State.Add_Func(STATE_RIGHTHAND::NONE, &ThisClass::Right_None);             // 오른손 없음
    m_tRightHand_State.Add_Func(STATE_RIGHTHAND::HAND, &ThisClass::Right_Hand);             // 오른손 주먹
    m_tRightHand_State.Add_Func(STATE_RIGHTHAND::GUN, &ThisClass::Right_Gun);               // 권총
    m_tRightHand_State.Add_Func(STATE_RIGHTHAND::RUN_HAND, &ThisClass::Right_RunHand);      // 달리기
    m_tRightHand_State.Add_Func(STATE_RIGHTHAND::THOMPSON, &ThisClass::Right_Thompson);     // 톰슨 기관총
    m_tRightHand_State.Add_Func(STATE_RIGHTHAND::STEELPIPE, &ThisClass::Right_Steelpipe);   // 쇠파이프
    m_tRightHand_State.Add_Func(STATE_RIGHTHAND::BEERBOTLE, &ThisClass::Right_BeerBotle);   // 맥주병
    m_tRightHand_State.Add_Func(STATE_RIGHTHAND::FRYINGPAN, &ThisClass::Right_FryingPan);   // 프라이팬
    m_tRightHand_State.Add_Func(STATE_RIGHTHAND::KICK, &ThisClass::Right_Kick);             // 발차기
#pragma endregion

#pragma region 액션 키
    for (size_t i = 0; i < static_cast<size_t>(EACTION_KEY::SIZE); i++)
        m_tActionKey.Add_Action(static_cast<EACTION_KEY>(i));
    
#pragma endregion


    // Tset (오브젝트 받아오는거)
    m_eObjectType = OBJECT_TYPE::TWO_HAND; // 초기상태 : 양손 주먹
    m_eObjectName = OBJECT_NAME::NONE; // 초기상태 : 없음

    // 플레이어 상태 (초기값)
    m_ePlayerState = STATE_PLAYER::NONE;
    m_gHp.Max = 100.f;
    m_gHp.Cur = 100.f;

    // 프레임 상태 (초기값)
    bLeftFrameOn = true;
    bRightFrameOn = true;

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

    Interpolation();

    // 상태 업데이트(체크)
    State_Update(fTimeDelta);

    // 지형 타기
    Height_On_Terrain();

    // 대쉬
    Dash(fTimeDelta);

    // 프레임 관리
    FrameManage(fTimeDelta);

    // 물리 업데이트 코드
    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // 충돌체 이동

    //플레이어 뒤로 콜라가 생성되는 코드
    //if (Engine::IsKey_Pressed(DIK_BACK))
    //Engine::Add_GameObject(L"GameLogic", CAceObjectFactory::Create(m_pGraphicDev,CAceObjectFactory::OBJECT_CLASS::FOOD, 
    //    L"Test", m_pTransformComp->Get_Pos().x, m_pTransformComp->Get_Pos().y, m_pTransformComp->Get_Pos().z));

    //_vec3 vTest = m_pTransformComp->Get_Pos();
    //vTest.z += 1.f;
    //list<CGameObject*> listCollision = Engine::IntersectTests_Sphere_GetGameObject(0, vTest, 3.f);
    //for (auto iter = listCollision.begin(); iter != listCollision.end(); ++iter)
    //{
    //    if ((*iter) != this) // 자기자신을 죽이지 않는 보안코드 삽입 
    //        (*iter)->Set_Dead();
    //}


    // 블랙보드 연동하기
    Update_BlackBoard();

    m_tActionKey.Update();

    // 랜더 그룹 지정, 현재상태 : 알파 테스트
    Engine::Add_RenderGroup(RENDER_UI, this);

    // 랜더 그룹 목록
    //RENDER_PRIORITY, RENDER_ALPHATEST, RENDER_NONALPHA, RENDER_ALPHA, RENDER_UI, RENDER_END

    return 0;
}

void CPlayer::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CPlayer::Render_GameObject()
{
#pragma region 옵션
    // 행렬 적용
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());

    // 랜더 상태 옵션
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
#pragma endregion

#pragma region 왼손
    // 왼손 위치 설정
    m_pLeftHandComp->Readjust_Transform();
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pLeftHandComp->Get_Transform());

    // 왼손 출력 여부
    if (bLeftHandOn)
    {
        // 왼손 텍스처 출력
        m_pLeftHandComp->Render_Texture((_ulong)m_fLeftFrame, true);  // 왼손 텍스처 출력
        m_pBufferComp->Render_Buffer();                         // 왼손 버퍼 
    }
#pragma endregion

#pragma region 오른손
    // 오른손 위치 설정
    m_pRightHandComp->Readjust_Transform();
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pRightHandComp->Get_Transform());

    // 오른손 출력 여부
    if (bRightHandOn)
    {
        m_pRightHandComp->Render_Texture(m_fRightFrame, true);    // 오른손 텍스처 출력
        m_pBufferComp->Render_Buffer();                             // 오른손 버퍼
    }
#pragma endregion

#pragma region 초기 테스트 버전
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

#pragma region 옵션 끄기 

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
#pragma endregion
}

HRESULT CPlayer::Add_Component()
{
#pragma region 컴포넌트
    // 버퍼 컴포넌트
    NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
    // 행렬 컴포넌트
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    // 지형타기 컴포넌트
    NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculatorComponent>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);
    // 콜라이더 컴포넌트
    NULL_CHECK_RETURN(m_pColliderComp = Set_DefaultComponent_FromProto<CColliderComponent>(ID_DYNAMIC, L"Com_Collider", L"Proto_ColliderSphereComp"), E_FAIL);
    // 왼손 컴포넌트
    NULL_CHECK_RETURN(m_pLeftHandComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_TextureLeftHandTest", L"Proto_PlayerLeftTextureComp"), E_FAIL);
    // 오른손 컴포넌트
    NULL_CHECK_RETURN(m_pRightHandComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_TextureRightHandTest", L"Proto_PlayerRightTextureComp"), E_FAIL);
#pragma endregion

    // 텍스처 매니저
    // 왼손
    m_pLeftHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Left_Hand");
    // 오른손
    m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Right_Hand");

    // 물리 세계 등록
    m_pColliderComp->EnterToPhysics(0);
    // 충돌 함수 연결
    m_pColliderComp->Set_Collision_Event<ThisClass>(this, &ThisClass::OnCollision);
    m_pColliderComp->Set_CollisionEntered_Event<ThisClass>(this, &ThisClass::OnCollisionEntered);
    m_pColliderComp->Set_CollisionExited_Event<ThisClass>(this, &ThisClass::OnCollisionExited);
    // 충돌 레이어, 마스크 설정
    m_pColliderComp->Set_CollisionLayer(LAYER_PLAYER);
    m_pColliderComp->Set_CollisionMask(LAYER_MONSTER | LAYER_ITEM);
    // 플레이어가 shift로 대쉬를 하거나 공격을 했을때만 몬스터와 충돌이 허용됨. 

    return S_OK;
}

bool CPlayer::Keyboard_Input(const _float& fTimeDelta)
{
    _vec3	vLook;

    m_pTransformComp->Get_Info(INFO_LOOK, &vLook);

    vLook = { vLook.x, 0.f, vLook.z };

    _vec3 vInverse = (-m_pCamera->Get_At());

    m_pTransformComp->Compute_LootAtTarget(&vInverse);

#pragma region 키입력
    // 전진
    if (Engine::IsKey_Pressing(DIK_W))
    {
        // 뛰기
        if (Engine::IsKey_Pressing(DIK_LSHIFT))
        {
            // 전진 속도 Up
            fStraightSpeed = 10.f;
            // 플레이어 상태 : 달리기
            m_ePlayerState = STATE_PLAYER::RUN;
            m_tActionKey[EACTION_KEY::RUN].Act();
        }

        if (m_tActionKey[EACTION_KEY::RUN].IsOnAct())
        {
            // 조건 추가
            // Move 함수
        }

        D3DXVec3Normalize(&vLook, &vLook);
        m_pTransformComp->Move_Pos(&vLook, fTimeDelta, fStraightSpeed);
        // 전진 속도 복구
        fStraightSpeed = 5.f;
    }

    // 뛰기
    if (Engine::IsKey_Released(DIK_LSHIFT))
    {
        // 플레이어 상태 : 초기화
        m_ePlayerState = STATE_PLAYER::NONE;
    }

    // 후진
    if (Engine::IsKey_Pressing(DIK_S))
    {
        D3DXVec3Normalize(&vLook, &vLook);
        m_pTransformComp->Move_Pos(&vLook, fTimeDelta, -fSpeed);

        // S + Shift 뒷 대쉬
        if (Engine::IsKey_Pressed(DIK_LSHIFT))
        {
            fDownDash = 0.f;    // 대쉬 높이(카메라) 설정
            fDash = 20.f;       // 대쉬 값 설정
            bDashOn = true;     // 대쉬 On/Off
            m_eDashDir = DOWN;  // 대쉬 방향
        }
    }

    // 오른쪽
    if (Engine::IsKey_Pressing(DIK_D))
    {
        _vec3 vRight;
        m_pTransformComp->Get_Info(INFO_RIGHT, &vRight);
        D3DXVec3Normalize(&vRight, &vRight);
        m_pTransformComp->Move_Pos(&vRight, fTimeDelta, fSpeed);

        // fSpeed = 5.f , fDash = 20.f
        // D + Shift 우측 대쉬
        if (Engine::IsKey_Pressed(DIK_LSHIFT))
        {
            fDownDash = 0.f;    // 대쉬 높이(카메라) 설정
            fDash = 20.f;       // 대쉬 값 설정
            bDashOn = true;     // 대쉬 On/Off
            m_eDashDir = RIGHT; // 대쉬 방향
        }
    }

    // 왼쪽
    if (Engine::IsKey_Pressing(DIK_A))
    {
        _vec3 vRight;
        m_pTransformComp->Get_Info(INFO_RIGHT, &vRight);
        D3DXVec3Normalize(&vRight, &vRight);
        m_pTransformComp->Move_Pos(&-vRight, fTimeDelta, fSpeed);

        // A + Shift 좌측 대쉬
        if (Engine::IsKey_Pressed(DIK_LSHIFT))
        {
            fDownDash = 0.f;    // 대쉬 높이(카메라) 설정
            fDash = 20.f;       // 대쉬 값 설정
            bDashOn = true;     // 대쉬 On/Off
            m_eDashDir = LEFT;  // 대쉬 방향
        }
    }

    // 손
    if (Engine::IsKey_Pressed(DIK_1))
    {
        //Test
        m_eObjectType = OBJECT_TYPE::TWO_HAND;
        m_eObjectName = OBJECT_NAME::NONE;
    }

    // 권총
    if (Engine::IsKey_Pressed(DIK_2))
    {
        //Test
        m_eObjectType = OBJECT_TYPE::RIGHT_OBJECT;
        m_eObjectName = OBJECT_NAME::GUN;
    }

    // 톰슨 기관총
    if (Engine::IsKey_Pressed(DIK_3))
    {
        //Test
        m_eObjectType = OBJECT_TYPE::TWO_OBJECT;
        m_eObjectName = OBJECT_NAME::THOMPSON;
    }

    // 쇠파이프
    if (Engine::IsKey_Pressed(DIK_4))
    {
        //Test
        m_eObjectType = OBJECT_TYPE::RIGHT_OBJECT;
        m_eObjectName = OBJECT_NAME::STEELPIPE;
    }

    // 맥주병
    if (Engine::IsKey_Pressed(DIK_5))
    {
        //Test
        m_eObjectType = OBJECT_TYPE::RIGHT_OBJECT;
        m_eObjectName = OBJECT_NAME::BEERBOTLE;
    }

    // 프라이팬
    if (Engine::IsKey_Pressed(DIK_6))
    {
        //Test
        m_eObjectType = OBJECT_TYPE::RIGHT_OBJECT;
        m_eObjectName = OBJECT_NAME::FRYINGPAN;
    }

    // 점프
    if (Engine::IsKey_Pressed(DIK_SPACE))
    {

    }

    // 발차기
    if (Engine::IsKey_Pressed(DIK_Q))
    {
        m_ePlayerState = STATE_PLAYER::KICK;
        bRightFrameOn = true;
    }

    // 라이터
    if (Engine::IsKey_Pressed(DIK_V) && m_ePlayerState != STATE_PLAYER::RUN)
    {
        bLeftFrameOn = true;    // 프레임 재생

        if (!bRighter)  // 라이터가 꺼져있을 경우
        {
            bRighter = true;        // 라이터 켜주기
            //m_fLeftMaxFrame = 6.f;  // 최대 프레임 설정
            m_eLeftState = STATE_LEFTHAND::RIGHTER; // 왼손 상태 라이터로
        }
        else // 라이터가 켜져있을 경우
        {
            bBackRighter = true; // 라이터 되돌리기On
        }
    }

    // 오브젝트 버리기 (오브젝트가 있을 경우만 실행가능)
    if (Engine::IsKey_Pressed(DIK_F) && m_eObjectName != OBJECT_NAME::NONE)
    {
        // 플레이어 상태 오브젝트 버리는 중
        m_ePlayerState = STATE_PLAYER::THROW_AWAY;
        bRightFrameOn = true;
    }

    // 공격 프레임 확인
    if (Engine::IsKey_Pressed(DIK_0))
    {
        if (bDBugFrame)
        {
            bDBugFrame = false;
        }
        else
        {
            bDBugFrame = true;
        }
        
    }
    
    // 오브젝트 상호작용 // 소영 추가 
    /*if (Engine::IsKey_Pressed(DIK_E) && Picking_On_Object_Local())
    {
        int a = 0;
    }*/
#pragma endregion

    // 플레이어 기본 속도
    fSpeed = 5.f;

    return false;
}

void CPlayer::FrameManage(const _float& fTimeDelta)
{
#pragma region 왼손 프레임On
    if (bLeftFrameOn)
    {
        // 쉴드를 했을 경우
        if (!bRighter && bShieldOn)
        {
            m_fLeftFrame = fShieldFrame; // 쉴드 모션
        }
        else
        {
            // 라이터 되돌리기가 꺼져있을 경우
            if (!bBackRighter)
            {
                // 선형 보간
                //Interpolation();

                // 현재 시간
                fCurrentTime += fCurChangeTime * fTimeDelta;

                // 양손이 주먹 상태 일경우
                if (bLeftHandFist && bRightHandFist)
                {
                    // 왼손 프레임이 최대 프레임보다 클 경우
                    if (fCurrentTime > m_fLeftMaxFrame)
                    {
                        bLeftPunch = false;  // 왼손 주먹 Off
                        bRightPunch = true;    // 오른손 주먹 On
                    }
                }

                // 현재 프레임이 최대 프레임에 도달한 경우
                if (fCurrentTime >= m_fLeftMaxFrame)
                {
                    // 만약 최대프레임인데 라이터가 켜져있을 경우
                    if (bRighter)
                    {
                        // (현재 프레임) 라이터를 켜져있는 이미지로 고정
                        m_fLeftFrame = m_fLeftMaxFrame;
                        m_PlayerLighter->Set_m_bLightOn(true);
                    }
                    else // 라이터가 안켜져있을 경우
                    {
                        // 현재 프레임 초기화
                        m_fLeftFrame = 0.f;
                        //Get_Component();
                        // 왼손 프레임 Off
                        bLeftFrameOn = false;
                    }
                }
            }
            else // 라이터 되돌리기On
            {
                // 현재 프레임을 시간(프레임)마다 감소시키기
                m_fLeftFrame -= fCurChangeTime * fTimeDelta;

                // 왼손 프레임이 0에 도달했을 경우
                if (m_fLeftFrame <= 0.f)
                {
                    m_fLeftFrame = 0.f;
                    bRighter = false;
                    bLeftFrameOn = false;
                    bBackRighter = false;
                    m_PlayerLighter->Set_m_bLightOn(false);
                }
            }
        }
    }
#pragma endregion

#pragma region 오른손 프레임On (왼손과 동일)
    if (bRightFrameOn)
    {
        // 선형 보간
        //Interpolation();

        // 쉴드를 했을 경우
        if (bShieldOn)
        {
            m_fRightFrame = fShieldFrame; // 쉴드 모션
        }
        else // 쉴드를 안했을 경우
        {
            // 오른손 프레임 증가
            //Interpolation(m_fRightFrame);

            // 현재 시간
            fCurrentTime += fCurChangeTime * fTimeDelta;


            // 차징 공격이 켜지고, 현재 프레임이 풀 차징 프레임에 도달했을 경우
            if (bChargeAttack &&
                m_fRightFrame >= fFullChage)
            {
                // 현재 프레임을 풀 차징에 고정
                m_fRightFrame = fFullChage;
            }

            // 플레이어 발차기 중 프레임이 다 돌았을 경우
            if (m_ePlayerState == STATE_PLAYER::KICK &&
                fCurrentTime > m_fRightMaxFrame)
            {
                m_ePlayerState = STATE_PLAYER::NONE; // 플레이어 상태 초기화
                Two_Hand(); // 맨 주먹으로 돌아가기 (나중에 이전상태로 돌아가게 해야함)
            }

            //if (!timeline.empty())
            //{
            //    // 현재 시간이 최대 변경시간 보다 크거나 같을 경우(변경하고 싶은 시간이 됐을 때)
            //    if (fCurrentTime >= timeline[timeline.size() - 1].time)
            //    {
            //        // 플레이어 상태 초기화
            //        m_ePlayerState = STATE_PLAYER::NONE;
            //        fCurrentTime = 0.f; // 텍스처 변경시간 초기화
            //        m_fRightFrame = 0.f; // 오른손 프레임 초기화
            //        bRightFrameOn = false; // 오른손 프레임Off
            //    }
            //}

            if (!timeline[KEYTYPE_END].empty())
            {
                // 오른손 프레임이 최대 프레임에 도달했을 경우
                if (fCurrentTime >= m_fRightMaxFrame)
                {
                    // 만약 최대프레임인데 버리는 중일 경우
                    if (m_ePlayerState == STATE_PLAYER::THROW_AWAY)
                    {
                        // (현재 프레임) 오른손을 Throw 이미지로 고정
                        m_fRightFrame = m_fRightMaxFrame;

                        // 시간마다 텍스처 변경 시간을 채워준다.
                        fCurrentTime += fCurChangeTime * fTimeDelta;
                    }
                    else // 버리는 중이 아닐 경우
                    {
                        // 플레이어 상태가 차징일 경우
                        if (m_ePlayerState == STATE_PLAYER::CHARGING)
                        {
                            m_ePlayerState = STATE_PLAYER::NONE; // 플레이어 상태 : 초기화
                            fChageTime = 0.f; // 차징 시간 초기화
                        }
                        else
                        {
                            // 양손이 주먹 상태 일경우
                            if (bLeftHandFist && bRightHandFist)
                            {
                                // 오른손 프레임이 다 돌았을 경우
                                if (fCurrentTime > m_fRightMaxFrame)
                                {
                                    bRightPunch = false;  // 오른손 주먹 Off
                                    bLeftPunch = true;    // 왼손 주먹 On
                                }
                            }
                        }

                        if (!bSpinOn)
                        {
                            // 오른손 프레임 초기화
                            m_fRightFrame = 0;
                            fCurrentTime = 0.f;
                            bRightFrameOn = false; // 오른손 프레임Off
                        }

                        //// 권총이 회전중이였을 때
                        //if (bSpinOn)
                        //{
                        //    // 회전 Off
                        //    bSpinOn = false;

                        //    // 권총으로 다시 돌아가기
                        //    bGunOn = true;

                        //    // 최대 프레임을 권총 기준으로 다시 맞춰놓기
                        //    m_fRightMaxFrame = 4.f;
                        //}
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

    // 차징 전환 시간 누적
    fChageTime += 10.f * fTimeDelta;

    // 시간 초과시 차징모드
    if (fChageTime >= fFullChargeTime)
    {

        switch (m_eObjectName)
        {
            case CPlayer::OBJECT_NAME::NONE:
            {
                m_ePlayerState = STATE_PLAYER::CHARGING; // 플레이어 상태 : 차징
                fFullChage = 0.f;       // 풀차징 프레임
                bChargeAttack = true;   // 차징공격On
                bRightFrameOn = true;   // 오른손 프레임On
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
            case CPlayer::OBJECT_NAME::STEELPIPE: // 쇠파이프
            {
                m_ePlayerState = STATE_PLAYER::CHARGING; // 플레이어 상태 : 차징
                fFullChage = 1.f;       // 풀차징 프레임
                bChargeAttack = true;   // 차징공격On
                bRightFrameOn = true;   // 오른손 프레임On
                break;
            }
            case CPlayer::OBJECT_NAME::BEERBOTLE:
            {

                break;
            }
            case CPlayer::OBJECT_NAME::FRYINGPAN: // 프라이팬
            {
                m_ePlayerState = STATE_PLAYER::CHARGING; // 플레이어 상태 : 차징
                fFullChage = 1.f;       // 풀차징 프레임
                bChargeAttack = true;   // 차징공격On
                bRightFrameOn = true;   // 오른손 프레임On
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

    _vec3 PickingPos = m_pCalculatorComp->Picking_On_Object_Local(g_hWnd, pObjectBufferCom, pObjectTransCom);

    if (PickingPos)
        return true;
    else
        return false;
}

void CPlayer::OnCollision(CGameObject* pDst)
{
    // 충돌중일때
    OutputDebugString(L"플레이어와 충돌중\n");
}

void CPlayer::OnCollisionEntered(CGameObject* pDst)
{
    // 처음 충돌했을때
    OutputDebugString(L"플레이어와 충돌진입\n");
}

void CPlayer::OnCollisionExited(CGameObject* pDst)
{
    // 충돌에서 나갈때
    OutputDebugString(L"플레이어와 충돌완료\n");
}

void CPlayer::Update_BlackBoard()
{
    // 블랙보드 연결 대기, 안전 코드로 필수
    if (!m_wpBlackBoard_Player.Get_BlackBoard())
    {
        m_wpBlackBoard_Player.Set_BlackBoard(Engine::Get_BlackBoard(L"Player"));
        // 연결 실패
        if (!m_wpBlackBoard_Player.Get_BlackBoard())
            return;
    }

    // 안전 코드를 거치면 일반 포인터로 접근 허용.
    CBlackBoard_Player* pBlackBoard = m_wpBlackBoard_Player.Get_BlackBoard();

    // 여기서부터 블랙보드의 정보를 업데이트 한다.
    pBlackBoard->Get_HP().Cur = m_gHp.Cur;

}

bool CPlayer::Attack_Input(const _float& fTimeDelta)
{
    // 발차기
    //if (Engine::Get_DIKeyState(DIK_Q) & 0x80)
    //{

    //    bFootAttack = true;
    //    return bFootAttack;
    //}

#pragma region 마우스

    // 마우스 좌클릭 (누르고 있을 때)
    if (Engine::IsMouse_Pressing(DIM_LB))
    {
        //Charge(fTimeDelta);
    }

    // 차징이 안켜졌을 때
    if (!bChargeAttack)
    {
        // 마우스 좌클릭 (누를 때)
        if (Engine::IsMouse_Pressed(DIM_LB))
        {
            // 양손 다 주먹상태일 경우
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
            else // 나머지
            {
                bRightFrameOn = true;
                
                // 권총이면 총알 발사
                if (m_tRightHand_State.Get_State() == STATE_RIGHTHAND::GUN ||
                    m_tRightHand_State.Get_State() == STATE_RIGHTHAND::THOMPSON)
                {
                    // 총알 발사 (디바이스, 생성 위치, 투사체 속도)
                    Engine::Add_GameObject(L"GameLogic", CPlayerBullet::Create(m_pGraphicDev,
                        m_pTransformComp->Get_Pos(), 300.f));
                }
            }
        }
    }

    // 마우스 좌클릭 (뗄 때)
    if (Engine::IsMouse_Released(DIM_LB))
    {
        bChargeAttack = false;  // 차징공격Off
    }

    // 마우스 우클릭
    if (Engine::IsMouse_Pressing(DIM_RB))
    {
        // 쉴드 가능 일때
        if (bShield)
        {
            // 현재 프레임이 0일 때
            if (m_fRightFrame == 0)
            {
                // 쉴드On
                bShieldOn = true;
                // 오른손 프레임On
                bRightFrameOn = true;

                // 왼손이 주먹일 경우
                if (bLeftHandFist)
                    bLeftFrameOn = true; // 왼손 프레임On
            }
        }
    }
    else
    {
        // 쉴드Off
        bShieldOn = false;
    }
#pragma endregion

    return false;
}

// 마우스 움직임
void CPlayer::Mouse_Move()
{
#pragma region 1인칭
    if (m_pCamera->Get_One())
    {
        //m_pTransformComp->Rotate(ROT_X, m_pCamera->Get_At());

        //마우스로 플레이어 회전
        //상, 하
        if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_Y))
        {
            m_pTransformComp->Rotate(ROT_X, D3DXToRadian(dwMouseMove / 10.f));
        }
        // 좌, 우
        if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_X))
        {
            m_pTransformComp->Rotate(ROT_Y, D3DXToRadian(dwMouseMove / 10.f));
        }
    }
#pragma endregion

#pragma region 3인칭 쿼터니언
    if (m_pCamera->Get_Three())
    {
        _vec3   CameraToPlayer;

        m_pTransformComp->Get_Info(INFO_POS, &m_vPlayerPos);

        CameraToPlayer = m_vPlayerPos - m_pCamera->Get_Eye();

        // 카메라가 바라보는 At
        //m_vCameraAt = m_pCamera->Get_At();

        // 플레이어가 바라보는 Look
        //m_pTransformComp->Get_Info(INFO_LOOK, &m_vPlayerLook);

        // 카메라와 플레이어가 바라보는 높이를 똑같이 맞춰준다.
        CameraToPlayer.y = 0.f;
        //m_vPlayerLook.y = 0.f;

        // 카메라와 플레이어 벡터를 정규화
        D3DXVec3Normalize(&CameraToPlayer, &CameraToPlayer);
        D3DXVec3Normalize(&vLook, &vLook);

        float fAngle;


        if (CameraToPlayer.x > 0)
        {
            // 내적을 이용해 둘 사이 끼인각을 구해준다.
            fDot = D3DXVec3Dot(&CameraToPlayer, &vLook);
            // 두 벡터 사이의 각도 계산 (라디안)
            fAngle = acosf(fDot);
        }
        else
        {
            //vLook = vLook * -1;
            // 내적을 이용해 둘 사이 끼인각을 구해준다.
            fDot = D3DXVec3Dot(&CameraToPlayer, &(vLook * -1));
            // 두 벡터 사이의 각도 계산 (라디안)
            fAngle = acosf(fDot);

            fAngle = fAngle + 3.141592f;
        }

        m_pTransformComp->Set_ScaleY(fAngle);
    }
#pragma endregion

#pragma region 1인칭 쿼터니언 회전 (폐기)
    //dwMouseMoveX = Engine::Get_DIMouseMove(DIMS_X);
    //dwMouseMoveY = Engine::Get_DIMouseMove(DIMS_Y);

    //D3DXQUATERNION quatPitch, quatYaw, quatRoll;

    //// 플레이어의 Up 벡터를 가져온다.
    //m_pTransformComp->Get_Info(INFO_UP, &m_vUp);

    //// 플레이어의 Look 벡터를 가져온다.
    //m_pTransformComp->Get_Info(INFO_LOOK, &m_vLook);

    //// 가져온 플레이어의 Up, Look 벡터로 Right벡터 구하기
    //D3DXVec3Cross(&m_vRight, &m_vUp, &m_vLook);

    //// 수평 회전 (Y축 회전)
    //if (dwMouseMoveX != 0)
    //{
    //    // 마우스 X 이동에 따라 Y축 주위로 회전하는 쿼터니언 계산
    //    D3DXQuaternionRotationAxis(&quatYaw, &m_vUp, D3DXToRadian(static_cast<_float>(dwMouseMoveX) * 1.f));

    //    // 현재 쿼터니언에 새로운 회전을 곱하여 누적시킨다.
    //    m_quaternion *= quatYaw;

    //    // 누적된 회전 쿼터니언 정규화
    //    D3DXQuaternionNormalize(&m_quaternion, &m_quaternion);
    //}

    //// 수직 회전 (X축 회전)
    //if (dwMouseMoveY != 0)
    //{
    //    // 마우스 X 이동에 따라 Y축 주위로 회전하는 쿼터니언 계산
    //    D3DXQuaternionRotationAxis(&quatPitch, &m_vRight, D3DXToRadian(static_cast<_float>(dwMouseMoveY) * 1.f));

    //    // 현재 쿼터니언에 새로운 회전을 곱하여 누적시킨다.
    //    m_quaternion *= quatPitch;

    //    // 누적된 회전 쿼터니언 정규화
    //    D3DXQuaternionNormalize(&m_quaternion, &m_quaternion);
    //}

    //// 쿼터니언을 사용하여 변환 행렬을 계산하기
    //D3DXMATRIX matRotation;
    //D3DXMatrixRotationQuaternion(&matRotation, &m_quaternion);

    //// 행렬을 플레이어의 변환에 적용하기.
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

    _float	fHeight = m_pCalculatorComp->
        Compute_HeightOnTerrain(&vPos, 
                                pTerrainBufferComp->Get_VtxPos(), 
                                pTerrainBufferComp->Get_VertexCountX() + 1U, 
                                pTerrainBufferComp->Get_VertexCountZ() + 1U,
                                pTerrainBufferComp->Get_Scale(),
                                pTerrainBufferComp->Get_InvOffset());

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

    // 대쉬 여부
    if (bDashOn)
    {
        // 대쉬 높이 최대치
        if (fDownDash >= 40.f)
        {
            bDashChange = true; // 감소 시작
        }

        if (bDashChange)
        {
            fDownDash--; // 다운 수치 감소
        }
        else
        {
            fDownDash++; // 다운 수치 증가
        }

        // 대쉬 높이
        m_pTransformComp->Set_MovePos(fTimeDelta, vSpeed);

        // 대쉬 속도가 스피드보다 빠를 경우만 작동
        if (fDash > fSpeed)
        {
            // 대쉬 방향 및 이동
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
            fDash--; // 대쉬 속도 감소
        }
        else // 대쉬 속도가 스피드와 같으면
        {

        }

        if (fDownDash <= 0.f)
        {
            bDashChange = false;
            bDashOn = false; // 대쉬 정지
        }
    }

}

#pragma region 양손 주먹 (기본 상태)
void CPlayer::Two_Hand()
{
    // 최대 프레임 설정
    //m_fLeftMaxFrame = 2.f;
    //m_fRightMaxFrame = 2.f;

    bChargingReady = true; // 차징 가능
    m_eObjectName = OBJECT_NAME::NONE; // 오브젝트 없음

    // 오른손 주먹
    m_eRIGHTState = STATE_RIGHTHAND::HAND;
    m_tRightHand_State.Set_State(STATE_RIGHTHAND::HAND);
    bRightHandFist = true; // 오른손 주먹 상태On

    // 라이터를 안켰을 경우
    if (!bRighter)
    {
        // 왼손 주먹
        m_tLeftHand_State.Set_State(STATE_LEFTHAND::HAND);
        bLeftHandFist = true; // 왼손 주먹 상태On
    }

}
#pragma endregion

#pragma region 양손 오브젝트
void CPlayer::Two_Object()
{
    // 양손 주먹 상태Off
    bRightHandFist = false;
    bLeftHandFist = false;

    // 라이터가 켜져있는 상태로 양손 오브젝트에 들어왔을 경우
    if (bRighter)
    {
        // 왼손 현재 프레임 초기화
        m_fLeftFrame = 0.f;
        bRighter = false;       // 라이터 끄기
    }

    bRighterSwitch = false; // 라이터 스위치 정지

    switch (m_eObjectName)
    {
        // 톰슨 기관총
    case CPlayer::OBJECT_NAME::THOMPSON:
    {
        // 오른손 톰슨 기관총 (양손)
        m_eRIGHTState = STATE_RIGHTHAND::THOMPSON;
        m_tRightHand_State.Set_State(STATE_RIGHTHAND::THOMPSON);
        // 왼손 없음
        m_tLeftHand_State.Set_State(STATE_LEFTHAND::NONE);
        break;
    }
    }
}
#pragma endregion

#pragma region 한손 오브젝트
void CPlayer::Right_Object()
{
    // 양손 주먹 상태Off
    bRightHandFist = false;
    bLeftHandFist = false;

    // 오른손
    switch (m_eObjectName)
    {
        // 권총
        case CPlayer::OBJECT_NAME::GUN:
        {
            // 오른손 권총 (한손)
            m_eRIGHTState = STATE_RIGHTHAND::GUN;
            m_tRightHand_State.Set_State(STATE_RIGHTHAND::GUN);
            // 왼손 없음
            m_eLeftState = STATE_LEFTHAND::NONE;
            // 차징 불가
            bChargingReady = false;
            break;
        }
        // 쇠파이프
        case CPlayer::OBJECT_NAME::STEELPIPE:
        {
            // 오른손 쇠파이프
            m_eRIGHTState = STATE_RIGHTHAND::STEELPIPE;
            m_tRightHand_State.Set_State(STATE_RIGHTHAND::STEELPIPE);
            // 왼손 오픈 핸드
            m_eLeftState = STATE_LEFTHAND::OPEN_HAND;
            // 차징 가능
            bChargingReady = true;
            break;
        }
        // 맥주병
        case CPlayer::OBJECT_NAME::BEERBOTLE:
        {
            // 오른손 맥주병
            m_eRIGHTState = STATE_RIGHTHAND::BEERBOTLE;
            m_tRightHand_State.Set_State(STATE_RIGHTHAND::BEERBOTLE);
            // 왼손 오픈 핸드
            m_eLeftState = STATE_LEFTHAND::OPEN_HAND;
            // 차징 불가
            bChargingReady = false;
            break;
        }
        // 프라이팬
        case CPlayer::OBJECT_NAME::FRYINGPAN:
        {
            // 오른손 프라이팬
            m_eRIGHTState = STATE_RIGHTHAND::FRYINGPAN;
            m_tRightHand_State.Set_State(STATE_RIGHTHAND::FRYINGPAN);
            // 왼손 오픈 핸드
            m_eLeftState = STATE_LEFTHAND::OPEN_HAND;
            // 차징 가능
            bChargingReady = true;
            break;
        }
    }

    // 왼손
    if (!bRighter) // 라이터를 안켰을 경우
    {
        switch (m_eLeftState) // 현재 왼손 상태
        {
        case CPlayer::STATE_LEFTHAND::NONE: // 왼손 없음
        {
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::NONE);
            break;
        }
        case CPlayer::STATE_LEFTHAND::HAND: // 왼손 주먹
        {
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::HAND);
            break;
        }
        case CPlayer::STATE_LEFTHAND::OPEN_HAND: // 왼손 오픈 핸드
        {
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::OPEN_HAND);
            break;
        }
        }
    }
}
#pragma endregion

// 손 상태 확인
void CPlayer::Hand_Check()
{
    // 왼손 오른손 출력 On
    bLeftHandOn = true;
    bRightHandOn = true;
    
    // 플레이어의 손이 기존 상태가 아닐경우
    if (m_tRightState_Old.Get_State() != m_tRightHand_State.Get_State())
    {
        bGetAnimation = true; // 새로운 애니메이션 불러오기 On
        
        // 새로운 상태가 권총일 경우
        if (m_tRightHand_State.Get_State() == STATE_RIGHTHAND::GUN)
        {
            bRightFrameOn = true; // 오른손 프레임On
            bSpinOn = true; // 회전On
        }
    }

    // 플레이어의 현재 상태를 저장
    m_tRightState_Old.Set_State(m_tRightHand_State.Get_State());

    // 플레이어가 안뛰고있는 경우
    if (m_ePlayerState != STATE_PLAYER::RUN)
    {
        // 플레이어가 오브젝트를 버리고있을 경우
        if (m_ePlayerState == STATE_PLAYER::THROW_AWAY)
        {
            // 오브젝트 초기화 (삭제)
            m_eObjectType = OBJECT_TYPE::NONE;
            m_eObjectName = OBJECT_NAME::NONE;
        }

        if (bRighter) // 플레이어가 라이터를 킨 경우
        {
            // 왼손 라이터
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::RIGHTER);
        }

#pragma region 오브젝트 타입 체크
        if (m_ePlayerState != STATE_PLAYER::KICK) // 플레이어가 발차기를 안한 경우
        {
            // 오브젝트가 NONE일 경우
            if (m_eObjectType == OBJECT_TYPE::NONE ||
                m_eObjectName == OBJECT_NAME::NONE)
            {
                m_eObjectType = OBJECT_TYPE::TWO_HAND; // 맨손
            }

            switch (m_eObjectType)
            {
            case CPlayer::OBJECT_TYPE::TWO_HAND:        // 양손 주먹
            {
                Two_Hand();
                break;
            }
            case CPlayer::OBJECT_TYPE::TWO_OBJECT:      // 양손 오브젝트 (두손 무기)
            {
                Two_Object();
                break;
            }
            case CPlayer::OBJECT_TYPE::RIGHT_OBJECT:    // 한손 오브젝트 (한손 무기)
            {
                Right_Object();
                break;
            }
            }
        }
        else if (m_ePlayerState == STATE_PLAYER::KICK) // 플레이어가 발차기를 한 경우
        {
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::NONE); // 왼손Off
            m_tRightHand_State.Set_State(STATE_RIGHTHAND::KICK); // 오른손 발차기
        }
#pragma endregion
    }
#pragma region 플레이어가 뛰고 있는 경우
    else if (m_ePlayerState == STATE_PLAYER::RUN)
    {
            m_eRIGHTState = STATE_RIGHTHAND::RUN_HAND;
        // 라이터를 켠 채로 뛰는 경우
        if (bRighter)
        {
            // 왼손 라이터 달리기
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::RUN_RIHGTER);
            // 오른손 맨손 달리기
            m_tRightHand_State.Set_State(STATE_RIGHTHAND::RUN_HAND);
        }
        // 라이터를 키지 않은 경우
        else
        {
            // 양손 다 맨손 달리기
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::RUN_HAND);
            m_tRightHand_State.Set_State(STATE_RIGHTHAND::RUN_HAND);
        }
    }
#pragma endregion

}

void CPlayer::State_Update(float fTimeDelta)
{
    // 플레이어가 살아있을 때
    if (!bDead)
    {
        // 플레이어의 상태
        m_tPlayer_State.Get_StateFunc()(this, fTimeDelta);

        // 플레이어 손 상태 체크
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

#pragma region 플레이어의 왼손 상태

void CPlayer::Left_None(float fTimeDelta)
{
    if (m_tLeftHand_State.IsState_Entered())
    {
        // 왼손 출력 Off
        bLeftHandOn = false;
        // 왼손 현재 프레임 초기화
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
        // 기본 왼손 출력
        m_pLeftHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Left_Hand");
        //m_fLeftMaxFrame = 2.f;  // 최대 프레임 설정
        bRighter = false;       // 라이터 Off

        // 애니메이션 불러오기
        if (bGetAnimation)
        {
            LeftLoadAnimationFromFile("LeftFist");
            //bGetAnimation = false; // Off
        }
    }

    if (m_tLeftHand_State.Can_Update())
    {
        // 플레이어가 버리는 중 일경우
        if (m_ePlayerState == STATE_PLAYER::THROW_AWAY)
        {
            bLeftHandOn = false; // 왼손Off
        }
    }

    if (m_tLeftHand_State.IsState_Exit())
    {

    }
}

// 왼손 달리기 라이터.ver
void CPlayer::Left_RunRighter(float fTimeDelta)
{
    if (m_tLeftHand_State.IsState_Entered())
    {
        // 라이터On (최대프레임) 그대로, 툴로 위 아래 움직이게 만들어주기
    }

    if (m_tLeftHand_State.Can_Update())
    {

    }

    if (m_tLeftHand_State.IsState_Exit())
    {

    }
}

// 한손 오브젝트일때 왼손 오픈 핸드
void CPlayer::Left_OpenHand(float fTimeDelta)
{
    if (m_tLeftHand_State.IsState_Entered())
    {
        // 왼손 오픈 핸드로 변경
        m_pLeftHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"OpenHand");

        //bGetAnimation = false;
        bLeftFrameOn = false;
        m_fLeftFrame = 0.f;
        m_fLeftMaxFrame = 0.f;
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
        // 왼손 뛰는 손으로 변경
        m_pLeftHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Left_RunHand");
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
        m_pLeftHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Righter");
        
        // 애니메이션 불러오기
        if (bGetAnimation)
        {
            LeftLoadAnimationFromFile("Righter");
            bGetAnimation = false; // Off
        }
    }

    if (m_tLeftHand_State.Can_Update())
    {
        // 현재 프레임이 최대 프레임보다 크거나 같을 경우(애니메이션을 다 돌았을 때)
        if (fCurrentTime >= m_fLeftMaxFrame)
        {
            if (bRighter)
            {
                // 프레임 재생 Off
                bLeftFrameOn = false;
                fCurrentTime = m_fLeftMaxFrame;
            }
            else // 라이터가 꺼졌을 경우
            {
                if (m_fLeftFrame <= 0) // 모두 감소했을 경우
                {
                    // 왼손 주먹
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

#pragma region 플레이어의 오른손 상태

void CPlayer::Right_None(float fTimeDelta)
{
    if (m_tRightHand_State.IsState_Entered())
    {
        // 오른손 출력 Off
        bRightHandOn = false;
        bShield = false; // 방어 불가
        // 오른손 현재 프레임 초기화
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
        // 기본 오른손 출력
        m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Right_Hand");
        //m_fRightMaxFrame = 2.f; // 최대 프레임 설정
        //fRightFrameSpeed = 10.f;// 프레임 속도 지정 (공격 속도)
        bShield = true;         // 방어 가능

        // 애니메이션 불러오기
        if (bGetAnimation)
        {
            RightLoadAnimationFromFile("RightFist");
            //bGetAnimation = false; // Off
        }
    }
    
    if (m_tRightHand_State.Can_Update())
    {
        // 플레이어가 차징 상태일 경우
        if (m_ePlayerState == STATE_PLAYER::CHARGING)
        {
            // 차징 텍스처로 변경
            m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"RightHand_Charging");

            // 불러오기 On
            bGetAnimation = true;
            // 방어 불가능
            bShield = false;

            // 애니메이션 불러오기
            if (bGetAnimation)
            {
                RightLoadAnimationFromFile("RightHand_Charging");
                bGetAnimation = false; // Off
            }

        }
        // 플레이어가 버리는 중 일경우
        if (m_ePlayerState == STATE_PLAYER::THROW_AWAY)
        {
            // 버리는 텍스처로 변경
            m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"UnderThrow_RightHand");
            //m_fRightMaxFrame = 0.f;  // 최대 프레임 지정
            fRightFrameSpeed = 4.f;  // 프레임 속도 지정 (공격 속도)
            bShield = false;         // 방어 불가능
        }

        // 현재 텍스처 변경 시간이 최대 변경시간 보다 크거나 같을 경우(변경하고 싶은 시간이 됐을 때)
        if (fCurrentTime >= fMaxChangeTime)
        {
            // 플레이어 상태 초기화
            m_ePlayerState = STATE_PLAYER::NONE;
            //fCurrentTime = 0.f; // 텍스처 변경시간 초기화

            // 불러오기 On
            bGetAnimation = true;
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
        // 오른손 뛰는 손으로 변경
        m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Right_RunHand");
        bShield = false; // 방어 불가
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
        bShield = false;        // 방어 불가

        // 총 회전이 켜질 경우
        if (bSpinOn)
        {
            //// 불러오기 ON
            //bGetAnimation = true;

            // 오른손 총 회전 이미지
            m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Gun_Spin");

            // 애니메이션 불러오기
            if (bGetAnimation)
            {
                RightLoadAnimationFromFile("GunSpin");
                bGetAnimation = false; // Off
            }
        }

    }

    if (m_tRightHand_State.Can_Update())
    {
        // 총 회전이 다 돌았을 경우
        if (fCurrentTime >= m_fRightMaxFrame)
        {
            // 오른손 총 불러오기
            m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Gun");
            RightLoadAnimationFromFile("Gun");
            fCurChangeTime = 1.5f; // 프레임 속도

            bSpinOn = false; // 총 회전 끄기
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
        // 오른손 톰슨 기관총
        m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Thompson");
        //m_fRightMaxFrame = 4.f; // 최대 프레임 지정
        //fRightFrameSpeed = 35.f;// 프레임 속도 지정 (공격 속도)
        bShield = false;        // 방어 불가

        // 애니메이션 불러오기
        if (bGetAnimation)
        {
            RightLoadAnimationFromFile("Thompson");
            bGetAnimation = false; // Off
        }
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
        // 오른손 쇠파이프
        m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Steel_Pipe");
        //m_fRightMaxFrame = 4.f; // 최대 프레임 지정
        //fRightFrameSpeed = 5.f;// 프레임 속도 지정 (공격 속도)
        bShield = true;         // 방어 가능

        // 애니메이션 불러오기
        if (bGetAnimation)
        {
            RightLoadAnimationFromFile("Steel_Pipe");
            bGetAnimation = false; // Off
        }
        
    }

    if (m_tRightHand_State.Can_Update())
    {
        // 플레이어가 차징 상태일 경우
        if (m_ePlayerState == STATE_PLAYER::CHARGING)
        {
            // 차징 텍스처로 변경
            m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Steel_Pipe_Charging");
            //m_fRightMaxFrame = 5.f; // 최대 프레임 지정
            //fRightFrameSpeed = 8.f;// 프레임 속도 지정 (공격 속도)
            bShield = false;         // 방어 불가능

            //// 애니메이션 불러오기
            //if (bGetAnimation)
            //{
                //LoadAnimationFromFile("Steel_Pipe_Charging");
                //bGetAnimation = false; // Off
            //}
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
        // 오른손 맥주병
        m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"BeerBottle");
        //m_fRightMaxFrame = 5.f; // 최대 프레임 지정
        //fRightFrameSpeed = 10.f;// 프레임 속도 지정 (공격 속도)
        // 애니메이션 불러오기
        if (bGetAnimation)
        {
            RightLoadAnimationFromFile("Steel_Pipe");
            bGetAnimation = false; // Off
        }
        bShield = false;        // 방어 불가
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
        // 오른손 프라이팬
        m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"FryingPan");
        //m_fRightMaxFrame = 5.f; // 최대 프레임 지정
        //fRightFrameSpeed = 10.f;// 프레임 속도 지정 (공격 속도)
        bShield = true;         // 방어 가능

        // 애니메이션 불러오기
        if (bGetAnimation)
        {
            RightLoadAnimationFromFile("FryingPan");
            bGetAnimation = false; // Off
        }
    }

    if (m_tRightHand_State.Can_Update())
    {
        // 플레이어가 차징 상태일 경우
        if (m_ePlayerState == STATE_PLAYER::CHARGING)
        {
            // 차징 텍스처로 변경
            m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"FryingPan_Charging");
            //m_fRightMaxFrame = 4.f; // 최대 프레임 지정
            fRightFrameSpeed = 9.f;// 프레임 속도 지정 (공격 속도)
            bShield = false;         // 방어 불가능
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
        // 오른손 발차기
        m_eRIGHTState = STATE_RIGHTHAND::KICK;
        m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Kick");
        //m_fRightMaxFrame = 4.f; // 최대 프레임 지정
        fRightFrameSpeed = 7.f; // 프레임 속도 지정 (공격 속도)
        bShield = false;        // 방어 불가
    }

    if (m_tRightHand_State.Can_Update())
    {

    }

    if (m_tRightHand_State.IsState_Exit())
    {

    }
}

#pragma endregion

// 왼손 애니메이션 불러오기
void CPlayer::LeftLoadAnimationFromFile(const char* fileName)
{
    // .dat 파일 확장자를 추가한 파일 이름 생성
    std::string datFileName = "../Data/" + std::string(fileName) + ".dat";

    // 파일을 UTF-8로 열기
    std::ifstream file(datFileName.c_str(), std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        // 파일을 열 수 없을 때의 오류 처리
        return;
    }

    timeline[KEYTYPE_LEFTHAND].clear();
    KEYFRAME keyframe;

    while (file >>
        keyframe.time >>
        keyframe.value >>
        keyframe.type >>
        keyframe.isEaseIn >>
        keyframe.isEaseOut >>
        keyframe.bChargePossible >>
        keyframe.bShieldPossible >>
        keyframe.texureframe >>
        keyframe.iFullChargeFrame >>
        keyframe.iShieldFrame >>
        keyframe.vScale.x >> keyframe.vScale.y >> keyframe.vScale.z >>
        keyframe.vRot.x >> keyframe.vRot.y >> keyframe.vRot.z >>
        keyframe.vPos.x >> keyframe.vPos.y >> keyframe.vPos.z >>
        keyframe.vKeyFramePos.x >> keyframe.vKeyFramePos.y)
    {
        timeline[KEYTYPE_LEFTHAND].push_back(keyframe);
    }



    file.close();

#pragma region 마지막 키프레임으로 설정할 값들
    // 최대 프레임 설정
    m_fLeftMaxFrame = timeline[KEYTYPE_LEFTHAND].back().time;
#pragma endregion  
}

// 오른손 애니메이션 불러오기
void CPlayer::RightLoadAnimationFromFile(const char* fileName)
{
    // .dat 파일 확장자를 추가한 파일 이름 생성
    std::string datFileName = "../Data/" + std::string(fileName) + ".dat";

    // 파일을 UTF-8로 열기
    std::ifstream file(datFileName.c_str(), std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        // 파일을 열 수 없을 때의 오류 처리
        return;
    }

    timeline[KEYTYPE_RIGHTHAND].clear();
    KEYFRAME keyframe;

    while (file >>
        keyframe.time >>
        keyframe.value >>
        keyframe.type >>
        keyframe.isEaseIn >>
        keyframe.isEaseOut >>
        keyframe.bChargePossible >>
        keyframe.bShieldPossible >>
        keyframe.texureframe >>
        keyframe.iFullChargeFrame >>
        keyframe.iShieldFrame >>
        keyframe.vScale.x >> keyframe.vScale.y >> keyframe.vScale.z >>
        keyframe.vRot.x >> keyframe.vRot.y >> keyframe.vRot.z >>
        keyframe.vPos.x >> keyframe.vPos.y >> keyframe.vPos.z >>
        keyframe.vKeyFramePos.x >> keyframe.vKeyFramePos.y)
    {
        timeline[KEYTYPE_RIGHTHAND].push_back(keyframe);
    }



    file.close();

#pragma region 마지막 키프레임으로 설정할 값들
    // 최대 프레임 설정
    m_fRightMaxFrame = timeline[KEYTYPE_RIGHTHAND].back().time;
#pragma endregion  
}

void CPlayer::Interpolation() // 왼손, 오른손 선형 보간 함수 별개로 만들기
{
    for (int i = 0; i < KEYTYPE_END; i++) // 키타입 순회
    {
        if (!timeline[i].empty()) // 해당 키타입이 비었는지 체크
        {
            //if (bRightHandOn) // 오른손 출력 On
            {
                if (fCurrentTime >= 0.f &&
                    fCurrentTime <= timeline[i].back().time)
                {
                    // 프레임 0으로 초기화
                    _uint iFrameIndex = 0U;

                    // 사이즈의 끝에서부터 시작해서 찾기
                    for (_uint j = timeline[i].size() - 1; j >= 0; j--)
                    {
                        if ((timeline)[i][j].time <= fCurrentTime)
                        {
                            iFrameIndex = j;
                            break;
                        }
                    }

                    // Linear
                    if (iFrameIndex + 1U < timeline[i].size())
                    {
                        // 키 프레임간 시간 변화율
                        fFrameTimeDelta = (timeline)[i][iFrameIndex + 1U].time - (timeline)[i][iFrameIndex].time;

                        // 현재 키 프레임시간부터 현재 시간 변화율
                        fCurFrameTimeDelta = (fCurrentTime - (timeline)[i][iFrameIndex].time);

                        fSizeX_Delta = (timeline)[i][iFrameIndex + 1U].vScale.x - (timeline)[i][iFrameIndex].vScale.x;
                        fSizeX_Delta *= fCurFrameTimeDelta / fFrameTimeDelta;
                        fSizeY_Delta = (timeline)[i][iFrameIndex + 1U].vScale.y - (timeline)[i][iFrameIndex].vScale.y;
                        fSizeY_Delta *= fCurFrameTimeDelta / fFrameTimeDelta;

                        fRotX_Delta = (timeline)[i][iFrameIndex + 1U].vRot.x - (timeline)[i][iFrameIndex].vRot.x;
                        fRotX_Delta *= fCurFrameTimeDelta / fFrameTimeDelta;
                        fRotY_Delta = (timeline)[i][iFrameIndex + 1U].vRot.y - (timeline)[i][iFrameIndex].vRot.y;
                        fRotY_Delta *= fCurFrameTimeDelta / fFrameTimeDelta;
                        fRotZ_Delta = (timeline)[i][iFrameIndex + 1U].vRot.z - (timeline)[i][iFrameIndex].vRot.z;
                        fRotZ_Delta *= fCurFrameTimeDelta / fFrameTimeDelta;

                        fPosX_Delta = (timeline)[i][iFrameIndex + 1U].vPos.x - (timeline)[i][iFrameIndex].vPos.x;
                        fPosX_Delta *= fCurFrameTimeDelta / fFrameTimeDelta;
                        fPosY_Delta = (timeline)[i][iFrameIndex + 1U].vPos.y - (timeline)[i][iFrameIndex].vPos.y;
                        fPosY_Delta *= fCurFrameTimeDelta / fFrameTimeDelta;

                        if (i == KEYTYPE_LEFTHAND) // 왼손
                        {
                            m_pLeftHandComp->Set_Pos({ (timeline)[i][iFrameIndex].vPos.x + fPosX_Delta,
                            (timeline)[i][iFrameIndex].vPos.y + fPosY_Delta,
                            0.f });	// 이미지 위치

                            m_pLeftHandComp->Set_Scale({ (timeline)[i][iFrameIndex].vScale.x + fSizeX_Delta, 	// 이미지 크기
                                                          (timeline)[i][iFrameIndex].vScale.y + fSizeY_Delta,
                                                          1.f });

                            m_pLeftHandComp->Set_Rotation({ (timeline)[i][iFrameIndex].vRot.x + fRotX_Delta, 	// 이미지 회전
                                                             (timeline)[i][iFrameIndex].vRot.y + fRotY_Delta,
                                                             (timeline)[i][iFrameIndex].vRot.z + fRotZ_Delta });

                            // 텍스처 번호
                            m_fLeftFrame = (timeline)[i][iFrameIndex].texureframe;
                        }
                        else if(i == KEYTYPE_RIGHTHAND) // 오른손
                        {
                            m_pRightHandComp->Set_Pos({ (timeline)[i][iFrameIndex].vPos.x + fPosX_Delta,
                            (timeline)[i][iFrameIndex].vPos.y + fPosY_Delta,
                            0.f });	// 이미지 위치

                            m_pRightHandComp->Set_Scale({ (timeline)[i][iFrameIndex].vScale.x + fSizeX_Delta, 	// 이미지 크기
                                                          (timeline)[i][iFrameIndex].vScale.y + fSizeY_Delta,
                                                          1.f });

                            m_pRightHandComp->Set_Rotation({ (timeline)[i][iFrameIndex].vRot.x + fRotX_Delta, 	// 이미지 회전
                                                             (timeline)[i][iFrameIndex].vRot.y + fRotY_Delta,
                                                             (timeline)[i][iFrameIndex].vRot.z + fRotZ_Delta });


                            // 텍스처 번호
                            m_fRightFrame = (timeline)[i][iFrameIndex].texureframe;
                        }
                    }
                    else
                    {
                        if (i == KEYTYPE_LEFTHAND) // 왼손
                        {
                            m_pLeftHandComp->Set_Scale({ (timeline)[i][iFrameIndex].vScale.x, 	// 이미지 크기
                              (timeline)[i][iFrameIndex].vScale.y,
                              1.f });

                            m_pLeftHandComp->Set_Rotation({ (timeline)[i][iFrameIndex].vRot.x, 	// 이미지 회전
                                 (timeline)[i][iFrameIndex].vRot.y,
                                 (timeline)[i][iFrameIndex].vRot.z});


                            m_pLeftHandComp->Set_Pos({ (timeline)[i][iFrameIndex].vPos.x,
                                                        (timeline)[i][iFrameIndex].vPos.y,
                                                        0.f });	// 이미지 위치

                            // 텍스처 번호
                            m_fLeftFrame = (timeline)[i][iFrameIndex].texureframe;
                        }
                        else if (i == KEYTYPE_RIGHTHAND) // 오른손
                        {
                            m_pRightHandComp->Set_Scale({ (timeline)[i][iFrameIndex].vScale.x, 	// 이미지 크기
                                                          (timeline)[i][iFrameIndex].vScale.y,
                                                          1.f });

                            m_pRightHandComp->Set_Rotation({ (timeline)[i][iFrameIndex].vRot.x, 	// 이미지 회전
                            (timeline)[i][iFrameIndex].vRot.y,
                            (timeline)[i][iFrameIndex].vRot.z });

                            m_pRightHandComp->Set_Pos({ (timeline)[i][iFrameIndex].vPos.x,
                                                        (timeline)[i][iFrameIndex].vPos.y,
                                                        0.f });	// 이미지 위치

                            // 텍스처 번호
                            m_fRightFrame = (timeline)[i][iFrameIndex].texureframe;
                        }

                    }
                }
            }

            // 현재 순환중인 i가 오른손 타입일 경우만
            if (i == KEYTYPE_RIGHTHAND)
            {
                #pragma region (오른손) 첫번째 키프레임으로 설정할 값들
                // 쉴드가능 여부
                bShield = timeline[KEYTYPE_RIGHTHAND][0].bShieldPossible;
                // 쉴드시 프레임 설정
                fShieldFrame = timeline[KEYTYPE_RIGHTHAND][0].iShieldFrame;

                // 차징가능 여부
                bChargingReady = timeline[KEYTYPE_RIGHTHAND][0].bChargePossible;
                // 차징시 프레임 설정
                fFullChage = timeline[KEYTYPE_RIGHTHAND][0].iFullChargeFrame;
                #pragma endregion
            }

            //#pragma region 마지막 키프레임으로 설정할 값들
            //// 최대 프레임 설정
            ////m_fRightMaxFrame = timeline.back().time;
            //#pragma endregion   
        }
    }
    
#if _TEST_CONSOLE
    //cout << _fFrame << endl;
    //cout << timeline.back().time << endl;
#endif // _TEST_CONSOLE

    
}
