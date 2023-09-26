#pragma once

#include "Base.h"
#include "Engine_Define.h"


#include "RigidBody.h"
#include "CollisionPrimitive.h"
#include "Contact.h"

BEGIN(Engine)

/// <summary>
/// 물리 프로세스가 진행되는 3D 공간
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
	_bool						m_bCalculateIterations;			// 계산 반복

	list<FRigidBody*>			m_listBody;						// 충돌체 리스트

	FContactResolver			m_ContactResolver;				// 접촉 해결자
	list<FContactGenerator*>	m_ConGenList;					// 접촉 발생기 리스트
	FContact*					m_pContacts;					// 추가 접촉처리가 필요한 객체에 대한 포인터

	_uint						m_iMaxContacts;					// 최대 접촉 개수
};

END