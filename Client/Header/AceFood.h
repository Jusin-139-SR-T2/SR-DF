#pragma once
#include "AceObjectFactory.h"
#include "GameObject.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;
class CCalculatorComponent;

END

enum class FOOD_NAME { APPLE, EATENAPPLE, BANANA, BANANAPEEL, COLA, MEDIKIT, FOOD_END };

class CAceFood : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CAceFood)


private:
	explicit CAceFood(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CAceFood(const CAceFood& rhs);
	virtual ~CAceFood();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;
	static CAceFood*	Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pObjTag);

private:
	virtual void		Free();
	HRESULT				Add_Component();
	void				Height_On_Terrain();
	HRESULT				BillBoard(const _float& fTimeDelta); // 플레이어쪽으로 향하는 함수 
	void				FoodName(const _tchar* pObjTag);
	FOOD_NAME			m_pReceiveName;

private:
	CRcBufferComp* m_pBufferComp = nullptr;
	CTextureComponent* m_pTextureComp = nullptr;
	CTransformComponent* m_pTransformComp = nullptr;
	CCalculatorComponent* m_pCalculatorComp = nullptr;
};

