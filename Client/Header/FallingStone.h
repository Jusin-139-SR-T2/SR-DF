#pragma once
#include "GameObject.h"

#include "Export_System.h"
#include "Export_Utility.h"
#include "Engine_Enum.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CColliderComponent;
class CTransformComponent;

END

class CPlayer;

class CFallingStone : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CFallingStone)

private:
	explicit CFallingStone(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CFallingStone(const CFallingStone& rhs);
	virtual ~CFallingStone();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

	static CFallingStone* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z);

private:
	HRESULT				Add_Component();
	virtual void		Free();
	HRESULT				Billboard();

	CRcBufferComp* m_pBufferComp = nullptr;
	CTextureComponent* m_pTextureComp = nullptr;
	CColliderComponent* m_pColliderComp = nullptr;
	CTransformComponent* m_pTransformComp = nullptr;

public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CColliderComponent*, m_pColliderComp, ColliderComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	
		// Ãæµ¹ -----------------------------------------------------------------
protected:
	virtual void	OnCollision(CGameObject* pDst);
	virtual void	OnCollisionEntered(CGameObject* pDst);
	virtual void	OnCollisionExited(CGameObject* pDst);
	PRIVATE FCollisionSphere* pShape;

private:
	_float m_fFrame;
	_float m_fFrameEnd;
	_float m_fFrameSpeed;

	_vec3 m_vOrigin;
	_vec3 vFall;

	_float m_fFallingSpeed;
	_bool m_bFall = TRUE; 

	_float m_fAge;
	_float m_fLifeTime;

	_bool m_bOld = FALSE;
	_bool m_bCollision = FALSE;
	void Falling(const _float& fTimeDelta);

	wstring CollideName;
};

