#pragma once
#include "UI.h"

/// <summary>
/// ���̵��� ���̵�ƿ��� 
/// add object�� priority 1�� �����ؾߵ�  ������ �켱 
/// </summary>
class CUI_SceneChange : public CUI
{
	DERIVED_CLASS(CUI, CUI_SceneChange)
protected:
	explicit CUI_SceneChange(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CUI_SceneChange(const CUI_SceneChange& rhs);
	virtual ~CUI_SceneChange();

public:
	static CUI_SceneChange* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

//---------------------------------------------------------------------------------------------
public:
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	
//---------------------------------------------------------------------------------------------
private:
	void				Update_InternalData();
	HRESULT				Add_Component();
	FRAME				m_tFrame;

};
