#include "stdafx.h"
#include "GateWay1.h"

#include "Player.h"

CGateWay1::CGateWay1(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CGateWay1::CGateWay1(const CGateWay1& rhs)
    : Base(rhs)
{
}

CGateWay1::~CGateWay1()
{
}

CGateWay1* CGateWay1::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3 vPos, const _vec3 vRot, const _vec3 vScale)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject(vPos, vRot, vScale)))
    {
        Safe_Release(pInstance);

        MSG_BOX("GateWay Create Failed");
        return nullptr;
    }

    return pInstance;
}

CGateWay1* CGateWay1::Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject& tObjectSerial)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject(tObjectSerial)))
    {
        Safe_Release(pInstance);

        MSG_BOX("GateWay Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CGateWay1::Free()
{
    SUPER::Free();
}

HRESULT CGateWay1::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    return S_OK;
}

HRESULT CGateWay1::Ready_GameObject(const _vec3 vPos, const _vec3 vRot, const _vec3 vScale)
{
    FAILED_CHECK_RETURN(Ready_GameObject(), E_FAIL);

    m_pTransformComp->Set_Pos(vPos);
    m_pTransformComp->Set_Rotation(D3DXToRadian(vRot));
    m_pTransformComp->Set_Scale(vScale);

    m_pTransformComp->Readjust_Transform();

    m_pTextureComp->Receive_Texture(TEX_CUBE, L"Furniture", L"Gate1");

    return S_OK;
}

HRESULT CGateWay1::Ready_GameObject(const FSerialize_GameObject& tObjectSerial)
{
    FAILED_CHECK_RETURN(Ready_GameObject(), E_FAIL);

    m_pTransformComp->Set_Pos(tObjectSerial.vPos);
    m_pTransformComp->Set_Rotation(D3DXToRadian(tObjectSerial.vRotation));
    m_pTransformComp->Set_Scale(tObjectSerial.vScale);

    wstring strConvName(tObjectSerial.tHeader.strName.begin(), tObjectSerial.tHeader.strName.end());
    Set_ObjectName(strConvName);

    m_fPriority[0] = tObjectSerial.fPriority_Update;
    m_fPriority[1] = tObjectSerial.fPriority_LateUpdate;
    m_fPriority[2] = tObjectSerial.fPriority_Render;

    m_bUsePriority[0] = tObjectSerial.bUsePriority_Update;
    m_bUsePriority[1] = tObjectSerial.bUsePriority_LateUpdate;
    m_bUsePriority[2] = tObjectSerial.bUsePriority_Render;

    if (!tObjectSerial.strGroupKey.empty() && !tObjectSerial.strTextureKey.empty())
    m_pTextureComp->Receive_Texture(TEX_CUBE,
        wstring(tObjectSerial.strGroupKey.begin(), tObjectSerial.strGroupKey.end()).c_str()
        , wstring(tObjectSerial.strTextureKey.begin(), tObjectSerial.strTextureKey.end()).c_str());

    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform());

    return S_OK;
}

_int CGateWay1::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    _vec3 vUp = m_pTransformComp->Get_Up();
    D3DXVec3Normalize(&vUp, &vUp);

    if (!m_bIsClosed && !m_bIsEnded)
    {
        if (m_bIsStarted)
        {
            if (m_pTransformComp->Get_Pos().y < 3.5f)
                m_pTransformComp->Set_Pos(m_pTransformComp->Get_Pos() + vUp * 1.f * fTimeDelta);
            else
            {
                m_bIsClosed = true;
                // 소리 재생
            }
        }
    }
    else if (m_bIsClosed && !m_bIsEnded)
    {

    }
    else if (m_bIsClosed && m_bIsEnded)
    {
        if (m_pTransformComp->Get_Pos().y > -3.5f)
            m_pTransformComp->Set_Pos(m_pTransformComp->Get_Pos() - vUp * 1.f * fTimeDelta);
    }

    m_pTransformComp->Readjust_Transform();

    // 물리바디 업데이트
    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform());

    // Renderer 등록 
    Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

    Update_BlackBoard();

    return S_OK;
}

void CGateWay1::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CGateWay1::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

    m_pTextureComp->Render_Texture();
    m_pCubeBufferComp->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CGateWay1::OnCollision(CGameObject* pDst, const FContact* const pContact)
{
}

void CGateWay1::OnCollisionEntered(CGameObject* pDst, const FContact* const pContact)
{
}

void CGateWay1::OnCollisionExited(CGameObject* pDst)
{
}

HRESULT CGateWay1::Add_Component()
{
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_MonsterTextureComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pCubeBufferComp = Set_DefaultComponent_FromProto<CCubeBufferComp>(ID_STATIC, L"Com_CubeTex", L"Proto_CubeBufferComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pColliderComp = Set_DefaultComponent_FromProto<CColliderComponent>(ID_DYNAMIC, L"Com_Collider", L"Proto_ColliderOBBComp"), E_FAIL);

    // 물리 세계 등록
    m_pColliderComp->EnterToPhysics(0);

    // 충돌 함수 연결
    m_pColliderComp->Set_Collision_Event<ThisClass>(this, &ThisClass::OnCollision);
    m_pColliderComp->Set_CollisionEntered_Event<ThisClass>(this, &ThisClass::OnCollisionEntered);
    m_pColliderComp->Set_CollisionExited_Event<ThisClass>(this, &ThisClass::OnCollisionExited);

    // 충돌 레이어, 마스크 설정
    m_pColliderComp->Set_CollisionLayer(LAYER_WALL);
    //m_pColliderComp->Set_CollisionMask(LAYER_PLAYER);

    return S_OK;
}

void CGateWay1::Update_BlackBoard()
{
    if (m_bIsUnused)
        return;

    // 블랙보드 연결 대기, 안전 코드로 필수
    if (!m_wpBlackBoard_GateWay.Get_BlackBoard())
    {
        m_wpBlackBoard_GateWay.Set_BlackBoard(Engine::Get_BlackBoard(L"GateWay1"));
        // 연결 실패
        if (!m_wpBlackBoard_GateWay.Get_BlackBoard())
            return;
    }

    // 안전 코드를 거치면 일반 포인터로 접근 허용.
    CBlackBoard_GateWay1* pBlackBoard = m_wpBlackBoard_GateWay.Get_BlackBoard();

    if (!m_bIsStarted && !pBlackBoard->Is_MonsterAllDead())
    {
        m_bIsStarted = true;
        // 열리는 소리 첨가
    }

    if (!m_bIsEnded && pBlackBoard->Is_MonsterAllDead())
    {
        m_bIsEnded = true;
        // 철컹 소리 첨가
    }
}
