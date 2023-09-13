#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;

END

class CMonster : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CMonster)
private:
	explicit CMonster(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMonster(const CMonster& rhs);
	virtual ~CMonster();

public:
	virtual HRESULT Ready_GameObject() override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject() override;
	virtual void	Render_GameObject() override;

private:
	HRESULT			Add_Component();
	void			Key_Input(const _float& fTimeDelta);

public:


private:
	CRcTex* m_pBufferComp = nullptr;
	CTransform*		m_pTransformComp = nullptr;
	CTexture*		m_pTextureComp = nullptr;
	CCalculator* m_pCalculatorComp = nullptr;

	CGameObject*	m_pTarget = nullptr;

public:
	void		Set_Target(CGameObject* pTarget) { m_pTarget = pTarget; }
	static CMonster* Create(LPDIRECT3DDEVICE9 pGraphicDev);

	_matrix m_matRotAxis;

private:
	virtual void	Free();
};

