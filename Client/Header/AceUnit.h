#pragma once
#include "AceGameObject.h"

/// <summary>
/// Player, Monster, Attack이 모두 받아서 사용 
/// </summary>
class CAceUnit : public CAceGameObject
{
	DERIVED_CLASS(CAceGameObject, CAceUnit)

protected:
	explicit CAceUnit(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CAceUnit(const CAceUnit& rhs);
	virtual ~CAceUnit();

protected:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;
	virtual void		Free() override;

	// 성희 추가 : MeshColider(Box, SphereColider)
	virtual void		MeshBoxColider(FLOAT _Width, FLOAT _Height, FLOAT Depth) override;
	virtual void		MeshSphereColider(FLOAT Radius, UINT Slices, UINT Stacks) override;

protected:
	GAUGE<_float>		m_gHp;

};