#pragma once

#include "Base.h"
#include "Engine_Define.h"


#include "RigidBody.h"
#include "CollisionPrimitive.h"
#include "Contact.h"

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

public:
	static CPhysicsWorld3D* Create(_uint iMaxContacts, _uint iIterations);

private:
	virtual		void	Free();

public:
	HRESULT				Ready_Physics(_uint iMaxContacts, _uint iIterations);
	void				StartFrame_Physics();
	_int				Update_Physics(const Real& fTimeDelta);
	_uint				Generate_Contacts();

private:
	_bool						m_bCalculateIterations;			// ��� �ݺ�

	list<FRigidBody*>			m_listBody;						// �浹ü ����Ʈ

	FContactResolver			m_ContactResolver;				// ���� �ذ���
	list<FContactGenerator*>	m_ConGenList;					// ���� �߻��� ����Ʈ
	FContact*					m_pContacts;					// �߰� ����ó���� �ʿ��� ��ü�� ���� ������

	_uint						m_iMaxContacts;					// �ִ� ���� ����
};

END