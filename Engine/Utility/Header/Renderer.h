#pragma once

#include "GameObject.h"

BEGIN(Engine)

// 렌더타겟용 뷰포트 열거체
enum class EVIEWPORT : _uint
{
	ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, SIZE
};

// 렌더타겟용 뷰포트 열거체
enum class EVIEWPORT_RT : _uint
{
	NORMAL, DEPTH, HEIGHT, SIZE
};

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
	_matrix						m_matOrtho;				// UI용 직교투영 범위
	vector<D3DVIEWPORT9>		m_vecViewport;			// 일반 뷰포트 세팅
	vector<D3DVIEWPORT9>		m_vecViewport_RT;		// 렌더 타겟 뷰포트

};

END