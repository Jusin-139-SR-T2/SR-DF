#pragma once
#include "AttackUnion.h"
#include "Brown.h"
#include "Gray.h"
#include "AceBoss.h"

#include "BlackBoard_Monster.h"
#include "BlackBoardPtr.h"
#include "BlackBoardMgr.h"

class CAwareness : public CAttackUnion
{
	DERIVED_CLASS(CAttackUnion, CAwareness)

	PUBLIC enum class TYPE {BROWN, GRAY, BOSS, TYPE_END};

private:
	explicit CAwareness(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CAwareness(const CAwareness& rhs);
	virtual ~CAwareness();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

	static CAwareness* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z, CAwareness::TYPE pType, CGameObject* pOwner);

private:
	HRESULT				Add_Component();
	virtual void		Free();
	HRESULT				Billboard_Aware();

public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)

private:
	HRESULT Set_Speed(CAwareness::TYPE pType, CGameObject* pOwner);
	CAwareness::TYPE m_eType;

	_vec3		vPlayerPos;
	_bool		m_bTrigger = FALSE;
	_bool		m_bBossConnect = FALSE;
	_bool		m_bMissTarget = FALSE;

	_float m_fAwareness = 0.f; // 현재값
	_float m_fMaxAwareness = 0.f; // max값
	_float m_fAwarenessPrev = 0.f; // 이전값

	CBrown*		pBrown = nullptr;
	CGray*		pGray = nullptr;
	CAceBoss*	pBoss = nullptr;

	void Update_Awareness(CAwareness::TYPE pType);
	//블랙보드
private:
	void	Update_BlackBoard(); // 블랙보드로부터 데이터를 받아오는용도 

protected:
	FBlackBoardPtr<CBlackBoard_Monster>	m_wpBlackBoard_Monster;	// 블랙보드

};
	

