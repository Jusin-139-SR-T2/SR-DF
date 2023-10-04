#include "PhysicsWorld3D.h"

#include "Collide.h"

CPhysicsWorld3D::CPhysicsWorld3D()
{
}

CPhysicsWorld3D::~CPhysicsWorld3D()
{
	Free();
}

CPhysicsWorld3D* CPhysicsWorld3D::Create(_uint iMaxContacts, _uint iIterations)
{
	ThisClass* pInstance = new ThisClass();

	if (FAILED(pInstance->Ready_Physics(iMaxContacts, iIterations)))
	{
		Safe_Release(pInstance);

		MSG_BOX("PhysicsWorld3D Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CPhysicsWorld3D::Free()
{

}

HRESULT CPhysicsWorld3D::Ready_Physics(_uint iMaxContacts, _uint iIterations)
{
	m_ContactResolver.Set_Iterations(iMaxContacts, iIterations);

	return S_OK;
}

void CPhysicsWorld3D::StartFrame_Physics()
{
	if (m_bIsPaused)
		return;

	// 힘 제거, 완료
	/*for (auto iter = m_listBody.begin(); iter != m_listBody.end(); ++iter)
	{
		(*iter)->Clear_Accumulators();
		(*iter)->CalculateDerivedData();
	}*/
}

_int CPhysicsWorld3D::Update_Physics(const Real& fTimeDelta)
{
	if (m_bIsPaused)
		return 0;

	// 힘 더하기
	/*for (auto iter = m_listBody.begin(); iter != m_listBody.end(); ++iter)
	{
		(*iter)->Integrate(fTimeDelta);
	}*/


	// 강체 위치 기반으로 충돌체 위치 수정
	for (auto iter = m_listBody.begin(); iter != m_listBody.end(); ++iter)
	{
		FCollisionPrimitive* pCol = reinterpret_cast<FCollisionPrimitive*>((*iter)->Get_Owner());
		switch (pCol->Get_Type())
		{
		case ECOLLISION::SPHERE:
		{
			FCollisionSphere* pShape = dynamic_cast<FCollisionSphere*>(pCol);
			pShape->vPos = pShape->pBody->Get_Position();
			break;
		}
		case ECOLLISION::BOX:
		{
			FCollisionBox* pShape = dynamic_cast<FCollisionBox*>(pCol);
			pShape->vPos = pShape->pBody->Get_Position();
			break;
		}
		case ECOLLISION::CAPSULE:
		{
			FCollisionCapsule* pShape = dynamic_cast<FCollisionCapsule*>(pCol);
			pShape->vStart = pShape->pBody->Get_Position();
			break;
		}
		}
	}


	// 접촉 발생기
	_uint iUsedContacts = Generate_Contacts();



	//// 힘 발생기와 접촉 발생기로 나온 결과로 충돌을 해결
	//if (m_bCalculateIterations)
	//	m_ContactResolver.Set_Iterations(iUsedContacts * 4);
	//m_ContactResolver.ResolveContacts(m_pContacts, iUsedContacts, fTimeDelta);

	return 0;
}

_uint CPhysicsWorld3D::Generate_Contacts()
{
	_uint iLimit = m_iMaxContacts;
	FContact* pNextContact = m_pContacts;

	// 충돌발생
	//for (auto iter = m_ConGenList.begin(); iter != m_ConGenList.end(); ++iter)
	//{
	//	_uint iUsed = (*iter)->Add_Contact(pNextContact, iLimit);
	//	iLimit -= iUsed;
	//	pNextContact += iUsed;

	//	// 리미트를 초과하면 더 이상 접촉처리를 하지 않는다.
	//	if (iLimit <= 0) break;
	//}

	// 충돌 최적화, 추후 추가 예정
	// 계획은 충돌객체를 트리로 만들어 부딪힐 것 같은 객체에 대해 처리하는 것.
	// 여기서 발생시킨 충돌에 대한 것은 엔진에서 발생하는 


	// 여기 구조를 좀 어떻게 하면 될 것 같다.
	/*for (auto iter = m_listBody.begin(); iter != m_listBody.end(); ++iter)
	{
		FCollisionDetector::SphereAndSphere();
	}*/

	for (auto iterSrc = m_listBody.begin(); iterSrc != m_listBody.end(); ++iterSrc)
	{
		for (auto iterDst = m_listBody.begin(); iterDst != m_listBody.end(); ++iterDst)
		{
			bool	bCollide = false;
			if ((*iterSrc) == (*iterDst))
				continue;

			FCollisionPrimitive* pColSrc = reinterpret_cast<FCollisionPrimitive*>((*iterSrc)->Get_Owner());
			FCollisionPrimitive* pColDst = reinterpret_cast<FCollisionPrimitive*>((*iterDst)->Get_Owner());
			switch (pColSrc->Get_Type())
			{
			case ECOLLISION::SPHERE:
			{
				if (pColDst->Get_Type() == ECOLLISION::SPHERE)
				{
					FCollisionSphere* pShapeSrc = dynamic_cast<FCollisionSphere*>(pColSrc);
					FCollisionSphere* pShapeDst = dynamic_cast<FCollisionSphere*>(pColDst);
					bCollide = FCollisionDetector::SphereAndSphere(*pShapeSrc, *pShapeDst);
				}
				else if (pColDst->Get_Type() == ECOLLISION::BOX)
				{
					FCollisionSphere* pShapeSrc = dynamic_cast<FCollisionSphere*>(pColSrc);
					FCollisionBox* pShapeDst = dynamic_cast<FCollisionBox*>(pColDst);
					bCollide = FCollisionDetector::SphereAndBox(*pShapeSrc, *pShapeDst);
				}
				else if (pColDst->Get_Type() == ECOLLISION::CAPSULE)
				{
					/*FCollisionSphere* pShapeSrc = dynamic_cast<FCollisionSphere*>(*iterSrc);
					FCollisionSphere* pShapeDst = dynamic_cast<FCollisionSphere*>(*iterDst);
					FCollisionDetector::SphereAndCa(*pShapeSrc, *pShapeDst);*/
				}
				break;
			}
			case ECOLLISION::BOX:
			{
				if (pColDst->Get_Type() == ECOLLISION::SPHERE)
				{
					FCollisionBox* pShapeSrc = dynamic_cast<FCollisionBox*>(pColSrc);
					FCollisionBox* pShapeDst = dynamic_cast<FCollisionBox*>(pColDst);
					bCollide = FCollisionDetector::BoxAndBox(*pShapeSrc, *pShapeDst);
				}
				else if (pColDst->Get_Type() == ECOLLISION::BOX)
				{
					FCollisionBox* pShapeSrc = dynamic_cast<FCollisionBox*>(pColSrc);
					FCollisionSphere* pShapeDst = dynamic_cast<FCollisionSphere*>(pColDst);
					bCollide = FCollisionDetector::BoxAndSphere(*pShapeSrc, *pShapeDst);
				}
				break;
			}
			case ECOLLISION::CAPSULE:
			{
				
				break;

			}
			}

			if (bCollide)
			{
				if (pColSrc->Get_Event())
					{ pColSrc->Get_Event()(pColSrc->Get_Owner()); }
				if (pColDst->Get_Event())
					{ pColDst->Get_Event()(pColDst->Get_Owner()); }
			}
		}
	}

	// 사용된 접촉 수를 반환
	return m_iMaxContacts - iLimit;
}

FCollisionPrimitive* CPhysicsWorld3D::Test_Contact(FCollisionPrimitive* pCollision)
{
	switch (pCollision->Get_Type())
	{
	case ECOLLISION::SPHERE:
	{
		FCollisionSphere* pShape = dynamic_cast<FCollisionSphere*>(pCollision);
		break;
	}
	case ECOLLISION::BOX:
	{
		FCollisionBox* pShape = dynamic_cast<FCollisionBox*>(pCollision);
		break;
	}
	case ECOLLISION::CAPSULE:
	{
		FCollisionCapsule* pShape = dynamic_cast<FCollisionCapsule*>(pCollision);
		break;
	}
	}

	return pCollision;
}

list<FCollisionPrimitive*> CPhysicsWorld3D::Test_Contacts(FCollisionPrimitive* pCollision)
{
	switch (pCollision->Get_Type())
	{
	case ECOLLISION::SPHERE:
	{
		FCollisionSphere* pShape = dynamic_cast<FCollisionSphere*>(pCollision);
		break;
	}
	case ECOLLISION::BOX:
	{
		FCollisionBox* pShape = dynamic_cast<FCollisionBox*>(pCollision);
		break;
	}
	case ECOLLISION::CAPSULE:
	{
		FCollisionCapsule* pShape = dynamic_cast<FCollisionCapsule*>(pCollision);
		break;
	}
	}

	return list<FCollisionPrimitive*>();
}

void CPhysicsWorld3D::Add_RigidBody(FRigidBody* pBody)
{
	m_listBody.push_back(pBody);
}

void CPhysicsWorld3D::Delete_RigidBody(FRigidBody* pBody)
{
	auto iter = find_if(m_listBody.begin(), m_listBody.end(), [&pBody](FRigidBody* pDst) {
		return (pBody == pDst);
		});
	if (iter != m_listBody.end())
		m_listBody.erase(iter);
}
