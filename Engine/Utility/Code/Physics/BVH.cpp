#include "BVH.h"

FBVHNode::~FBVHNode()
{
    if (pParent != nullptr)
    {
        FBVHNode* pSibling = { nullptr };
        if (pParent->pChildren[0] == this) pSibling = pParent->pChildren[1];
        else pSibling = pParent->pChildren[0];

        pParent->AABB = pSibling->AABB;
        pParent->pBody = pSibling->pBody;
        pParent->pChildren[0] = pSibling->pChildren[0];
        pParent->pChildren[1] = pSibling->pChildren[1];

        pSibling->pParent = nullptr;
        pSibling->pBody = nullptr;
        pSibling->pChildren[0] = nullptr;
        pSibling->pChildren[1] = nullptr;
        Safe_Delete(pSibling);

        pParent->RecalculateBoundingBox();
    }

    if (pChildren[0])
    {
        pChildren[0]->pParent = nullptr;
        Safe_Delete(pChildren[0]);
    }
    if (pChildren[1])
    {
        pChildren[1]->pParent = nullptr;
        Safe_Delete(pChildren[1]);
    }
}

_uint FBVHNode::Get_PotentialContacts(FPotentialContact* pContacts, _uint iLimit) const
{
    if (IsLeaf() || (iLimit == 0)) return 0;

    return pChildren[0]->Get_PotentialContactsWith(pChildren[1], pContacts, iLimit);
}

void FBVHNode::Insert(FRigidBody* pNewBody, const FBoundingBox& newBox)
{
    // 이 노드가 리프면, 자신과 새로운 것을 하위로 만든다.
    if (IsLeaf())
    {
        // 왼쪽엔 자신의 것을
        pChildren[0] = new FBVHNode(this, AABB, pBody);
        // 오른쪽엔 새로 들어온 것을
        pChildren[1] = new FBVHNode(this, newBox, pNewBody);

        this->pBody = nullptr;

        // 바운딩 박스 재계산
        RecalculateBoundingBox();
    }
    // 중간 계층이면 겹침 검사를 통해 어떤 계층으로 합칠지 정한다.
    else
    {
        if (pChildren[0]->AABB.Get_Growth(newBox) < pChildren[1]->AABB.Get_Growth(newBox))
            pChildren[0]->Insert(pNewBody, newBox);
        else
            pChildren[1]->Insert(pNewBody, newBox);
    }
}

_bool FBVHNode::Overlaps(const FBVHNode* pOther) const
{
    return AABB.Overlaps(&pOther->AABB);
}

_uint FBVHNode::Get_PotentialContactsWith(const FBVHNode* pOther, FPotentialContact* pContacts, _uint iLimit) const
{
    if (!Overlaps(pOther) || iLimit == 0) return 0;

    if (IsLeaf() && pOther->IsLeaf())
    {
        pContacts->body[0] = pBody;
        pContacts->body[1] = pOther->pBody;
        return 1;
    }

    if (pOther->IsLeaf() || (!IsLeaf() && AABB.Get_Size() >= pOther->AABB.Get_Size()))
    {
        _uint iCount = pChildren[0]->Get_PotentialContactsWith(pOther, pContacts, iLimit);

        if (iLimit > iCount)
        {
            return iCount + pChildren[1]->Get_PotentialContactsWith(pOther, pContacts + iCount, iLimit - iCount);
        }
        else
        {
            return iCount;
        }
    }
    else
    {
        _uint iCount = Get_PotentialContactsWith(pOther->pChildren[0], pContacts, iLimit);

        if (iLimit > iCount)
        {
            return iCount + Get_PotentialContactsWith(pOther->pChildren[1], pContacts + iCount, iLimit - iCount);
        }
        else
        {
            return iCount;
        }
    }

    return _uint();
}

void FBVHNode::RecalculateBoundingBox(_bool bIsRecurse)
{
    // 끝 노드면 재계산 안함
    if (IsLeaf()) return;

    AABB = FBoundingBox(pChildren[0]->AABB, pChildren[1]->AABB);

    if (pParent != nullptr) 
        pParent->RecalculateBoundingBox(true);
}
