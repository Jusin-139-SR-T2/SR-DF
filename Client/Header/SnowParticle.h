#pragma once
#include "Export_System.h"
#include "Export_Utility.h"
#include "Psystem.h"

BEGIN(Engine)

class CTransformComponent;

END

class CSnowParticle : public Engine::CPsystem
{
	DERIVED_CLASS(CPsystem, CSnowParticle)

private:
	explicit CSnowParticle(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CSnowParticle();

public:
	HRESULT					Ready_GameObject(_vec3 vOriginPos, _int numParticles);
	_int					Update_GameObject(const _float& fTimeDelta);
	void					LateUpdate_GameObject(void);
	void					Render_GameObject() override;

	static CSnowParticle*	Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vOriginPos, _int numParticles);


private:
	void					ResetParticle(Attribute* _attribute) override;
	virtual void			Free();
	HRESULT					Add_Component();
	void					billboard();

private:
	_float	    m_fTime = 0.f;
	_float      m_fMoveTime = 20.f;

public:
	CTransformComponent* m_pTransformComp = nullptr;
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)

};
