#pragma once
#include "Psystem.h"
#include "Export_System.h"
#include "Export_Utility.h"

BEGIN(Engine)

class CTransformComponent;

END

class CFireWork : public Engine::CPsystem
{
	DERIVED_CLASS(CPsystem, CSnowParticle)

private:
	explicit CFireWork(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CFireWork();

public:
	HRESULT					Ready_GameObject(_vec3 vOriginPos, _int numParticles);
	_int					Update_GameObject(const _float& fTimeDelta);
	void					LateUpdate_GameObject(void);
	void					Render_GameObject() override;

	static CFireWork* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vOriginPos, _int numParticles);


private:
	void					ResetParticle(Attribute* _attribute) override;
	virtual void			Free();
	HRESULT					Add_Component();
	void					billboard();

private:
	_float	    m_fTime = 0.f;
	_float      m_fMoveTime = 20.f;
	_float      m_fDieTime = 20.f;

public:
	CTransformComponent* m_pTransformComp = nullptr;
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)

};

