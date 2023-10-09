#pragma once

#include "BlackBoard.h"

class CPlayer;

/// <summary>
/// �̴ϼȶ������ ����ü
/// </summary>
struct FBlackBoard_PlayerInit
{
	CPlayer* pPlayer;
};


/// <summary>
/// �÷��̾� ���� ������
/// �÷��̾�� �ʿ��� ������� ���⿡ ������Ʈ ����
/// </summary>
class CBlackBoard_Player : public CBlackBoard
{
	DERIVED_CLASS(CBlackBoard, CBlackBoard_Player)
	enum class STATE_RIGHTHAND { NONE, HAND, RUN_HAND, GUN, THOMPSON, STEELPIPE, BEERBOTLE, FRYINGPAN, KICK };

protected:
	explicit CBlackBoard_Player();
	explicit CBlackBoard_Player(const CBlackBoard_Player& rhs);
	virtual ~CBlackBoard_Player();

public:
	static CBlackBoard_Player* Create();

private:
	virtual void		Free();

protected:
	virtual HRESULT Ready_BlackBoard() override { return S_OK; }
	// �ʱ�ȭ�� ����ü�� ����� �Ű������� �ް� �Ѵ�.
	virtual HRESULT Ready_BlackBoard(const FBlackBoard_PlayerInit& tInit);

public:		// �÷��̾ ���� ������ ���� ���� 
	GETSET_EX1(CPlayer*, m_pPlayer, Player, GET)

private:
	CPlayer*			m_pPlayer = nullptr;

public:		// ������ ������ ������ �鶧�� �ش� �Լ��� ���� �����.
	GETSET_EX2(GAUGE<_float>, m_fHP, HP, GET_C_REF, GET_REF)
	GETSET_EX2(GAUGE<_float>, m_fStamina, Stamina, GET_C_REF, GET_REF)

private:
	GAUGE<_float>		m_fHP;
	GAUGE<_float>		m_fStamina;
};

