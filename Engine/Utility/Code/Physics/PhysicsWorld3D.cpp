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

	// �� ����, �Ϸ�
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

	// �� ���ϱ�
	/*for (auto iter = m_listBody.begin(); iter != m_listBody.end(); ++iter)
	{
		(*iter)->Integrate(fTimeDelta);
	}*/


	// ��ü ��ġ ������� �浹ü ��ġ ����
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


	// ���� �߻���
	_uint iUsedContacts = Generate_Contacts();



	//// �� �߻���� ���� �߻���� ���� ����� �浹�� �ذ�
	//if (m_bCalculateIterations)
	//	m_ContactResolver.Set_Iterations(iUsedContacts * 4);
	//m_ContactResolver.ResolveContacts(m_pContacts, iUsedContacts, fTimeDelta);

	return 0;
}

_uint CPhysicsWorld3D::Generate_Contacts()
{
	_uint iLimit = m_iMaxContacts;
	FContact* pNextContact = m_pContacts;

	// �浹�߻�
	//for (auto iter = m_ConGenList.begin(); iter != m_ConGenList.end(); ++iter)
	//{
	//	_uint iUsed = (*iter)->Add_Contact(pNextContact, iLimit);
	//	iLimit -= iUsed;
	//	pNextContact += iUsed;

	//	// ����Ʈ�� �ʰ��ϸ� �� �̻� ����ó���� ���� �ʴ´�.
	//	if (iLimit <= 0) break;
	//}

	// �浹 ����ȭ, ���� �߰� ����
	// ��ȹ�� �浹��ü�� Ʈ���� ����� �ε��� �� ���� ��ü�� ���� ó���ϴ� ��.
	// ���⼭ �߻���Ų �浹�� ���� ���� �������� �߻��ϴ� 


	// ���� ������ �� ��� �ϸ� �� �� ����.
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

	// ���� ���� ���� ��ȯ
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
