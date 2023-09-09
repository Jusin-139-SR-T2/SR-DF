#include "stdafx.h"
#include "Monster.h"

#include "Export_System.h"
#include "Export_Utility.h"

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CMonster::CMonster(const CMonster& rhs)
    : Base(rhs)
{
}

CMonster::~CMonster()
{
}

HRESULT CMonster::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    D3DXMatrixIdentity(&m_matRotAxis);
    m_pTransformCom->m_vScale.x = 1.f;


    return S_OK;
}

_int CMonster::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    Key_Input(fTimeDelta);

    return 0;
}

void CMonster::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CMonster::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Render_Texture(0);
    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CMonster::Add_Component()
{
    CComponent* pComponent = nullptr;

    // 버퍼 컴포넌트
    NULL_CHECK_RETURN(pComponent = m_pBufferCom = Engine::Clone_Proto<CRcTex>(L"Proto_RcTexComp"), E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // 트랜스폼 컴포넌트
    NULL_CHECK_RETURN(pComponent = m_pTransformCom = Engine::Clone_Proto<CTransform>(L"Proto_TransformComp"), E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // 텍스처 컴포넌트
    NULL_CHECK_RETURN(pComponent = m_pTextureCom = Engine::Clone_Proto<CTexture>(L"Proto_LogoTextureComp"), E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

void CMonster::Key_Input(const _float& fTimeDelta)
{
    NULL_CHECK(m_pTarget);

    _vec3	vDir, vTargetDir, vTargetPos;

    m_pTransformCom->Get_Info(INFO_UP, &vDir);

    D3DXVec3Normalize(&vDir, &vDir);
    

    CTransform* pTransform = dynamic_cast<CTransform*>(m_pTarget->Get_Component(ID_DYNAMIC, L"Com_Transform"));
    pTransform->Get_Info(INFO_POS, &vTargetPos);
    vTargetDir = vTargetPos - m_pTransformCom->m_vInfo[INFO_POS];
    _bool bClash = false;
    D3DXVec3Normalize(&vTargetDir, &vTargetDir);

    _float fDot = D3DXVec3Dot(&vDir, &vTargetDir);
    _vec3 vCross;
    D3DXVec3Cross(&vCross, &vDir, &vTargetDir);
    _float fCCW = vDir.x * vTargetDir.y + vDir.y * vTargetDir.z + vDir.z * vTargetDir.x - (vTargetDir.x * vDir.y + vTargetDir.y * vDir.z + vTargetDir.z * vDir.x);
    
    if (vTargetDir == _vec3({ 0.f, 0.f, 0.f }))
    {
        vCross = { 0.f, 0.f, 1.f };
        vTargetDir = { 0.f, 1.f, 0.f };
        bClash = true;
    }
    _float fCos = fDot / (D3DXVec3Length(&vDir) * D3DXVec3Length(&vTargetDir));
    _float fRadian = acosf(fCos);
    //_matrix matRot;

    if (!bClash)
    {
        // D3DXVec3TransformCoord()
        //D3DXVec3Normalize(&vFinalDir, &vFinalDir);
        //m_pTransformCom->Move_Pos(&, fTimeDelta, 2.f);
        //m_pTransformCom->Move_Pos(&vTargetDir, fTimeDelta, 2.f);
    }

    D3DXMatrixRotationAxis(&m_matRotAxis, &vCross, fRadian);

    *m_pTransformCom->Get_WorldMatrix() *= m_matRotAxis;

    _matrix matTest = *m_pTransformCom->Get_WorldMatrix();
    D3DXMatrixIdentity(&matTest);
}

CMonster* CMonster::Create(LPDIRECT3DDEVICE9 pGraphicDev)
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

void CMonster::Free()
{
    SUPER::Free();
}
