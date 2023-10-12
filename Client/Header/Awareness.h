#pragma once
#include "AttackUnion.h"
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
	HRESULT Billboard_Aware();

public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)

private:
	_float		m_fFrame;
	_float		m_fFrameEnd;
	_float		m_fFrameSpeed;
	_vec3		vPlayerPos;
	_float 		m_fAwareness;
	_bool		m_bTrigger = FALSE;
	_float		m_fAge;
	_float		m_fLifeTime;
	_tchar		debugString[100];

	CAwareness::TYPE m_eType;
	void Set_Speed(CAwareness::TYPE pType);

	//������� 
private:
	void	Update_BlackBoard(); // ������κ��� �����͸� �޾ƿ��¿뵵 

protected:
	FBlackBoardPtr<CBlackBoard_Monster>	m_wpBlackBoard_Monster;	// ������

private:
	GAUGE<_float> m_fBossAwareness;
	_float m_fBossAwarenessPrevCur;
	_bool m_bBossConnect = FALSE;
	_bool m_bMissTarget = FALSE;
};
	

