#pragma once
#include "Export_System.h"
#include "Export_Utility.h"
#include "Esystem.h"

class CAwareness : public Engine::CEsystem
{
	DERIVED_CLASS(CEsystem, CAwareness)

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

	void Billboard();


};

