#pragma once
#include "AceMonster.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "Engine_Macro.h"

#include "MonsterPunch.h"

#include "Awareness.h"
#include "RedLaser.h"
#include "FallingStone.h"
#include "SlowThunder.h"
#include "EnergyBall.h"
#include "SpawnFire.h"
#include "BlueBuff.h"

class CAceBoss : public CAceMonster
{
	DERIVED_CLASS(CAceMonster, CAceBoss)

private:
	explicit CAceBoss(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CAceBoss(const CAceBoss& rhs);
	virtual ~CAceBoss();

public:
	virtual HRESULT Ready_GameObject() override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject() override;
	virtual void	Render_GameObject() override;

	static CAceBoss* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z);

private:
	HRESULT				Add_Component();
	virtual void		Free();

};