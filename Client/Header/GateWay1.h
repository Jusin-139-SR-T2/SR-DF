#pragma once

#include "GameObject.h"

#include "Serialize_BaseClass.h"
#include <BlackBoard_GateWay1.h>

BEGIN(Engine)

class CCubeBufferComp;
class CTextureComponent;
class CTransformComponent;
class CColliderComponent;

END

class CGateWay1 : public CGameObject
{
	DERIVED_CLASS(CGameObject, CGateWay1)

private:
	explicit CGateWay1(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CGateWay1(const CGateWay1& rhs);
	virtual ~CGateWay1();

public:
	static CGateWay1*	Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3 vPos, const _vec3 vRot, const _vec3 vScale);
	static CGateWay1*	Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject& tObjectSerial);

private: // 함수 
	virtual void		Free();											// Release

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual HRESULT		Ready_GameObject(const _vec3 vPos, const _vec3 vRot, const _vec3 vScale);
	virtual HRESULT		Ready_GameObject(const FSerialize_GameObject& tObjectSerial);
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

public:
	virtual void		OnCollision(CGameObject* pDst, const FContact* const pContact) override;
	virtual void		OnCollisionEntered(CGameObject* pDst, const FContact* const pContact) override;
	virtual void		OnCollisionExited(CGameObject* pDst) override;

public:
	GETSET_EX2(CCubeBufferComp*, m_pCubeBufferComp, CubeBufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)

private:
	CCubeBufferComp* m_pCubeBufferComp = nullptr;
	CTextureComponent* m_pTextureComp = nullptr;
	CTransformComponent* m_pTransformComp = nullptr;
	CColliderComponent* m_pColliderComp = nullptr;


private:
	HRESULT				Add_Component();								// 컴포넌트 추가 

private:
	_bool				m_bIsStarted = false;
	_bool				m_bIsClosed = false;
	_bool				m_bIsEnded = false;
	_bool				m_bIsUnused = false;

private:
	void				Update_BlackBoard();

private:
	FBlackBoardPtr<CBlackBoard_GateWay1>		m_wpBlackBoard_GateWay;
};

