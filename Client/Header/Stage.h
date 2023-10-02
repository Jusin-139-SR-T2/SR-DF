#pragma once

#include "Scene.h"

#include "Player.h"
#include "Brown.h"
#include "Gray.h"
#include "Boss.h"
#include "AceObjectFactory.h"
#include "AceLighter.h"

#include "Terrain.h"
#include "SnowParticle.h"

class CStage : public Engine::CScene
{
	DERIVED_CLASS(CScene, CStage)

private:
	explicit CStage(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStage();

public:
	static CStage*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void		Free();

public:
	virtual HRESULT		Ready_Scene();
	virtual _int		Update_Scene(const _float& fTimeDelta);
	virtual void		LateUpdate_Scene();
	virtual void		Render_Scene();

private:
	HRESULT				Ready_LightInfo();
	HRESULT				Ready_SpotLightInfo();

	HRESULT				Ready_Layer_Environment(const _tchar* pLayerTag);
	HRESULT				Ready_Layer_GameLogic(const _tchar* pLayerTag);
	HRESULT				Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT				Ready_Layer_UI(const _tchar* pLayerTag);

protected:
	virtual HRESULT		Ready_Layer_Completed();


};

