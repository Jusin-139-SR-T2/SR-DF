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
	// �� ����, �Ϸ�
	for (auto iter = m_listBody.begin(); iter != m_listBody.end(); ++iter)
	{
		(*iter)->Clear_Accumulators();
		(*iter)->CalculateDerivedData();
	}
}

_int CPhysicsWorld3D::Update_Physics(const Real& fTimeDelta)
{
	// �� ���ϱ�
	for (auto iter = m_listBody.begin(); iter != m_listBody.end(); ++iter)
	{
		(*iter)->Integrate(fTimeDelta);
	}


	// ���� �߻���
	_uint iUsedContacts = Generate_Contacts();

	// �� �߻���� ���� �߻���� ���� ����� �浹�� �ذ�
	if (m_bCalculateIterations)
		m_ContactResolver.Set_Iterations(iUsedContacts * 4);
	m_ContactResolver.ResolveContacts(m_pContacts, iUsedContacts, fTimeDelta);

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
	for (auto iter = m_listBody.begin(); iter != m_listBody.end(); ++iter)
	{
		//FCollisionDetector::SphereAndSphere()
	}

	// ���� ���� ���� ��ȯ
	return m_iMaxContacts - iLimit;
}
