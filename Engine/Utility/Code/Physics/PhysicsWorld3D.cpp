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
	// 힘 제거, 완료
	for (auto iter = m_listBody.begin(); iter != m_listBody.end(); ++iter)
	{
		(*iter)->Clear_Accumulators();
		(*iter)->CalculateDerivedData();
	}
}

_int CPhysicsWorld3D::Update_Physics(const Real& fTimeDelta)
{
	// 힘 더하기
	for (auto iter = m_listBody.begin(); iter != m_listBody.end(); ++iter)
	{
		(*iter)->Integrate(fTimeDelta);
	}


	// 접촉 발생기
	_uint iUsedContacts = Generate_Contacts();

	// 힘 발생기와 접촉 발생기로 나온 결과로 충돌을 해결
	if (m_bCalculateIterations)
		m_ContactResolver.Set_Iterations(iUsedContacts * 4);
	m_ContactResolver.ResolveContacts(m_pContacts, iUsedContacts, fTimeDelta);

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
	for (auto iter = m_listBody.begin(); iter != m_listBody.end(); ++iter)
	{
		//FCollisionDetector::SphereAndSphere()
	}

	// 사용된 접촉 수를 반환
	return m_iMaxContacts - iLimit;
}
