#pragma once

#include "GameObject.h"
#include "ImguiAnimationTool.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;

END

//// 키프레임 구조체
//struct Keyframe
//{
//	char name[64];			// 키프레임 이름 (표시용)
//
//	bool isEaseIn;			// Ease In 설정 (True 또는 False)
//	bool isEaseOut;			// Ease Out 설정 (True 또는 False)
//
//	float time;				// 키프레임의 시간 (0.0f ~ MaxTime 범위)
//	float value;			// 애니메이션 값 (크기, 회전, 이동 등)
//	float color[3];			// 키프레임 색상 (R, G, B)
//
//	OBJ_TYPE m_eObjectType; // 타입을 부여할 그릇 (ex : 한손, 양손)
//	OBJ_NAME m_eObjectName; // 이름을 부여할 그릇 (ex : 권총, 쇠파이프)
//
//	int type;				// 애니메이션 타입 (0: 크기, 1: 회전, 2: 이동)
//
//	_vec3	vScale = { 0.f, 0.f, 0.f };			// 크기를 담을 그릇
//	_vec3	vRot = { 0.f, 0.f, 0.f };			// 회전을 담을 그릇
//	_vec3	vPos = { 0.f, 0.f, 0.f };			// 위치를 담을 그릇
//};

class CBackGround : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CBackGround)

private:
	explicit CBackGround(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBackGround(const CBackGround& rhs);
	virtual ~CBackGround();

public:
	static CBackGround* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();

private:
	HRESULT				Add_Component();

private:
	// 애니메이션 불러오기
	void LoadAnimationFromFile(const char* fileName);

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

public:
	GETSET_EX2(CTransformComponent*,	m_pTransformComp,	TransformComponent,	GET, SET)
	GETSET_EX2(CRcBufferComp*,			m_pBufferComp,		BufferComponent,	GET, SET)
	GETSET_EX2(CTextureComponent*,		m_pTextureComp,		TextureComponent,	GET, SET)

private:
	CTransformComponent*			m_pTransformComp = nullptr;
	CRcBufferComp*					m_pBufferComp = nullptr;
	CTextureComponent*				m_pTextureComp = nullptr;
	CTextureComponent*				m_pBackTextureComp = nullptr;

	_matrix		m_ViewMatrix, m_ProjMatrix;

private: //변수
	// dt 값
	_float fFrameTimeDelta, fCurFrameTimeDelta;

	// 크기
	_float fSizeX_Delta, fSizeY_Delta;

	// 회전
	_float fRotX_Delta, fRotY_Delta, fRotZ_Delta;

	// 이동
	_float fPosX_Delta, fPosY_Delta;

	// 애니메이션 툴
	CImguiAnimationTool* m_pAnimationTool;

	// 애니메이션 값
	std::vector<Keyframe>*  m_vecAnimationInfo;

	//_bool bFrameOn = false;
};

