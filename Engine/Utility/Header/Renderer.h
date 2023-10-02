#pragma once

#include "GameObject.h"

BEGIN(Engine)

// ����Ÿ�ٿ� ����Ʈ ����ü
enum class EVIEWPORT : _uint
{
	ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, SIZE
};

// ����Ÿ�ٿ� ����Ʈ ����ü
enum class EVIEWPORT_RT : _uint
{
	NORMAL, DEPTH, HEIGHT, SIZE
};

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

private:
	virtual void	Free();

public:
	HRESULT			Ready_Renderer(const _uint iWidth = 1200U, const _uint iHeight = 900U);
	void			Add_RenderGroup(RENDERID eType, CGameObject* pGameObject);
	void			Render_GameObject(LPDIRECT3DDEVICE9& pGraphicDev);
	void			Clear_RenderGroup();

public:
	void			Render_Priority(LPDIRECT3DDEVICE9& pGraphicDev);
	void			Render_AlphaTest(LPDIRECT3DDEVICE9& pGraphicDev);
	void			Render_NonAlpha(LPDIRECT3DDEVICE9& pGraphicDev);
	void			Render_Alpha(LPDIRECT3DDEVICE9& pGraphicDev);
	void			Render_UI(LPDIRECT3DDEVICE9& pGraphicDev);

private:
	list<CGameObject*>		m_RenderGroup[RENDER_END];

public:


public:
	GETSET_EX1(_matrix,					m_matOrtho,			MatOrtho, GET)
	GETSET_EX1(vector<D3DVIEWPORT9>,	m_vecViewport,		VecViewport, GET_C_REF)
	GETSET_EX1(vector<D3DVIEWPORT9>,	m_vecViewport_RT,	VecViewport_RenderTarget, GET_C_REF)

private:
	_matrix						m_matOrtho;				// UI�� �������� ����
	vector<D3DVIEWPORT9>		m_vecViewport;			// �Ϲ� ����Ʈ ����
	vector<D3DVIEWPORT9>		m_vecViewport_RT;		// ���� Ÿ�� ����Ʈ

};

END