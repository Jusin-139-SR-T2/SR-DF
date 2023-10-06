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
	for (auto iter = m_setBody.begin(); iter != m_setBody.end(); ++iter)
	{
		FCollisionPrimitive* pCol = static_cast<FCollisionPrimitive*>((*iter)->Get_Owner());
		pCol->Calculate_Transform();
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
			pShape->vPos = pShape->pBody->Get_Position();
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
	_int iDebugCount = 0;
	for (auto iterSrc = m_setBody.begin(); iterSrc != m_setBody.end(); ++iterSrc)
	{
		// �翬�� ����, ���� �ݺ��ڷκ��� ���� ���� �����ٰ� ����.
		for (auto iterDst = m_setBody.begin(); iterDst != m_setBody.end(); ++iterDst)
		{
			bool bCollide = false;
			if ((*iterSrc) == (*iterDst))
				continue;

			FCollisionPrimitive* pColSrc = static_cast<FCollisionPrimitive*>((*iterSrc)->Get_Owner());
			FCollisionPrimitive* pColDst = static_cast<FCollisionPrimitive*>((*iterDst)->Get_Owner());

			
				

			bCollide = FCollisionDetector::CollsionPrimitive(pColSrc, pColDst, nullptr);

			if (bCollide)
			{
				if (pColSrc->Get_CollisionMask() & pColDst->Get_CollisionLayer())
					pColSrc->Handle_CollsionEvent(pColDst->Get_Owner());
				if (pColDst->Get_CollisionMask() & pColSrc->Get_CollisionLayer())
					pColDst->Handle_CollsionEvent(pColSrc->Get_Owner());
			}
			++iDebugCount;
		}
	}
	wstringstream ss;
	wstring str;
	ss << iDebugCount;
	str = L"Physics CheckCount : " + ss.str() + L"\n";
	OutputDebugString(str.c_str());

	// ���� ���� ���� ��ȯ
	return m_iMaxContacts - iLimit;
}

FCollisionPrimitive* CPhysicsWorld3D::Test_Contact(FCollisionPrimitive* const pCollision)
{
	// ���� �浹ü�� ����Ʈ�ٵ��� �浹ü��� ���ؼ� �ϳ��� �浹�ϸ� ��ȯ�Ѵ�.
	for (auto iter = m_setBody.begin(); iter != m_setBody.end(); ++iter)
	{
		FCollisionPrimitive* pCol = static_cast<FCollisionPrimitive*>((*iter)->Get_Owner());
		if (FCollisionDetector::CollsionPrimitive(pCollision, pCol, nullptr))
			return pCol;
	}

	return nullptr;
}

list<FCollisionPrimitive*> CPhysicsWorld3D::Test_Contacts(FCollisionPrimitive* const pCollision)
{
	list<FCollisionPrimitive*> listCollision;

	// ���� �浹ü�� ����Ʈ�ٵ��� �浹ü��� ���ؼ� �ϳ��� �浹�ϸ� ��ȯ�Ѵ�.
	for (auto iter = m_setBody.begin(); iter != m_setBody.end(); ++iter)
	{
		FCollisionPrimitive* pCol = static_cast<FCollisionPrimitive*>((*iter)->Get_Owner());
		FCollisionPrimitive* pColDst = static_cast<FCollisionPrimitive*>(pCollision->Get_Owner());
		if (pCol == pColDst)
			continue;

		if (FCollisionDetector::CollsionPrimitive(pCollision, pCol, nullptr))
			listCollision.push_back(pCol);
	}

	return listCollision;
}

void CPhysicsWorld3D::Add_RigidBody(FRigidBody* pBody)
{
	auto iter = m_setBody.find(pBody);
	if (iter != m_setBody.end())
		return;
	
	m_setBody.emplace(pBody);
}

void CPhysicsWorld3D::Delete_RigidBody(FRigidBody* pBody)
{
	auto iter = m_setBody.find(pBody);
	if (iter != m_setBody.end())
		m_setBody.erase(iter);
}
