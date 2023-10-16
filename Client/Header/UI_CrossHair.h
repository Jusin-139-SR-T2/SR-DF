#pragma once

#include "UI.h"
#include "Engine_Macro.h"

#include "BlackBoard_Player.h"
#include "BlackBoardPtr.h"
#include "BlackBoardMgr.h"

#include "Export_System.h"
#include "Export_Utility.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;

END

/// <summary>
/// 화면 중앙 크로스헤어용 
/// </summary>
class CUI_CrossHair : public CUI
{
	DERIVED_CLASS(CUI, CUI_CrossHair)
protected:
	explicit CUI_CrossHair(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CUI_CrossHair(const CUI_CrossHair& rhs);
	virtual ~CUI_CrossHair();

public:
	static CUI_CrossHair* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private: 
	virtual void Free();

public:
	virtual HRESULT Ready_GameObject() override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject() override;
	virtual void Render_GameObject() override;

private:
	HRESULT Add_Component();

public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)

private:
	CRcBufferComp*			m_pBufferComp = nullptr;
	CTransformComponent*	m_pTransformComp = nullptr;
	CTextureComponent*		m_pTextureComp = nullptr;

public:
	_bool				m_bPlayerEquipGunState;	// 플레이어가 총을쥐고있을때
	_bool				m_bPlayerAttackState;	// 플레이어가 공격할때 (주먹, 장비, 발차기 등)
	_bool				m_bPlayerPickUpState;	// 들것 들고있을때 - 시체 
	_bool				m_bPlayerDetect;		// 플레이어 일점범위내 몬스터있을때 
	
	void				Change_Texture();



private:
	void	Update_InternalData();

protected:
	FBlackBoardPtr<CBlackBoard_Player>	m_wpBlackBoard_Player;	// 블랙보드 플레이어

private:
	_float				m_fX, m_fY, m_fSizeX, m_fSizeY;
	_matrix				m_ViewMatrix, m_ProjMatrix;
	_float				m_fHp;
};

// 내가 피격 - 없음
// 내 공격사거리 - 일반 + 에다가 빨간색 x
// 발차기 - 안맞아도 기본 사라지고 빨간색 점3개 회전  
// 공격 - 안맞아도 기본 사라지고 빨간색 점3개 회전 
// 강공격 준비 - 없음
// 총으로 맞아도 점 그대로 