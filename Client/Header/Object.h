#pragma once
#include "GameObject.h"

#include "Export_System.h"
#include "Export_Utility.h"
#include "Engine_Macro.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;

END

class CObject : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CObject)

private:
	explicit CObject(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CObject(const CObject& rhs);
	virtual ~CObject();

public:
	virtual HRESULT Ready_GameObject() override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject() override;
	virtual void	Render_GameObject() override;

	static CObject* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	CRcBufferComp* m_pBufferComp = nullptr;
	CTextureComponent* m_pTextureComp = nullptr;
	CTransformComponent* m_pTransformComp = nullptr;
	CCalculatorComponent* m_pCalculatorComp = nullptr;

private:
	void				Height_On_Terrain();
	HRESULT				Add_Component();
	virtual void		Free();
	void				Billboard();

private:
	enum class OBJECT_TYPE { NONE, TWO_HAND, TWO_OBJECT, RIGHT_OBJECT }; // 테스트(오브젝트)
	enum class OBJECT_NAME { NONE, GUN, THOMPSON, STEELPIPE, BEERBOTLE, FRYINGPAN };

	OBJECT_TYPE m_eObjectType;	// 오브젝트 타입
	OBJECT_NAME m_eObjectName;	// 오브젝트 이름
};