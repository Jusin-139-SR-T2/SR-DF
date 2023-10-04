#pragma once

#include "GameObject.h"

#include "Export_System.h"
#include "Export_Utility.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;

END

class CAwareness : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CAwareness)

private:
	explicit CAwareness(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CAwareness(const CAwareness& rhs);
	virtual ~CAwareness();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

	static CAwareness* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	HRESULT				Add_Component();
	virtual void		Free();
	void				Billboard();

	CGameObject* m_pTarget = nullptr;
	CRcBufferComp* m_pBufferComp = nullptr;
	CTextureComponent* m_pTextureComp = nullptr;
	CTransformComponent* m_pTransformComp = nullptr;
	CTransformComponent* m_pMonsterTransformComp = nullptr;

public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pMonsterTransformComp, MonsterTransformComponent, GET, SET)
	GETSET_EX2(CGameObject*, m_pTarget, Target, GET, SET)

	//기본셋팅 끝
private:
	_float m_fFrame = 0;
	_float m_fFrameEnd;

	_bool m_bMonsterAware = false;
	HRESULT ShowImg(const _float& fTimeDelta);


};

