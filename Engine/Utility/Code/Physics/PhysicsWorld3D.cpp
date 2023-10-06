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
	_int iDebugCount = 0;
	for (auto iterSrc = m_setBody.begin(); iterSrc != m_setBody.end(); ++iterSrc)
	{
		// 재연산 방지, 현재 반복자로부터 다음 것을 가져다가 쓴다.
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

	// 사용된 접촉 수를 반환
	return m_iMaxContacts - iLimit;
}

FCollisionPrimitive* CPhysicsWorld3D::Test_Contact(FCollisionPrimitive* const pCollision)
{
	// 받은 충돌체로 리스트바디의 충돌체들과 비교해서 하나라도 충돌하면 반환한다.
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

	// 받은 충돌체로 리스트바디의 충돌체들과 비교해서 하나라도 충돌하면 반환한다.
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
