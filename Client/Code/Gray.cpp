#include "stdafx.h"
#include "Gray.h"

CGray::CGray(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CGray::CGray(const CGray& rhs)
	: Base(rhs)
{
}

CGray::~CGray()
{
}

CGray* CGray::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);

        MSG_BOX("GrayMonster Create Failed");
        return nullptr;
    }

    pInstance->m_pTransformComp->Set_Pos(_x, _y, _z);
    pInstance->vPatrolPointZero = { _x, _y, _z };
    return pInstance;
}

HRESULT CGray::Ready_GameObject()
{
    srand(_ulong(time(NULL)));

    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_pTransformComp->Set_Scale({ 3.f, 5.f, 1.f });
    m_fFrame = 0;
    m_fFrameEnd = 0;
    m_fFrameSpeed = 10.f;
    
    // 충돌용
    m_pTransformComp->Readjust_Transform();
    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // 충돌 불러오는곳 
    FCollisionBox* pShape = dynamic_cast<FCollisionBox*>(m_pColliderComp->Get_Shape());
    pShape->vHalfSize = { 1.f, 2.f, 0.1f };

    // INFO
    m_iHP = 40;         // 체력
    m_iAttack = 10;     // 공격력
    
#pragma region 목표 상태머신 등록 - (AI) Judge
    m_tState_Obj.Set_State(STATE_OBJ::IDLE); // 초기상태 지정 

    m_tState_Obj.Add_Func(STATE_OBJ::IDLE, &CGray::AI_Idle);
    m_tState_Obj.Add_Func(STATE_OBJ::SUSPICIOUS, &CGray::AI_Suspicious);
    m_tState_Obj.Add_Func(STATE_OBJ::RECONNAISSANCE, &CGray::AI_Reconnaissance);
    m_tState_Obj.Add_Func(STATE_OBJ::YOUDIE, &CGray::AI_YouDie);
    m_tState_Obj.Add_Func(STATE_OBJ::TAUNT, &CGray::AI_Taunt);
    m_tState_Obj.Add_Func(STATE_OBJ::CHASE, &CGray::AI_Chase); 
    m_tState_Obj.Add_Func(STATE_OBJ::REST, &CGray::AI_Rest);

    m_tState_Obj.Add_Func(STATE_OBJ::WALK, &CGray::AI_Walk);
    m_tState_Obj.Add_Func(STATE_OBJ::RUN, &CGray::AI_Run);
    m_tState_Obj.Add_Func(STATE_OBJ::SIDEWALK, &CGray::AI_SideWalk);
    m_tState_Obj.Add_Func(STATE_OBJ::KEEPEYE, &CGray::AI_KeepEye);
    m_tState_Obj.Add_Func(STATE_OBJ::UPRIGHTRUN, &CGray::AI_UpRightRun);
    m_tState_Obj.Add_Func(STATE_OBJ::FRIGHTEN, &CGray::AI_Frighten);

    m_tState_Obj.Add_Func(STATE_OBJ::THROW, &CGray::AI_Throw);
    m_tState_Obj.Add_Func(STATE_OBJ::HEAVYATTACK, &CGray::AI_HeavyAttack);
    m_tState_Obj.Add_Func(STATE_OBJ::ATTACK, &CGray::AI_Attack);

    m_tState_Obj.Add_Func(STATE_OBJ::RECONNAISSANCE, &CGray::AI_Reconnaissance);
    m_tState_Obj.Add_Func(STATE_OBJ::GOHOME, &CGray::AI_GoHome);

    m_tState_Obj.Add_Func(STATE_OBJ::BLOCK, &CGray::AI_Block);
    m_tState_Obj.Add_Func(STATE_OBJ::CROTCHHIT, &CGray::AI_CrotchHit);
    m_tState_Obj.Add_Func(STATE_OBJ::HEADSHOT, &CGray::AI_HeadShot);
    m_tState_Obj.Add_Func(STATE_OBJ::HEADLESS, &CGray::AI_Headless);
    m_tState_Obj.Add_Func(STATE_OBJ::DEATH, &CGray::AI_Death);

    m_tState_Obj.Add_Func(STATE_OBJ::DAZED, &CGray::AI_Dazed);
    m_tState_Obj.Add_Func(STATE_OBJ::CHOPPED, &CGray::AI_Chopped);
    m_tState_Obj.Add_Func(STATE_OBJ::HEADLESS, &CGray::AI_Headless);
    m_tState_Obj.Add_Func(STATE_OBJ::DEATH, &CGray::AI_Death);

#pragma endregion

#pragma region 행동 상태머신 등록 - Acting
    m_tState_Act.Set_State(STATE_ACT::IDLE);

    m_tState_Act.Add_Func(STATE_ACT::IDLE, &CGray::Idle);
    m_tState_Act.Add_Func(STATE_ACT::APPROACH, &CGray::Approach);
    m_tState_Act.Add_Func(STATE_ACT::SUDDENATTACK, &CGray::SuddenAttack);
    m_tState_Act.Add_Func(STATE_ACT::SIDEMOVING, &CGray::SideMoving);
    m_tState_Act.Add_Func(STATE_ACT::ATTACK, &CGray::Attack);
    m_tState_Act.Add_Func(STATE_ACT::GOHOME, &CGray::GoHome);

#pragma endregion

#pragma region 액션 키 등록
    m_mapActionKey.Add_Action(ACTION_KEY::RUN); // 멀때 
    m_mapActionKey.Add_Action(ACTION_KEY::WALK); //중간
    m_mapActionKey.Add_Action(ACTION_KEY::RUN); //가까울때

    m_mapActionKey.Add_Action(ACTION_KEY::KEEPEYE); // 가까울때
    m_mapActionKey.Add_Action(ACTION_KEY::SIDEWALK); // y축에 차이가 있을때 

    m_mapActionKey.Add_Action(ACTION_KEY::UPRIGHT); // 멀때 
    m_mapActionKey.Add_Action(ACTION_KEY::FRIGHTEN); //중간

    m_mapActionKey.Add_Action(ACTION_KEY::BASIC_ATTACK);
    m_mapActionKey.Add_Action(ACTION_KEY::HEAVY_ATTACK);

    m_mapActionKey.Add_Action(ACTION_KEY::GOHOME);

#pragma endregion

    return S_OK;
}

_int CGray::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    // 플레이어 정보 계속해서 백업 
    Get_PlayerPos(); 

    // 지형타기 
    Height_On_Terrain(); 

    // 빌보드 
    if(m_bBillboard)
        Billboard(fTimeDelta);

    // 상태머신-------------------------------------
    m_fFrame += m_fFrameSpeed * fTimeDelta;

    m_tState_Obj.Get_StateFunc()(this, fTimeDelta);	// AI
    m_tState_Act.Get_StateFunc()(this, fTimeDelta);	// 행동
    m_mapActionKey.Update();	// 액션키 초기화

    if (m_fFrame > m_fFrameEnd)
    {
        m_fFrame = 0.f;

        if (STATE_OBJ::TAUNT == m_tState_Obj.Get_State() ||
            STATE_OBJ::KEEPEYE == m_tState_Obj.Get_State() ||
            STATE_OBJ::SIDEWALK == m_tState_Obj.Get_State() 
            )
            m_fCheck += 1;
    }

    

#pragma region 테스트 장소 

    // ---------- 테스트 빌드 ---------------------
    if (Engine::IsKey_Pressing(DIK_G))
    {
        m_tState_Obj.Set_State(STATE_OBJ::THROW);
    }

    if (Engine::IsKey_Pressing(DIK_H))
    {
        RenderSplitImages();
    }
    // --------------------------------------------

#pragma endregion 
    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // 충돌 불러오는곳

    Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

    return S_OK;
}

#pragma region 기본 환경설정 

void CGray::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CGray::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

    m_pTextureComp->Render_Texture(_ulong(m_fFrame));
    m_pBufferComp->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}


HRESULT CGray::Add_Component()
{
    NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculatorComponent>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_GrayTextureComp"), E_FAIL);

    // 콜라이더 컴포넌트
    NULL_CHECK_RETURN(m_pColliderComp = Set_DefaultComponent_FromProto<CColliderComponent>(ID_DYNAMIC, L"Com_Collider", L"Proto_ColliderBoxComp"), E_FAIL);

    // 물리 세계 등록
    m_pColliderComp->EnterToPhysics(0);

    // 충돌 함수 연결
    m_pColliderComp->Set_Collision_Event<ThisClass>(this, &ThisClass::OnCollision);
    m_pColliderComp->Set_CollisionEntered_Event<ThisClass>(this, &ThisClass::OnCollisionEntered);
    m_pColliderComp->Set_CollisionExited_Event<ThisClass>(this, &ThisClass::OnCollisionExited);

    // 충돌 레이어, 마스크 설정
    m_pColliderComp->Set_CollisionLayer(ELAYER_MONSTER); // 이 클래스가 속할 충돌레이어 
    m_pColliderComp->Set_CollisionMask(ELAYER_PLAYER); // 얘랑 충돌해야하는 레이어들 

    return S_OK;
}

void CGray::Free()
{
    SUPER::Free();
}

#pragma endregion 

#pragma region 환경설정 부속파트 + 상태머신 보조함수 

void CGray::Height_On_Terrain()
{
    _vec3		vPos;
    m_pTransformComp->Get_Info(INFO_POS, &vPos);

    CTerrainBufferComp* pTerrainBufferComp = dynamic_cast<CTerrainBufferComp*>(Engine::Get_Component(ID_STATIC, L"Environment", L"Terrain", L"Com_Buffer"));
    NULL_CHECK(pTerrainBufferComp);

    _float	fHeight = m_pCalculatorComp->Compute_HeightOnTerrain(&vPos, pTerrainBufferComp->Get_VtxPos());

    m_pTransformComp->Set_Pos(vPos.x, fHeight + 1.3f , vPos.z);
}

void CGray::Billboard(const _float& fTimeDelta)
{ 
    // 몬스터가 플레이어 바라보는 벡터 
    _vec3 vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();

    D3DXVec3Normalize(&vDir, &vDir);

    _float rad = atan2f(vDir.x, vDir.z);

    m_pTransformComp->Set_RotationY(rad);
}

void CGray::RenderSplitImages()
{
    _vec3 MonsterLook = m_pTransformComp->Get_Look();
    _vec3 PlayerLook = m_pPlayerTransformcomp->Get_Look();

    _float DotProduct = D3DXVec3Dot(&MonsterLook, &PlayerLook);

    //if(DotProduct)

    swprintf_s(debugString, L"Brown - 변수 확인 DotProduct = %f\n", DotProduct);
    OutputDebugStringW(debugString);
}

_bool CGray::Detect_Player()
{
    _vec3 vMonsterLook;

    m_pTransformComp->Get_Info(INFO_LOOK, &vMonsterLook);

    //몬스터가 플레이어 바라보는 벡터 
    _vec3 MonToPlayer = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();
    _float currdistance = D3DXVec3Length(&MonToPlayer); 
   
    //정규화
    D3DXVec3Normalize(&MonToPlayer, &MonToPlayer); 

    // 시야범위 우선체크 
    if (currdistance > m_fMonsterSightDistance)
        return false;

    // 내적하여 RADIAN구하기 
    _float fradian = acos(D3DXVec3Dot(&vMonsterLook, &MonToPlayer)) * 180 / D3DX_PI;

    // 시야범위 + 시야각도 = 포착 
    if (fradian < m_fMonsterFov * 2 && currdistance < m_fMonsterSightDistance)
        return true;
    else
        return false;
}

_float CGray::Calc_Distance()
{
    // 플레이어 위치 - 몬스터 위치 = 몬스터가 플레이어 바라보는 벡터 
    _float fDistance = D3DXVec3Length(&(m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos()));

    return fDistance;
}

HRESULT CGray::Get_PlayerPos()
{
    m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
    NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

    m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);

    return S_OK;
}

#pragma endregion

#pragma region 충돌파트 

void CGray::OnCollision(CGameObject* pDst)
{
    OutputDebugString(L"▶Gray 충돌중 \n");
}
void CGray::OnCollisionEntered(CGameObject* pDst)
{
    OutputDebugString(L"▶Gray 충돌시작 \n");
}

void CGray::OnCollisionExited(CGameObject* pDst)
{
    OutputDebugString(L"▶Gray이랑 충돌끝남 \n");
}


#pragma endregion 

#pragma region 목표(AI) 상태머신 부분 
void CGray::AI_Idle(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"▷Gray - 상태머신 : idle 진입   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Single", L"Idle");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_iPreHP = m_iHP; // 상태체크용 hp저장 
    }

    if (m_tState_Obj.Can_Update())
    {
        // 조건 - 플레이어가 시야각으로 들어오면 
        if (Detect_Player())
        {
            m_tState_Obj.Set_State(STATE_OBJ::SUSPICIOUS);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"▷Gray - 상태머신 : idle 끝  \n");
    }
}

void CGray::AI_Suspicious(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"▷Gray - 상태머신 : Suspicious 진입  \n");
        // idle상태 그대로 이어서 텍스처 유지 
        m_bBillboard = true;
        Engine::Add_GameObject(L"GameLogic", CAwareness::Create(m_pGraphicDev,
            m_pTransformComp->Get_Pos().x + 0.2f, m_pTransformComp->Get_Pos().y + 1.3f, m_pTransformComp->Get_Pos().z));

    }

    if (m_tState_Obj.Can_Update())
    {
        if (Detect_Player()) // 시야각 이내에 위치 + 시야거리 이내 위치 
        {
            OutputDebugString(L"▷Gray - 상태머신 : Suspicious -> 플레이어 포착   \n");

            m_fGrayAwareness += fDeltaTime * 3.f;

            // 2. 인지값이 MAX가 되면 플레이어 추격 시작 
            if (m_fMaxAwareness <= m_fGrayAwareness)
            {
                m_fGrayAwareness = m_fMaxAwareness; // 추후 감소를 위해 최대값으로 고정 

               // int iCombo = (rand() % 10) + 1; // 1~10 
               //
               // if (6 <= iCombo) // 6~10
                    m_tState_Obj.Set_State(STATE_OBJ::YOUDIE);
               //
               // if (6 > iCombo) // 1~5
               //     m_tState_Obj.Set_State(STATE_OBJ::TAUNT);

            }
        }
        else // 범위밖은 감소
        {
            OutputDebugString(L"▶Gray - 변수체크 : 인지변수 감소중   \n");

            m_fGrayAwareness -= fDeltaTime * 6.f;

            //플레이어가 시야각을 벗어나 인지값이 초기화되면 idle로 back
            if (0 >= m_fGrayAwareness)
            {
                m_fGrayAwareness = 0.f;
                m_tState_Obj.Set_State(STATE_OBJ::IDLE);
            }
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"▷Gray - 상태머신 : Suspicious 끝  \n");

    }
}

void CGray::AI_Taunt(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Taunt");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        // 조건 - 플레이어가 시야각으로 들어오면 
        if (Detect_Player())
        {
            if (2 == m_fCheck) // 도발 두번 하고 따라가기 
            {
                m_fCheck = 0; //다른데도 쓰니까 0으로 되돌리기 
                m_iPreHP = m_iHP; // 피격없으면 pre로 이전hp 저장 
                m_tState_Obj.Set_State(STATE_OBJ::CHASE); // AI = 추격모드
            }
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {

    }
}

void CGray::AI_YouDie(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"▷Gray - 상태머신 : You Die 돌입   \n");

        m_fFrameSpeed = 8.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"YouDie");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_fFrame > m_fFrameEnd ) // 도발 두번 하고 따라가기 
        {
            m_tState_Obj.Set_State(STATE_OBJ::CHASE); // AI = 추격모드
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"▷Gray - 상태머신 : You Die 끝   \n");
    }
}

void CGray::AI_Reconnaissance(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {

    }
    if (m_tState_Obj.Can_Update())
    {
        m_fConsider -= fDeltaTime * 3.f;

        if (Detect_Player())
        {
            m_fGrayAwareness += fDeltaTime * 4.f; // 이전보다 더 빠르게 증가할것 

            if (m_fGrayAwareness >= m_fMaxAwareness)
            {
                m_fGrayAwareness = m_fMaxAwareness;
                m_tState_Obj.Set_State(STATE_OBJ::YOUDIE);
            }
        }

        if (0 >= m_fConsider)
        {
            m_fConsider = 10.f; // 다시 초기 셋팅으로 
            m_tState_Obj.Set_State(STATE_OBJ::GOHOME);
        }
    }


    if (m_tState_Obj.IsState_Exit())
    {

    }
}

void CGray::AI_GoHome(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_fFrameSpeed = 14.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"WalkNorth");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        //행동이 IDLE일때 WALK 가상키 누르기 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::GOHOME].Act();

        if (m_bArrive && m_fFrame > m_fFrameEnd) // 프레임 다 돌면 
        {
            m_bArrive = false;
            m_tState_Obj.Set_State(STATE_OBJ::IDLE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
    }
}

void CGray::AI_Chase(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"▷Gray - 상태머신 : Chase 진입  \n");

        m_fFrameSpeed = 8.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Rest");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        OutputDebugString(L"★ Gray 디버깅 : Chease - Update 돌입   \n");

        if (Detect_Player()) // 플레이어 포착 
        {
            _float CurDistance = Calc_Distance();

            // 뛰어서 다가옴 : 8 < a <= 13
            if (m_fRunDistance < CurDistance)
            {
                OutputDebugString(L"★ Gray 디버깅 : Chease -RUN 상태함수 Set   \n");
                m_tState_Obj.Set_State(STATE_OBJ::RUN);
            }
            // 걸어서 다가옴 : 7.5 < a <= 8 
            else if (m_fWalkDistance < CurDistance && m_fRunDistance >= CurDistance)
            {
                OutputDebugString(L"★ Gray 디버깅 : Chease - WALK 상태함수 Set   \n");
                m_tState_Obj.Set_State(STATE_OBJ::WALK);
            }
            // 주시하면서 경계 : 6 < a <= 7.5
            else if (m_fEyesOnYouDistance < CurDistance && m_fWalkDistance >= CurDistance)
            {
                OutputDebugString(L"★ Gray 디버깅 : Chease - 경계 상태함수 Set    \n");
                int iCombo = (rand() % 10) + 1;

                if (6 <= iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::KEEPEYE);

                if (6 > iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::SIDEWALK);
            }
            //대충 공격하러 옴 : 3 < a <=6
            else if (m_fCloseToYouDistance < CurDistance && m_fEyesOnYouDistance >= CurDistance)
            {
                OutputDebugString(L"★ Gray 디버깅 : Chease - 접근 상태함수 Set  \n");
                int iCombo = (rand() % 15) + 1;

                if (4 > iCombo)                         // 20프로 1~3
                    m_tState_Obj.Set_State(STATE_OBJ::THROW);
                else if ( 10 > iCombo && 4 <= iCombo)   // 40프로  4 ~ 9
                    m_tState_Obj.Set_State(STATE_OBJ::UPRIGHTRUN);
                else                                    // 40프로 10~15
                    m_tState_Obj.Set_State(STATE_OBJ::FRIGHTEN);
            }
            else  // 공격함
            {
                OutputDebugString(L"★ Gray 디버깅 : Chease - 공격 상태함수 Set   \n");
                int iCombo = (rand() % 10) + 1;

                if (6 > iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::ATTACK);
                else
                m_tState_Obj.Set_State(STATE_OBJ::HEAVYATTACK);
            }

        }
        else // 쫒다가도 시야에서 벗어나면 게이지 줄어들어서 SUSPICIOUS로 돌아감 
        {
            OutputDebugString(L"★ Gray 디버깅 : Chease - 포착실패   \n");
            m_fGrayAwareness -= fDeltaTime * 4.f;

            if (0 >= m_fGrayAwareness) //인지값이 초기화되면 
            {
                m_fGrayAwareness = 0.f;
                m_tState_Obj.Set_State(STATE_OBJ::RECONNAISSANCE);
            }

        }
    }
     
    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"▷Gray - 상태머신 : Chase 끝   \n");
    }
}

void CGray::AI_Rest(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //일종의 숨고르기 구간임 
        OutputDebugString(L"▷Gray - 상태머신 : Rest 돌입   \n");
        m_fFrameSpeed = 10.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Rest");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_fFrame > m_fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"▷Gray - 상태머신 : Rest 끝   \n");
    }
}

void CGray::AI_Run(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"▷Gray - 상태머신 : Run 돌입   \n");
        m_fFrameSpeed = 16.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Run");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        //행동이 IDLE일때 RUN 가상키 누르기 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::RUN].Act();

        // 조건 - 플레이어가 시야각으로 들어오면 
        if (m_fFrame > m_fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }


    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"▷Gray - 상태머신 : Run 끝   \n");
    }
}

void CGray::AI_Walk(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        
        OutputDebugString(L"▷Gray - 상태머신 : Walk 돌입   \n");

        m_fFrameSpeed = 10.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Walk");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::WALK].Act();

        if (m_fFrame > m_fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
        
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"▷Gray - 상태머신 : Walk 끝   \n");
    }
}

void CGray::AI_KeepEye(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"▷Gray - 상태머신 : KeepEye 진입   \n");

        m_fFrameSpeed = 10.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"KeepEye");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        //행동이 IDLE일때 WALK 가상키 누르기 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::KEEPEYE].Act();

        // 조건 - 플레이어가 시야각으로 들어오면 
        if (m_fFrame > m_fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::FRIGHTEN);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"▷Gray - 상태머신 : KeepEye 끝 \n");
    }
}

void CGray::AI_SideWalk(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"▷Gray - 상태머신 : SideWalk 진입   \n");
        m_fFrameSpeed = 9.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"SideWalk");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::SIDEWALK].Act();

        if (2 == m_fCheck)
        {
            if (m_fFrame > m_fFrameEnd)
            {
                m_fCheck = 0.f;
                m_tState_Obj.Set_State(STATE_OBJ::REST);
            }
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"▷Gray - 상태머신 : SideWalk 끝 \n");
    }
}

void CGray::AI_Throw(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"▷Gray - 상태머신 : Throw 진입   \n");

        m_fFrameSpeed = 10.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Throw");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
       
        // 투사체 발사 
        Engine::Add_GameObject(L"GameLogic", CThrowPipe::Create(m_pGraphicDev, 
            m_pTransformComp->Get_Pos().x, m_pTransformComp->Get_Pos().y + 1.f , m_pTransformComp->Get_Pos().z));
    }

    if (m_tState_Obj.Can_Update())
    {
        // 충돌체는 여기서 가상키 누르고 state에서 할예정 

        if (m_fFrame > m_fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"▷Gray - 상태머신 : Throw 끝   \n");
    }
}

void CGray::AI_UpRightRun(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"▷Gray - 상태머신 : UpRightRun 진입   \n");
        m_fFrameSpeed = 13.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"UpRightRun");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::UPRIGHT].Act();

        if (m_fFrame > m_fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::ATTACK);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"▷Gray - 상태머신 : UpRightRun 끝   \n");
    }
}

void CGray::AI_Frighten(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"▷Gray - 상태머신 : Frighten 진입   \n");
        m_fFrameSpeed = 12.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Frighten");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::FRIGHTEN].Act();

        if (m_fFrame > m_fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::ATTACK);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"▷Gray - 상태머신 : Frighten 끝   \n");
    }
    
}

void CGray::AI_Attack(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"▷Gray - 상태머신 : Attack 진입   \n");
        m_fFrameSpeed = 10.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Attack");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::BASIC_ATTACK].Act();

        if (m_fFrame > m_fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"▷Gray - 상태머신 : Attack 끝   \n");
    }
}

void CGray::AI_HeavyAttack(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"▷Gray - 상태머신 : HeavyAttack 진입   \n");
        m_fFrameSpeed = 10.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"HeavyAttack");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::HEAVY_ATTACK].Act();

        if (m_fFrame > m_fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"▷Gray - 상태머신 : HeavyAttack 끝   \n");
    }
}

void CGray::AI_Block(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"▷Gray - 상태머신 : Block 진입   \n");
        m_fFrameSpeed = 10.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Block");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        //행동이 IDLE일때 WALK 가상키 누르기 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::GOHOME].Act();

        // 조건 - 처음 위치로 돌아갈때까지 
        m_tState_Obj.Set_State(STATE_OBJ::WALK);
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"▷Gray - 상태머신 : HeavyAttack 끝   \n");
    }
}

void CGray::AI_CrotchHit(float fDeltaTime)
{
}

void CGray::AI_FacePunch(float fDeltaTime)
{
}

void CGray::AI_Falling(float fDeltaTime)
{
}

void CGray::AI_Dazed(float fDeltaTime)
{
}

void CGray::AI_Chopped(float fDeltaTime)
{
}

void CGray::AI_HeadShot(float fDeltaTime)
{
}

void CGray::AI_Headless(float fDeltaTime)
{
}

void CGray::AI_Death(float fDeltaTime)
{
}


#pragma endregion

#pragma region 행동 상태머신 부분 
void CGray::Idle(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 행동머신 : Idle 진입   \n");
    }

    if (m_tState_Act.Can_Update())
    {
        // 일반 다가오는파트 
        if (m_mapActionKey[ACTION_KEY::RUN].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::APPROACH);

        if (m_mapActionKey[ACTION_KEY::WALK].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::APPROACH);

        // 주시하며 경계
        if (m_mapActionKey[ACTION_KEY::KEEPEYE].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::SIDEMOVING);

        if (m_mapActionKey[ACTION_KEY::SIDEWALK].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::SIDEMOVING);

        // 
        if (m_mapActionKey[ACTION_KEY::UPRIGHT].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::SUDDENATTACK);

        if (m_mapActionKey[ACTION_KEY::FRIGHTEN].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::SUDDENATTACK);

        // 공격파트 
        if (m_mapActionKey[ACTION_KEY::BASIC_ATTACK].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::ATTACK);

        if (m_mapActionKey[ACTION_KEY::HEAVY_ATTACK].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::ATTACK);

        // 패트롤파트 
        if (m_mapActionKey[ACTION_KEY::GOHOME].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::GOHOME);
    }

    if (m_tState_Act.IsState_Exit()) // 가끔 필요할때가 있어서 - 찾아보기 
    {
        //OutputDebugString(L"▷Gray - 행동머신 : Idle 끝   \n");
    }
}

void CGray::Approach(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 행동머신 : Approach 진입   \n");
    }

    // 실행
    {
        if (Calc_Distance() > m_fMonsterSightDistance) // 시야밖은 recon으로 - 우선사항 
            m_tState_Obj.Set_State(STATE_OBJ::RECONNAISSANCE);

        m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);

        vDir = vPlayerPos - m_pTransformComp->Get_Pos();
        //D3DXVec3Normalize(&vDir, &vDir);
        
        m_pTransformComp->Set_Look(vDir);
        
        if (STATE_OBJ::RUN == m_tState_Obj.Get_State())
            m_pTransformComp->Move_Pos(&vDir, fDeltaTime, m_fRunSpeed);
        
        if (STATE_OBJ::WALK == m_tState_Obj.Get_State())
            m_pTransformComp->Move_Pos(&vDir, fDeltaTime, m_fWalkSpeed);

        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }
    // 조건
    {
    }

    if (m_tState_Act.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 행동머신 : Approach 끝   \n");
    }
}

void CGray::SideMoving(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
    }

    // 실행
    {   
        if (Calc_Distance() > m_fMonsterSightDistance) // 시야밖은 recon으로
            m_tState_Obj.Set_State(STATE_OBJ::RECONNAISSANCE);
        
        _vec3 Right = m_pTransformComp->Get_Right();
        D3DXVec3Normalize(&Right, &Right);
        
        if (STATE_OBJ::KEEPEYE == m_tState_Obj.Get_State())
        {
            m_pTransformComp->Move_Pos(&Right, fDeltaTime, m_fKeepEyeSpeed);
        }

        if (STATE_OBJ::SIDEWALK == m_tState_Obj.Get_State())
        {
            m_pTransformComp->Move_Pos(&Right, fDeltaTime, m_fSideWalkSpeed);
        }

        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }
    // 조건
    {
    }

    if (m_tState_Act.IsState_Exit())
    {
    }
}

void CGray::SuddenAttack(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
    }

    // 실행
    {
        if (Calc_Distance() > m_fMonsterSightDistance) // 시야밖은 recon으로
            m_tState_Obj.Set_State(STATE_OBJ::RECONNAISSANCE);

        m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);

            vDir = vPlayerPos - m_pTransformComp->Get_Pos();

        if (STATE_OBJ::UPRIGHTRUN == m_tState_Obj.Get_State())
            m_pTransformComp->Move_Pos(&vDir, fDeltaTime, m_fUprightSpeed);
       
        if (STATE_OBJ::FRIGHTEN == m_tState_Obj.Get_State())
            m_pTransformComp->Move_Pos(&vDir, fDeltaTime, m_fRunSpeed);
        
        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    // 조건
    {

    }

    if (m_tState_Act.IsState_Exit())
    {
    }
}

void CGray::Attack(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
    }

    // 실행
    {
        if (STATE_OBJ::ATTACK == m_tState_Obj.Get_State())
        {
            // 충돌체 만들어서 기본공격 수행

        }

        if (STATE_OBJ::HEAVYATTACK == m_tState_Obj.Get_State())
        {
            // 충돌체 만들어서 공격 수행
        }

        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    // 조건
    {

    }

    if (m_tState_Act.IsState_Exit())
    {
    }
}

void CGray::GoHome(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
    }

    // 실행
    {
        _vec3 vDirect = vPatrolPointZero - m_pTransformComp->Get_Pos();

        _float fDistance = D3DXVec3Length(&vDirect);

        D3DXVec3Normalize(&vDirect, &vDirect);

        if (2.f >= fDistance)
        {
            m_bArrive = true;
            m_tState_Act.Set_State(STATE_ACT::IDLE);
        }
        else
        {
            //OutputDebugString(L"▷Brown - 기존 패트롤 포인트 복귀중   \n");
            m_pTransformComp->Move_Pos(&vDirect, fDeltaTime, m_fSideWalkSpeed);
        }
    }

    // 조건
    {

    }

    if (m_tState_Act.IsState_Exit())
    {
    }
}


#pragma endregion