#include "Renderer.h"

IMPLEMENT_SINGLETON(CRenderer)

CRenderer::CRenderer()
{
}

CRenderer::~CRenderer()
{
	Free();
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
	// ����ó���� �ϴ� ������ ���� ���� ��Ƽ� ó���Ѵ�.
	Render_Priority(pGraphicDev);
	Render_AlphaTest(pGraphicDev); // + ���� �߰�
	Render_NonAlpha(pGraphicDev);
	Render_Alpha(pGraphicDev);
	Render_UI(pGraphicDev);

	// �׻� ó�� �� ���� �������� ���� �ʱ�ȭ��Ų��.
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

// + ���� �߰�
void CRenderer::Render_AlphaTest(LPDIRECT3DDEVICE9& pGraphicDev)
{
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

	for (auto& iter : m_RenderGroup[RENDER_ALPHA])
		iter->Render_GameObject();

	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void CRenderer::Render_UI(LPDIRECT3DDEVICE9& pGraphicDev)
{
	//D3DVIEWPORT9 m_BackupViewPort;
	//pGraphicDev->GetViewport(&m_BackupViewPort); // ���� �� ��Ʈ ���

	// UI�� ���ο� �� ��Ʈ ���� �� ����
	D3DVIEWPORT9 UiViewPort;
	UiViewPort.X = 0;
	UiViewPort.Y = 0;
	UiViewPort.Width = WINCX;
	UiViewPort.Height = WINCY;
	UiViewPort.MinZ = 0;
	UiViewPort.MaxZ = 0;
	pGraphicDev->SetViewport(&UiViewPort);

	_matrix matView;
	pGraphicDev->SetTransform(D3DTS_VIEW, D3DXMatrixIdentity(&matView)); // �׵���ķ� ����� �� ��� �ʱ�ȭ.

	_matrix   m_matProj;
	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, UiViewPort.MinZ, UiViewPort.MaxZ);
	//D3DXMatrixOrthoOffCenterLH(&m_matProj, 0, WINCX, 0, WINCY, -1.f, 1.f);
	//pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);   // �������� ��� ����.

	//pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);    // Z���� OFF

	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE); // ���ķ����� ON
	pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	for (auto& iter : m_RenderGroup[RENDER_UI])
		iter->Render_GameObject();

	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE); // ���ķ����� OFF

	//pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);      // Z���� ON

	//D3DXMatrixPerspectiveFovLH(&m_matProj, D3DXToRadian(60.f), (_float)WINCX / WINCY, 0.1f, 1000.f);
	//pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);    // UI ��ü ��� �� �ٽ� �������� ��� ����.

	//pGraphicDev->SetViewport(&m_BackupViewPort);                // UI ��ü ��� �� ����ص� ���� ����Ʈ�� �ǵ���.
}

void CRenderer::Free()
{
	Clear_RenderGroup();
}
