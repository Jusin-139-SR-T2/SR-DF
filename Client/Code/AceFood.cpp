#include "stdafx.h"
#include "AceFood.h"

CAceFood::CAceFood(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CAceFood::CAceFood(const CAceFood& rhs)
    : Base(rhs)
{
}

CAceFood::~CAceFood()
{
}

CAceFood* CAceFood::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pObjTag, const _float _fx, const _float _fy, const _float _fz)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (!pInstance)
        return nullptr;

    if (FAILED(pInstance->Ready_GameObject(pObjTag, _fx, _fy, _fz)))
    {
        Safe_Release(pInstance);

        MSG_BOX("FoodObject Create Failed");
        return nullptr;
    }
    
    return pInstance;
}

// Collision - Ʈ���� �ߵ��� (event���)
void CAceFood::OnCollision(CGameObject* pDst) // ��� �浹�� 
{
    OutputDebugString(L"Object - Food �浹��\n");
}

void CAceFood::OnCollisionEntered(CGameObject* pDst) // ó�� �浿 ���� 
{

    OutputDebugString(L"Object - Food �浹 ���� \n");
}

void CAceFood::OnCollisionExited(CGameObject* pDst) // �浹 ������ 
{
    OutputDebugString(L"Object - Food �浹 ��\n");
    Set_Dead();
}

HRESULT CAceFood::Ready_GameObject(const _tchar* pObjTag, const _float _fx, const _float _fy, const _float _fz)
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_pTransformComp->Set_Pos({ _fx, _fy, _fz });

    FoodName(pObjTag);

    CurPos = m_pTransformComp->Get_Pos();

    m_pTransformComp->Readjust_Transform();
    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform());

    return S_OK;
}

_int CAceFood::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    // ����Ÿ�� 
    Height_On_Terrain();

    // ������ 
    BillBoard(fTimeDelta);
    
    // ������ ����� enum���� texture ���� - eaten ���涧���� 
    Eat_Food(m_pCurName, fTimeDelta);

    // �����ٵ� ������Ʈ
    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform());

    // Renderer ��� 
    Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

    return S_OK;
}

void CAceFood::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CAceFood::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

    m_pTextureComp->Render_Texture();
    m_pBufferComp->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CAceFood::Free()
{
    SUPER::Free();
}

HRESULT CAceFood::Add_Component()
{
    NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_ObjectTextureComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculatorComponent>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pColliderComp = Set_DefaultComponent_FromProto<CColliderComponent>(ID_DYNAMIC, L"Com_Collider", L"Proto_ColliderSphereComp"), E_FAIL);

    // ���� ���� ���
    m_pColliderComp->EnterToPhysics(0);

    // �浹 �Լ� ����
    m_pColliderComp->Set_Collision_Event<ThisClass>(this, &ThisClass::OnCollision);
    m_pColliderComp->Set_CollisionEntered_Event<ThisClass>(this, &ThisClass::OnCollisionEntered);
    m_pColliderComp->Set_CollisionExited_Event<ThisClass>(this, &ThisClass::OnCollisionExited);

    // �浹 ���̾�, ����ũ ����
    m_pColliderComp->Set_CollisionLayer(LAYER_ITEM);
    m_pColliderComp->Set_CollisionMask(LAYER_PLAYER);

    return S_OK;
}

void CAceFood::Height_On_Terrain()
{
    _vec3		vPos;
    m_pTransformComp->Get_Info(INFO_POS, &vPos);

    CTerrainBufferComp* pTerrainBufferComp = dynamic_cast<CTerrainBufferComp*>(Engine::Get_Component(ID_STATIC, L"Environment", L"Terrain", L"Com_Buffer"));
    NULL_CHECK(pTerrainBufferComp);

    _float	fHeight = m_pCalculatorComp->Compute_HeightOnTerrain(&vPos,
        pTerrainBufferComp->Get_VtxPos(),
        pTerrainBufferComp->Get_VertexCountX() + 1U,
        pTerrainBufferComp->Get_VertexCountZ() + 1U,
        pTerrainBufferComp->Get_Scale(),
        pTerrainBufferComp->Get_InvOffset());

    m_pTransformComp->Set_Pos(vPos.x, fHeight +0.25f, vPos.z);
}

HRESULT CAceFood::BillBoard(const _float& fTimeDelta)
{
    _matrix		matWorld;
    _vec3 vPlayerPos;

    matWorld = *m_pTransformComp->Get_Transform();

    CTransformComponent* m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
    NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

    m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);
    _vec3 Pos = m_pTransformComp->Get_Pos();

    _vec3 vDir = vPlayerPos - m_pTransformComp->Get_Pos();

    D3DXVec3Normalize(&vDir, &vDir);

    _float rad = atan2f(vDir.x, vDir.z);

    // ȸ����� ����
    _matrix rotationMatrix;
    D3DXMatrixRotationY(&rotationMatrix, rad);

    m_pTransformComp->Set_WorldMatrixS(&(rotationMatrix * matWorld));

    return S_OK;
}

void CAceFood::FoodName(const _tchar* pObjTag)
{
    if ((wcscmp(pObjTag, L"APPLE") == 0) || (wcscmp(pObjTag, L"Apple") == 0))
    {
        m_pCurName = CAceFood::FOOD_NAME::APPLE;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Food", L"Apple");
        m_pTransformComp->Set_Scale({ 0.5f, 0.5f, 0.5f });
    }
    else if ((wcscmp(pObjTag, L"EATENAPPLE") == 0) || (wcscmp(pObjTag, L"EatenApple") == 0))
    {
        m_pCurName = CAceFood::FOOD_NAME::EATENAPPLE;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Food", L"EatenApple");
        m_pTransformComp->Set_Scale({ 0.5f, 0.5f, 0.5f });
    }
    else if ((wcscmp(pObjTag, L"BANANA") == 0) || (wcscmp(pObjTag, L"Banana") == 0))
    {
        m_pCurName = CAceFood::FOOD_NAME::BANANA;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Food", L"Banana");
        m_pTransformComp->Set_Scale({ 0.5f, 0.5f, 0.5f });
    }
    else if ((wcscmp(pObjTag, L"BANANAPEEL") == 0) || (wcscmp(pObjTag, L"BananaPeel") == 0))
    {
        m_pCurName = CAceFood::FOOD_NAME::BANANAPEEL;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Food", L"BananaPeel");
        m_pTransformComp->Set_Scale({ 0.5f, 0.5f, 0.5f });
    }
    else if ((wcscmp(pObjTag, L"COLA") == 0) || (wcscmp(pObjTag, L"Cola") == 0))
    {
        m_pCurName = CAceFood::FOOD_NAME::COLA;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Food", L"Cola");
        m_pTransformComp->Set_Scale({ 0.1f, 1.f,  0.5f });
    }
    else if ((wcscmp(pObjTag, L"MEDIKIT") == 0) || (wcscmp(pObjTag, L"Medkit") == 0))
    {
        m_pCurName = CAceFood::FOOD_NAME::MEDIKIT;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Food", L"Medkit");
        m_pTransformComp->Set_Scale({ 0.5f, 0.5f, 0.5f });
    }
    else
        m_pCurName = CAceFood::FOOD_NAME::FOOD_END;

}

void CAceFood::Eat_Food(FOOD_NAME eCurName, const _float& fTimeDelta)
{
    if (m_bEat)
    {
        switch (eCurName)
        {
        case CAceFood::FOOD_NAME::APPLE:

            m_pCurName = CAceFood::FOOD_NAME::EATENAPPLE;
            m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Food", L"EatenApple");
            m_pTransformComp->Set_Scale({ 0.5f, 0.5f, 0.5f });
            m_bDead = FALSE; // �״°� ����� �װɷ� ���� 
            //���ڸ��� 1����, ü�� 5�ʰ� 0.8�ʴ� 1�� ���� // 58 -> 65 , 71 -> 78 (+7)

            break;

        case CAceFood::FOOD_NAME::BANANA:

            m_pCurName = CAceFood::FOOD_NAME::BANANAPEEL;
            m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Food", L"BananaPeel");
            m_pTransformComp->Set_Scale({ 0.5f, 0.5f, 0.5f });
            m_bDead = FALSE;

            break;

        case CAceFood::FOOD_NAME::COLA:
            // ���ڸ��� 0����, ü�� 0.5�ʴ� 1�� ���� , 20�ʰ� �̼����� 
            m_bDead = TRUE;

            break;

        case CAceFood::FOOD_NAME::MEDIKIT:
            // ���ڸ��� 35 ü�� ���� 
            m_bDead = TRUE;
            break;

        }
    }
}
