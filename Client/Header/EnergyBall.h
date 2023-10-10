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
class CCalculatorComponent;

END

class CPlayer;

class CEnergyBall : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CEnergyBall)

private:
	explicit CEnergyBall(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CEnergyBall(const CEnergyBall& rhs);
	virtual ~CEnergyBall();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

	static CEnergyBall* Create(LPDIRECT3DDEVICE9 pGraphicDev,
								_float _x, _float _y, _float _z, MonsterPhase _CurrPhase);

private:
	HRESULT				Add_Component();
	virtual void		Free();
	HRESULT				Billboard();

	CRcBufferComp* m_pBufferComp = nullptr;
	CTextureComponent* m_pTextureComp = nullptr;
	CColliderComponent* m_pColliderComp = nullptr;
	CTransformComponent* m_pTransformComp = nullptr;
	CCalculatorComponent* m_pCalculatorComp = nullptr;

public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CColliderComponent*, m_pColliderComp, ColliderComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CCalculatorComponent*, m_pCalculatorComp, CalculatorComponent, GET, SET)

	// 충돌 -----------------------------------------------------------------
protected:
	virtual void	OnCollision(CGameObject* pDst);
	virtual void	OnCollisionEntered(CGameObject* pDst);
	virtual void	OnCollisionExited(CGameObject* pDst);
	PRIVATE FCollisionSphere* pShape;

private:
	_float m_fFrame;
	_float m_fFrameEnd;
	_float m_fFrameSpeed;

	_float m_fFrameCnt;
	_vec3 m_vOrigin;

	_float m_fMovingSpeed; 

	_float m_fAge;
	_float m_fLifeTime; 

	_vec3 Dir;
	MonsterPhase m_eCurrPhase;

	HRESULT Follow_Player(const _float fTimeDelta, MonsterPhase _phase);
	void Value_Setting(_float _x, _float _y, _float _z, MonsterPhase _phase);

	wstring CollideName;
};


// 플레이어 유도 스킬
