#pragma once

#include "Base.h"
#include "Engine_Define.h"


#include "RigidBody.h"

BEGIN(Engine)

/// <summary>
/// ���� ���μ����� ����Ǵ� 3D ����
/// </summary>
class ENGINE_DLL CPhysicsWorld3D : public CBase
{
	DERIVED_CLASS(CBase, CPhysicsWorld3D)

private:
	explicit CPhysicsWorld3D();
	virtual ~CPhysicsWorld3D();

private:
	virtual		void	Free();

public:
	HRESULT				Ready_Physics();
	_int				Update_Physics();

public:
	//void				Add_Shape(FPHY_Shape* pShape);

private:
	list<FRigidBody*>			m_BodyList;		// �浹ü ����Ʈ
};

END