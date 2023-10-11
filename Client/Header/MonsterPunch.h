#pragma once
#include "GameObject.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "Player.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;
class CCalculatorComponent;
class CColliderComponent;
END

class CMonsterPunch : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CMonsterPunch)

	PUBLIC enum class TYPE { NORMAL, HEAVY, TYPE_END };

private:
	explicit CMonsterPunch(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMonsterPunch(const CMonsterPunch& rhs);
	virtual ~CMonsterPunch();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

	static CMonsterPunch* Create(LPDIRECT3DDEVICE9 pGraphicDev, 
								_float _x, _float _y, _float _z, TYPE _option, CAceUnit* pOwner);

private:
	HRESULT				Add_Component();
	virtual void		Free();
	void				Billboard(const _float& fTimeDelta);

	CRcBufferComp* m_pBufferComp = nullptr;
	CTextureComponent* m_pTextureComp = nullptr;
	CColliderComponent* m_pColliderComp = nullptr;
	CTransformComponent* m_pTransformComp = nullptr;
	CTransformComponent* m_pPlayerTransformcomp = nullptr; //플레이어용도

public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CColliderComponent*, m_pColliderComp, ColliderComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)

		// 충돌 -----------------------------------------------------------------
protected:
	virtual void	OnCollision(CGameObject* pDst);
	virtual void	OnCollisionEntered(CGameObject* pDst);
	virtual void	OnCollisionExited(CGameObject* pDst);


private:
	_float			m_fAge;
	_float			m_fLifeTime;
	GAUGE<_float>	PlayerHp;
	TYPE			m_eAttackType;

};

//플레이어 = -20 : 주먹
// 91 84 76 69 61 