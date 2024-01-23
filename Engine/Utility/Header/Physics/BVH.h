#pragma once

#include "Engine_Define.h"
#include "CollisionPrimitive.h"

BEGIN(Engine)

struct FPotentialContact
{
	/**
	 * Holds the bodies that might be in contact.
	 */
	FRigidBody* body[2];
};

class FBVHNode
{
public:
	FBVHNode(FBVHNode* pParent, const FBoundingBox& box, FRigidBody* pBody = nullptr)
		: pParent(pParent), AABB(box), pBody(pBody)
	{
		pChildren[0] = pChildren[1] = nullptr;
	}
	~FBVHNode();

public:
	FBVHNode* pChildren[2] = {};

	FBoundingBox AABB;

	FRigidBody* pBody = { nullptr };

	FBVHNode* pParent = { nullptr };

public:
	_bool IsLeaf() const { return (pBody != nullptr); }
	_uint Get_PotentialContacts(FPotentialContact* pContacts, _uint iLimit) const;
	void Insert(FRigidBody* pBody, const FBoundingBox& box);

protected:
	_bool Overlaps(const FBVHNode* pOther) const;
	_uint Get_PotentialContactsWith(const FBVHNode* pOther, FPotentialContact* pContacts, _uint iLimit) const;
	void RecalculateBoundingBox(_bool bIsRecurse = true);
};

END