#pragma once

#include "GameObject.h"

BEGIN(Engine)

const _int VIEWPORT_COUNT = 8;

/// <summary>
/// �������� ���� ���̾��� Rendering ������ �ο����� Ŭ������
/// ������ ������ ���� ó�� �Լ��� �޸��Ͽ� �����Ѵ�.
/// �� �����Ӹ��� �����׷쿡 �߰����־�� �մϴ�.
/// </summary>
class ENGINE_DLL CRenderer : public CBase
{
	DERIVED_CLASS_SINGLETON(CBase, CRenderer)

private:
	explicit CRenderer();
	virtual ~CRenderer();

public:
	void		Ready_Renderer();
	void		Add_RenderGroup(RENDERID eType, CGameObject* pGameObject);
	void		Render_GameObject(LPDIRECT3DDEVICE9& pGraphicDev);
	void		Clear_RenderGroup();

public:
	void		Render_Priority(LPDIRECT3DDEVICE9& pGraphicDev);
	void		Render_AlphaTest(LPDIRECT3DDEVICE9& pGraphicDev);
	void		Render_NonAlpha(LPDIRECT3DDEVICE9& pGraphicDev);
	void		Render_Alpha(LPDIRECT3DDEVICE9& pGraphicDev);
	void		Render_UI(LPDIRECT3DDEVICE9& pGraphicDev);

private:
	list<CGameObject*>		m_RenderGroup[RENDER_END];

private:
	virtual void		Free();

public:
	GETSET_EX1(_matrix,					m_matOrtho,		MatOrtho, GET)
	GETSET_EX1(vector<D3DVIEWPORT9>,	m_vecViewport,	VecViewport, GET)

private:
	_matrix						m_matOrtho;				// UI�� �������� ����
	vector<D3DVIEWPORT9>		m_vecViewport;			// ����Ʈ�� ��ȯ ���

};

END