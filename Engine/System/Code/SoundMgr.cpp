#include "SoundMgr.h"



IMPLEMENT_SINGLETON(CSoundMgr)

CSoundMgr::CSoundMgr()
{
}


CSoundMgr::~CSoundMgr()
{
	Free();
}

void CSoundMgr::Free()
{
	for (auto& Mypair : m_mapSound)
	{
		delete[] Mypair.first;
		FMOD_Sound_Release(Mypair.second);
	}
	m_mapSound.clear();

	FMOD_System_Release(m_pSystem);
	FMOD_System_Close(m_pSystem);
}

HRESULT CSoundMgr::Ready_Sound()
{
	FMOD_RESULT	result;

	// 사운드를 담당하는 대표객체를 생성하는 함수
	result = FMOD_System_Create(&m_pSystem, FMOD_VERSION);

	result = FMOD_System_SetOutput(m_pSystem, FMOD_OUTPUTTYPE_ALSA);

	// 1. 시스템 포인터, 2. 사용할 가상채널 수 , 초기화 방식) 
	result = FMOD_System_Init(m_pSystem, 32, FMOD_INIT_NORMAL, NULL);

	result = FMOD_System_CreateChannelGroup(m_pSystem, "BGM_Group", &m_pChannelGroup[BGM_GROUP]);
	result = FMOD_System_CreateChannelGroup(m_pSystem, "SND1_Group", &m_pChannelGroup[SND1_GROUP]);
	result = FMOD_System_CreateChannelGroup(m_pSystem, "SND2_Group", &m_pChannelGroup[SND2_GROUP]);

	result = FMOD_System_GetMasterChannelGroup(m_pSystem, &m_pMasterChanelGroup);

	// 실제 사운드 폴더 찾아 로드
	LoadSoundFile("./Resource/Sound/bgm/");
	LoadSoundFile("./Resource/Sound/sfx/");

	return S_OK;
}

void CSoundMgr::Play_Sound(TCHAR* pSoundKey, CHANNELID eID, float fVolume)
{
	_unmap<const _tchar*, FMOD_SOUND*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool {
		return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == m_mapSound.end())
		return;

	FMOD_BOOL bPlay = FALSE;

	if (FMOD_Channel_IsPlaying(m_pChannelArr[eID], &bPlay))
	{
		FMOD_System_PlaySound(m_pSystem, iter->second, m_pChannelGroup[SND1_GROUP], FALSE, &m_pChannelArr[eID]);
	}
	else
	{
		FMOD_Channel_Stop(m_pChannelArr[eID]);
		FMOD_System_PlaySound(m_pSystem, iter->second, m_pChannelGroup[SND1_GROUP], FALSE, &m_pChannelArr[eID]);
	}

	FMOD_Channel_SetVolume(m_pChannelArr[eID], 0.1f);//fVolume);

	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::Play_BGM(TCHAR* pSoundKey, float fVolume)
{
	_unmap<const _tchar*, FMOD_SOUND*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool {
		return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == m_mapSound.end())
		return;

	CSoundMgr::GetInstance()->Stop_Sound(SOUND_BGM);
	FMOD_System_PlaySound(m_pSystem, iter->second, m_pChannelGroup[BGM_GROUP], FALSE, &m_pChannelArr[SOUND_BGM]);
	FMOD_Channel_SetMode(m_pChannelArr[SOUND_BGM], FMOD_LOOP_NORMAL);
	FMOD_Channel_SetVolume(m_pChannelArr[SOUND_BGM], 0.1f);//fVolume);
	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::Stop_Sound(CHANNELID eID)
{
	FMOD_Channel_Stop(m_pChannelArr[eID]);
}

void CSoundMgr::StopAll()
{
	for (int i = 0; i < MAXCHANNEL; ++i)
		FMOD_Channel_Stop(m_pChannelArr[i]);
}

void CSoundMgr::SetChannelVolume(CHANNELID eID, float fVolume)
{
	FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);

	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::LoadSoundFile(const char* pPath)
{
	char sText[128] = "";
	strcpy_s(sText, pPath);
	strcat_s(sText, "*.*");

	// _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
	_finddata_t fd;

	// _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
	auto handle = _findfirst(sText, &fd);

	if (handle == -1)
		return;

	int iResult = 0;

	char szCurPath[128] = "";
	char szFullPath[128] = "";

	strcpy_s(szCurPath, pPath);

	// 이게 뭐게 ㅋ
	using sound_tuple = tuple<FMOD_SOUND*, FMOD_RESULT, string, string>;
	enum ETMP_SOUND : int {
		TMP_SOUND,
		TMP_RESULT,
		TMP_PATH,
		TMP_FILE_NAME
	};
	vector<future<FMOD_RESULT>> vecAsync;
	vector<sound_tuple> vecSoundData;
	
	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath);

		// "../Sound/Success.wav"
		strcat_s(szFullPath, fd.name);
		sound_tuple tpSound;

		get<TMP_PATH>(tpSound) = string(szFullPath);
		get<TMP_FILE_NAME>(tpSound) = string(fd.name);
		get<TMP_RESULT>(tpSound) = FMOD_OK;
		get<TMP_SOUND>(tpSound) = nullptr;

		vecSoundData.push_back(tpSound);

		//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
		iResult = _findnext(handle, &fd);
	}

	for (size_t i = 0; i < vecSoundData.size(); i++)
	{
		vecAsync.push_back(async(launch::async, &CSoundMgr::LoadSoundFile_Async, this
									, get<TMP_PATH>(vecSoundData[i]).c_str(), get<TMP_FILE_NAME>(vecSoundData[i]).c_str()
									, ref(get<TMP_RESULT>(vecSoundData[i])), &get<TMP_SOUND>(vecSoundData[i])));
	}

	for (size_t i = 0; i < vecSoundData.size(); i++)
	{
		vecAsync[i].get();

		if (get<TMP_RESULT>(vecSoundData[i]) == FMOD_OK)
		{
			int iLength = (int)strlen(get<TMP_FILE_NAME>(vecSoundData[i]).c_str()) + 1;

			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);

			// 아스키 코드 문자열을 유니코드 문자열로 변환시켜주는 함수
			MultiByteToWideChar(CP_ACP, 0, get<TMP_FILE_NAME>(vecSoundData[i]).c_str(), iLength, pSoundKey, iLength);

			m_mapSound.emplace(pSoundKey, get<TMP_SOUND>(vecSoundData[i]));
		}
	}

	FMOD_System_Update(m_pSystem);

	_findclose(handle);
}

FMOD_RESULT CSoundMgr::LoadSoundFile_Async(const char* pPath, const char* pFileName, FMOD_RESULT& hResult, FMOD_SOUND** pSound)
{
	hResult =  FMOD_System_CreateSound(m_pSystem, pPath, FMOD_DEFAULT, NULL, pSound);

	return hResult;
}

