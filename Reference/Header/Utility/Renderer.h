#pragma once

#include "GameObject.h"

BEGIN(Engine)

const _int VIEWPORT_COUNT = 8;

/// <summary>
/// 렌더러는 기존 레이어의 Rendering 역할을 부여받은 클래스로
/// 렌더링 목적에 따라 처리 함수를 달리하여 수행한다.
/// 매 프레임마다 렌더그룹에 추가해주어야 합니다.
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
	_matrix						m_matOrtho;				// UI용 직교투영 범위
	vector<D3DVIEWPORT9>		m_vecViewport;			// 뷰포트용 변환 행렬

};

END