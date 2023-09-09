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
	// ���� ���
	void Play_Sound(TCHAR* pSoundKey, CHANNELID eID, float fVolume);
	// ��� ���
	void Play_BGM(TCHAR* pSoundKey, float fVolume);
	// ���� ����
	void Stop_Sound(CHANNELID eID);
	// ��� ���� ����
	void StopAll();
	// ä���� ���� ����
	void SetChannelVolume(CHANNELID eID, float fVolume);

private:
	// ���������� �ε��� ���� ���� ������ ������ �ε��ϴ� �Լ�
	void LoadSoundFile(const char* pPath);

	// ���� ���ҽ� ������ ���� ��ü 
	_unmap<const _tchar*, FMOD_SOUND*> m_mapSound;

	// FMOD_CHANNEL : ����ϰ� �ִ� ���带 ������ ��ü 
	FMOD_CHANNEL*		m_pChannelArr[MAXCHANNEL];

	// ���� ,ä�� ��ü �� ��ġ�� �����ϴ� ��ü 
	FMOD_SYSTEM*		m_pSystem = nullptr;

	FMOD_CHANNELGROUP*	m_pChannelGroup[MAX_CHANNEL_GROUP];
	FMOD_CHANNELGROUP*	m_pMasterChanelGroup;
};

END