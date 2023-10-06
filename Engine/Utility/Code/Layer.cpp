#include "Layer.h"

CLayer::CLayer()
	: m_fPriority(0.f)
{

}

CLayer::~CLayer()
{

}

CLayer* CLayer::Create(_float fPriority)
{
	CLayer* pLayer = new CLayer;

	if (FAILED(pLayer->Ready_Layer(fPriority)))
	{
		Safe_Release(pLayer);

		MSG_BOX("Layer Create Failed");
		return nullptr;
	}

	return pLayer;
}

void CLayer::Free()
{
	// ������Ʈ �����̳ʸ� ����, ������Ʈ�� �����Ѵ�.
	for_each(m_mapObject.begin(), m_mapObject.end(), CDeleteMap());
	m_mapObject.clear();
}

HRESULT CLayer::Ready_Layer(_float fPriority)
{
	m_fPriority = fPriority;

	return S_OK;
}

_int CLayer::Update_Layer(const _float& fTimeDelta)
{
	// Set Dead�� ������ ��ü�� ������Ų��.
	for (auto iter = m_mapObject.begin(); iter != m_mapObject.end();)
	{
		if ((*iter).second->Get_IsDead())
		{
			// ���۷��� ī���Ͱ� 1�̻��̾��� �� �߰����� ���� �۾��� �ʿ��ϴ�.
			_uint iRefCount = Safe_Release((*iter).second);
			iter = m_mapObject.erase(iter);
		}
		else
			++iter;
	}

	// �켱�� ������ ���ش�.
	for (_uint i = 0; i < static_cast<_uint>(EPRIORITY::RENDER); i++)
	{
		m_arrvecPriorityObject[i].reserve(m_mapObject.size());

		// vector�� ��ü �߰�
		for (auto& item : m_mapObject)
			m_arrvecPriorityObject[i].push_back(item.second);

		// �켱�� ��� ����
		sort(m_arrvecPriorityObject[i].begin(), m_arrvecPriorityObject[i].end(),
			[&i](CGameObject* const pDst, CGameObject* const pSrc) {
				if (!pDst->Get_IsUsePriority(i) && pDst->Get_IsUsePriority(i))
					return true;
				else if (!pDst->Get_IsUsePriority(i) && !pDst->Get_IsUsePriority(i))
					return false;
				return (pDst->Get_Priority(i) > pSrc->Get_Priority(i));
			});
	}

	// �� ���̾ ���� ���� ������Ʈ�� ������Ʈ �Ѵ�.
	_int	iResult = 0;
	for (auto& gameObj : m_arrvecPriorityObject[static_cast<_uint>(EPRIORITY::UPDATE)])
	{
		iResult = gameObj->Update_GameObject(fTimeDelta);

		if (iResult & 0x80000000)
			return iResult;
	}

	// ���� ���� Ŭ����
	m_arrvecPriorityObject[static_cast<_uint>(EPRIORITY::UPDATE)].clear();

	return iResult;
}

void CLayer::LateUpdate_Layer()
{
	for (auto& gameObj : m_arrvecPriorityObject[static_cast<_uint>(EPRIORITY::LATE)])
	{
		gameObj->LateUpdate_GameObject();
	}

	// ���� ���� Ŭ����
	m_arrvecPriorityObject[static_cast<_uint>(EPRIORITY::LATE)].clear();
}

CComponent* CLayer::Get_Component(COMPONENTID eID, const _tchar* pObjTag, const _tchar* pComponentTag)
{
	// ���̾� -> ������Ʈ -> ������Ʈ
	//auto	iter = find_if(m_mapObject.begin(), m_mapObject.end(), CTag_Finder(pObjTag));
	auto iter = m_mapObject.find(pObjTag);

	if (iter == m_mapObject.end())
		return nullptr;

	return iter->second->Get_Component(eID, pComponentTag);
}

HRESULT CLayer::Add_GameObject(const _tchar* pObjTag, CGameObject* pGameObject)
{
	// �� ���̾ ���� ������Ʈ�� �����Ѵ�.
	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	m_mapObject.insert({ pObjTag, pGameObject });

	return S_OK;
}

CGameObject* CLayer::Get_GameObject(const _tchar* pObjTag)
{
	CGameObject* pObj = m_mapObject[pObjTag];

	return pObj;
}

HRESULT CLayer::LateCreate_Object(const _tchar* pObjTag, CGameObject* pGameObject)
{
	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	m_mapObject.insert({ pObjTag, pGameObject });

	return S_OK;
}
