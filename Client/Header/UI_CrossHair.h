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
/// ȭ�� �߾� ũ�ν����� 
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
	_bool				m_bPlayerEquipGunState;	// �÷��̾ �������������
	_bool				m_bPlayerAttackState;	// �÷��̾ �����Ҷ� (�ָ�, ���, ������ ��)
	_bool				m_bPlayerPickUpState;	// ��� ��������� - ��ü 
	_bool				m_bPlayerDetect;		// �÷��̾� ���������� ���������� 
	
	void				Change_Texture();



private:
	void	Update_InternalData();

protected:
	FBlackBoardPtr<CBlackBoard_Player>	m_wpBlackBoard_Player;	// ������ �÷��̾�

private:
	_float				m_fX, m_fY, m_fSizeX, m_fSizeY;
	_matrix				m_ViewMatrix, m_ProjMatrix;
	_float				m_fHp;
};

// ���� �ǰ� - ����
// �� ���ݻ�Ÿ� - �Ϲ� + ���ٰ� ������ x
// ������ - �ȸ¾Ƶ� �⺻ ������� ������ ��3�� ȸ��  
// ���� - �ȸ¾Ƶ� �⺻ ������� ������ ��3�� ȸ�� 
// ������ �غ� - ����
// ������ �¾Ƶ� �� �״�� 