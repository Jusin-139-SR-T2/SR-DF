#pragma once

#include "Scene.h"

class CScene_MapTool : public Engine::CScene
{
	DERIVED_CLASS(CScene, CScene_MapTool)
private:
	explicit CScene_MapTool(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CScene_MapTool();

public:
	static CScene_MapTool* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void		Free();

public:
	virtual HRESULT		Ready_Scene();
	virtual _int		Update_Scene(const _float& fTimeDelta);
	virtual void		LateUpdate_Scene();
	virtual void		Render_Scene();

protected:
	virtual HRESULT		Ready_Layer_Completed();


};

