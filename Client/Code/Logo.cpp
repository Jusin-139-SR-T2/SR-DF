#include "stdafx.h"
#include "Logo.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include <Player.h>
#include <Monster.h>
#include <Terrain.h>
#include <Stage.h>
#include <DynamicCamera.h>

CLogo::CLogo(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{
}

CLogo::~CLogo()
{
}

CLogo* CLogo::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Logo Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CLogo::Free()
{
	Safe_Release(m_pLoading);

	SUPER::Free();
}

HRESULT CLogo::Ready_Scene()
{
	/*_matrix	matView, matProj;

	D3DXMatrixLookAtLH(&matView,
		&_vec3(0.f, 0.f, -10.f),
		&_vec3(0.f, 0.f, 10.f),
		&_vec3(0.f, 1.f, 0.f));

	D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(60.f), (float)WINCX / WINCY, 0.1f, 1000.f);

	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matProj);*/

	/*m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);*/


	FAILED_CHECK_RETURN(Ready_Prototype(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Environment(L"Environment"), E_FAIL);

	// �ε� ������ �����ϱ�
	NULL_CHECK_RETURN(m_pLoading = CLoading::Create(m_pGraphicDev, CLoading::LOADING_STAGE), E_FAIL);

	return S_OK;
}

_int CLogo::Update_Scene(const _float& fTimeDelta)
{
	_int	iExit = SUPER::Update_Scene(fTimeDelta);

	// �ε��� ������ �� ���͸� ���� �������� ������ ������ �� �ִ�.
	if (m_pLoading->Get_Finish())
	{
		if (GetAsyncKeyState(VK_RETURN) & 0x8000)
		{
			CScene* pScene = CStage::Create(m_pGraphicDev);
			NULL_CHECK_RETURN(pScene, E_FAIL);

			FAILED_CHECK_RETURN(Engine::Set_Scene(pScene), E_FAIL);

			return 0;
		}
	}

	return iExit;
}

void CLogo::LateUpdate_Scene()
{
	SUPER::LateUpdate_Scene();
}

void CLogo::Render_Scene()
{
	// _DEBUG ��
}

HRESULT CLogo::Ready_Prototype()
{
	// ������Ÿ�� �ν��Ͻ��� ����Ѵ�.
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TransformComp", CTransform::Create(m_pGraphicDev)), E_FAIL);	// Ʈ������
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_RcTexBufferComp", CRcTex::Create(m_pGraphicDev)), E_FAIL);		// ����
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_LogoTextureComp", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"Resource/Texture/Player/Ma.jpg")), E_FAIL);	// �ؽ�ó
	
	return S_OK;
}

HRESULT CLogo::Ready_Layer_Environment(const _tchar* pLayerTag)
{
	// ���̾� �ʱ�ȭ
	Engine::CLayer* pLayer = nullptr;
	FAILED_CHECK_RETURN(Add_Layer(pLayerTag, pLayer = Engine::CLayer::Create()), E_FAIL);

	/*FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"DynamicCamera", CDynamicCamera::Create(m_pGraphicDev,
		&_vec3(0.f, 0.f, -1.f),
		&_vec3(0.f, 0.f, 1.f),
		&_vec3(0.f, 1.f, 0.f))), E_FAIL);*/

	// ������
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"BackGround", CBackGround::Create(m_pGraphicDev)), E_FAIL);

	return S_OK;
}



