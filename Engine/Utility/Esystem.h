#pragma once
#include "GameObject.h"
#include "Engine_Define.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;

class CEsystem : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CEsystem)

private:
	explicit CEsystem(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CEsystem(const CEsystem& rhs);
	virtual ~CEsystem();

public:
	virtual HRESULT		Ready_GameObject(const _tchar* _stateKey);
	virtual _int		Update_GameObject(const _float& fTimeDelta);
	virtual void		LateUpdate_GameObject();
	virtual void		Render_GameObject() override;

private:
	HRESULT				Add_Component();
	virtual void		Free();
	void				Billboard();


public:
	CGameObject* m_pTarget = nullptr;
	CRcBufferComp* m_pBufferComp = nullptr;
	CTextureComponent* m_pTextureComp = nullptr;
	CTransformComponent* m_pTransformComp = nullptr;

public:
	GETSET_EX2(_bool, m_bEffectOn, EffectOn, GET, SET)
	GETSET_EX2(CGameObject*, m_pTarget, GameObject, GET, SET)
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
		
private:
	_float m_fFrame = 0; // 프레임시작 - 0부터 ~
	_float m_fFrameEnd; // 프레임 끝

	// 변수 - 직교투영해야하는 이펙트 용도 ==========
	_float				m_fX, m_fY, m_fSizeX, m_fSizeY;
	_matrix				m_ViewMatrix, m_ProjMatrix;
	_float				m_fHp;

	// 체크용
	_bool				m_bEffectOn = false;
};

END