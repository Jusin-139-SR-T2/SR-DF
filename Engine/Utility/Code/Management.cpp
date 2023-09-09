#include "Management.h"

#include "Export_Utility.h"

IMPLEMENT_SINGLETON(CManagement)

CManagement::CManagement()
	: m_pScene(nullptr)
{
}

CManagement::~CManagement()
{
	Free();
}

CComponent* Engine::CManagement::Get_Component(COMPONENTID eID, const _tchar* pLayerTag, const _tchar* pObjTag, const _tchar* pComponentTag)
{
	// 매니지먼트 -> 씬 -> 레이어 -> 게임 오브젝트 -> 컴포넌트
	if (nullptr == m_pScene)
		return nullptr;

	return m_pScene->Get_Component(eID, pLayerTag, pObjTag, pComponentTag);
}

CGameObject* CManagement::Get_GameObject(const _tchar* pLayerTag, const _tchar* pObjTag)
{
	// 매니지먼트 -> 씬 -> 레이어 -> 게임 오브젝트
	if (nullptr == m_pScene)
		return nullptr;

	return m_pScene->Get_GameObject(pLayerTag, pObjTag);
}

HRESULT CManagement::Set_Scene(CScene* pScene)
{
	// 씬 해제 후 새로운 씬을 로드
	Safe_Release(m_pScene);

	Engine::Clear_RenderGroup();

	m_pScene = pScene;

	return S_OK;
}

_int CManagement::Update_Scene(const _float& fTimeDelta)
{
	// 씬 없으면 예외처리
	NULL_CHECK_RETURN(m_pScene, -1)

	return m_pScene->Update_Scene(fTimeDelta);
}

void CManagement::LateUpdate_Scene()
{
	// 씬 없으면 예외처리
	NULL_CHECK(m_pScene);
	m_pScene->LateUpdate_Scene();
}

void CManagement::Render_Scene(LPDIRECT3DDEVICE9 pGraphicDev)
{
	Engine::Render_GameObject(pGraphicDev);

	NULL_CHECK(m_pScene);
	m_pScene->Render_Scene();		// 디버깅용
}

void CManagement::Free()
{
	Safe_Release(m_pScene);
}
