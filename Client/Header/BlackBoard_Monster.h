#pragma once

#include "BlackBoard.h"

class CBrown;
class CGray;
class CBoss;

/// <summary>
/// �̴ϼȶ������ ����ü
/// </summary>
struct FBlackBoard_MonsterInit
{
	CBrown* pBrown;
	CGray* pGray;
	CBoss* pBoss;
};


/// <summary>
/// �÷��̾� ���� ������
/// �÷��̾�� �ʿ��� ������� ���⿡ ������Ʈ ����
/// </summary>
class CBlackBoard_Monster : public CBlackBoard
{
	DERIVED_CLASS(CBlackBoard, CBlackBoard_Monster)

protected:
	explicit CBlackBoard_Monster();
	explicit CBlackBoard_Monster(const CBlackBoard_Monster& rhs);
	virtual ~CBlackBoard_Monster();

public:
	static CBlackBoard_Monster* Create();

private:
	virtual void		Free();

protected:
	virtual HRESULT Ready_BlackBoard() override { return S_OK; }
	// �ʱ�ȭ�� ����ü�� ����� �Ű������� �ް� �Ѵ�.
	virtual HRESULT Ready_BlackBoard(const FBlackBoard_MonsterInit& tInit);



public:		// ������ ���� ���� 
	GETSET_EX1(CBrown*, m_pBrown, Brown, GET)
	GETSET_EX1(CGray*, m_pGray, Gray, GET)
	GETSET_EX1(CBoss*, m_pBoss, Boss, GET)

private:
	CBrown* m_pBrown = nullptr;
	CGray* m_pGray = nullptr;
	CBoss* m_pBoss = nullptr;

public:		// ������ ������ ������ �鶧�� �ش� �Լ��� ���� �����.
	//GETSET_EX2(GAUGE<_float>, m_fHP, HP, GET_C_REF, GET_REF)
	//GETSET_EX2(GAUGE<_float>, m_fStamina, Stamina, GET_C_REF, GET_REF)
	GETSET_EX2(GAUGE<_float>, m_fBrownAwareness, BrownAwareness, GET_C_REF, GET_REF)
	GETSET_EX2(GAUGE<_float>, m_fGrayAwareness, GrayAwareness, GET_C_REF, GET_REF)
	GETSET_EX2(GAUGE<_float>, m_fBossAwareness, BossAwareness, GET_C_REF, GET_REF)


private:
	//GAUGE<_float>		m_fHP;
	//GAUGE<_float>		m_fStamina;
	GAUGE<_float>		m_fBrownAwareness;
	GAUGE<_float>		m_fGrayAwareness;
	GAUGE<_float>		m_fBossAwareness;
};

