#include "Renderer.h"

IMPLEMENT_SINGLETON(CRenderer)

CRenderer::CRenderer()
{
	// 뷰포트 사용가능 디폴트 8개
	m_vecViewport.reserve(VIEWPORT_COUNT);
	for (size_t i = 0; i < VIEWPORT_COUNT; i++)
	{
		D3DVIEWPORT9 UiViewPort;
		UiViewPort.X = 0;
		UiViewPort.Y = 0;
		UiViewPort.Width = WINCX;
		UiViewPort.Height = WINCY;
		UiViewPort.MinZ = 0;
		UiViewPort.MaxZ = 100;
		m_vecViewport.push_back(UiViewPort);
	}
	D3DXMatrixOrthoLH(&m_matOrtho, WINCX, WINCY, 0.f, 100.f);

}

CRenderer::~CRenderer()
{
	Free();
}

void CRenderer::Ready_Renderer()
{

}

void CRenderer::Add_RenderGroup(RENDERID eType, CGameObject* pGameObject)
{
	if ((0 > eType || RENDER_END <= eType) || nullptr == pGameObject)
		return;

	m_RenderGroup[eType].push_back(pGameObject);
	pGameObject->AddRef();
}

void CRenderer::Render_GameObject(LPDIRECT3DDEVICE9& pGraphicDev)
{
	// 렌더처리를 하는 종류에 따라 따로 모아서 처리한다.
	Render_Priority(pGraphicDev);
	Render_NonAlpha(pGraphicDev);
	Render_Alpha(pGraphicDev);
	Render_AlphaTest(pGraphicDev); // + 성희 추가
	Render_UI(pGraphicDev);

	// 항상 처리 후 다음 프레임을 위해 초기화시킨다.
	Clear_RenderGroup();
}

void CRenderer::Clear_RenderGroup()
{
	for (size_t i = 0; i < RENDER_END; ++i)
	{
		for_each(m_RenderGroup[i].begin(), m_RenderGroup[i].end(), CDeleteObj());
		m_RenderGroup[i].clear();
	}
}

void CRenderer::Render_Priority(LPDIRECT3DDEVICE9& pGraphicDev)
{
	for (auto& iter : m_RenderGroup[RENDER_PRIORITY])
		iter->Render_GameObject();
}

// + 성희 추가
void CRenderer::Render_AlphaTest(LPDIRECT3DDEVICE9& pGraphicDev)
{
	DWORD dwTest;
	pGraphicDev->GetRenderState(D3DRS_ALPHABLENDENABLE, &dwTest);

	pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 200);

	for (auto& iter : m_RenderGroup[RNEDER_ALPHATEST])
		iter->Render_GameObject();

	pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}

void CRenderer::Render_NonAlpha(LPDIRECT3DDEVICE9& pGraphicDev)
{
	for (auto& iter : m_RenderGroup[RENDER_NONALPHA])
		iter->Render_GameObject();
}

void CRenderer::Render_Alpha(LPDIRECT3DDEVICE9& pGraphicDev)
{
	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pGraphicDev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	for (auto& iter : m_RenderGroup[RENDER_ALPHA])
		iter->Render_GameObject();

	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void CRenderer::Render_UI(LPDIRECT3DDEVICE9& pGraphicDev)
{
	_matrix matIdentity;
	pGraphicDev->SetTransform(D3DTS_VIEW, D3DXMatrixIdentity(&matIdentity));	// 항등행렬로 적용된 뷰 행렬 초기화.
	pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matOrtho);					// 직교투영 행렬 적용.
	pGraphicDev->SetViewport(&m_vecViewport[0]);								// 뷰포트 설정.

	pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);						// Z버퍼 OFF

	pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);					// 알파렌더링 ON

	pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pGraphicDev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	for (auto& iter : m_RenderGroup[RENDER_UI])
		iter->Render_GameObject();

	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);					// 알파렌더링 OFF

	pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);						// Z버퍼 ON
}

void CRenderer::Free()
{
	Clear_RenderGroup();
}
