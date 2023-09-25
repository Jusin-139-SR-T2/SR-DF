#pragma once

#include "Base.h"
#include "Engine_Define.h"

#include "PhysicsWorld3D.h"


BEGIN(Engine)

/// <summary>
/// ���� ���踦 �����ϴ� Ŭ����
/// </summary>
class CPhysicsMgr : public CBase
{
	DERIVED_CLASS_SINGLETON(CBase, CPhysicsMgr)

private:
	explicit CPhysicsMgr();
	virtual ~CPhysicsMgr();

public:
	virtual void	Free();

public:
	HRESULT		Ready_Physics(const _uint iMaxPhysicsWorld3D);
	void		StartFrame_Physics();
	// ���� ��Ȯ���� ���� Precision�� �Ǽ� Ÿ���� ����Ѵ�.
	_int		Update_Physics(const Real& fTimeDelta);

public:
	GETSET_EX1(vector<CPhysicsWorld3D*>, m_vecWorld3D, VecWorld3D, GET_PTR)
public:
	CPhysicsWorld3D* Get_World3D(const _uint iWorldID) const
	{
		return m_vecWorld3D[iWorldID];
	}

private:
	vector<CPhysicsWorld3D*> m_vecWorld3D;
};

END