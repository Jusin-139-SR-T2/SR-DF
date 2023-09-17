#pragma once

#include "GameObject.h"

BEGIN(Engine)

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
	void		Add_RenderGroup(RENDERID eType, CGameObject* pGameObject);
	void		Render_GameObject(LPDIRECT3DDEVICE9& pGraphicDev);
	void		Clear_RenderGroup();

public:
	void		Render_Priority(LPDIRECT3DDEVICE9& pGraphicDev);
	void		Render_NonAlpha(LPDIRECT3DDEVICE9& pGraphicDev);
	void		Render_Alpha(LPDIRECT3DDEVICE9& pGraphicDev);
	void		Render_UI(LPDIRECT3DDEVICE9& pGraphicDev);

private:
	list<CGameObject*>		m_RenderGroup[RENDER_END];

private:
	virtual void		Free();

};

END