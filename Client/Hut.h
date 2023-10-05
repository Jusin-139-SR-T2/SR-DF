#pragma once
#include "AceObjectFactory.h"
#include "GameObject.h"

BEGIN(Engine)

class CRcBufferComp;
class CCubeBufferComp;
class CTextureComponent;
class CTransformComponent;
class CCalculatorComponent;

END

class CHut : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CHut)

		// 빌딩 dds 만들때 positiveX 에 문이 위치한것 기준으로 만들기 
		enum class BuildDirection {EAST, SOUTHEAST, SOUTH, SOUTHWEST, WEST, NORTHWEST, NORTH, NORTHEAST, DIR_END};
private:
	explicit CHut(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CHut(const CHut& rhs);
	virtual ~CHut();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

	static CHut* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	CRcBufferComp* m_pBufferComp = nullptr;
	CCubeBufferComp* m_pCubeBufferComp = nullptr;
	CTextureComponent* m_pTextureComp = nullptr;	
	CTransformComponent* m_pTransformComp = nullptr;
	CCalculatorComponent* m_pCalculatorComp = nullptr;

public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CCubeBufferComp*, m_pCubeBufferComp, CubeBufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CCalculatorComponent*, m_pCalculatorComp, CalculatorComponent, GET, SET)

private:
	void				Height_On_Terrain();
	virtual void		Free();
	
	BuildDirection		m_eDir;
	_float m_fSize;
	HRESULT				Add_Component();
	HRESULT				Set_BuildingSize(_float	_Size, BuildDirection _eDir);
};