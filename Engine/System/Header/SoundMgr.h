#pragma once

#include "Base.h"
#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CSoundMgr : public CBase
{
	DERIVED_CLASS_SINGLETON(CBase, CSoundMgr)

private:
	CSoundMgr();
	~CSoundMgr();

private:
	virtual void Free() override;

public:
	HRESULT Ready_Sound();

public:
	// 사운드 재생
	void Play_Sound(TCHAR* pSoundKey, CHANNELID eID, float fVolume);
	// 브금 재생
	void Play_BGM(TCHAR* pSoundKey, float fVolume);
	// 사운드 정지
	void Stop_Sound(CHANNELID eID);
	// 모든 사운드 정지
	void StopAll();
	// 채널의 볼륨 설정
	void SetChannelVolume(CHANNELID eID, float fVolume);

private:
	// 내부적으로 로드할 사운드 파일 폴더를 지정해 로드하는 함수
	void LoadSoundFile(const char* pPath);

	// 사운드 리소스 정보를 갖는 객체 
	_unmap<const _tchar*, FMOD_SOUND*> m_mapSound;

	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD_CHANNEL*		m_pChannelArr[MAXCHANNEL];

	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD_SYSTEM*		m_pSystem = nullptr;

	FMOD_CHANNELGROUP*	m_pChannelGroup[MAX_CHANNEL_GROUP];
	FMOD_CHANNELGROUP*	m_pMasterChanelGroup;
};

END