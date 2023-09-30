#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
	enum WINMODE { MODE_FULL, MODE_WIN };

	// Dynamic 컴포넌트 경우 매 프레임마다 갱신해야하는 컴포넌트 집단
	enum COMPONENTID { ID_DYNAMIC, ID_STATIC, ID_END };

	enum INFO {	INFO_RIGHT, INFO_UP, INFO_LOOK, INFO_POS, INFO_END };
	enum ROTATION { ROT_X, ROT_Y, ROT_Z, ROT_END };

	enum TEXTUREID { TEX_NORMAL, TEX_CUBE, TEX_VOLUME, TEX_END };

	enum RENDERID { RENDER_PRIORITY, RENDER_ALPHATEST, RENDER_NONALPHA, RENDER_ALPHA, RENDER_UI, RENDER_END };

	enum CHANNELID { SOUND_EFFECT, SOUND_BGM, SYSTEM_EFFECT, SOUND_PLAYER, SOUND_ENEMY, SOUND_VFX, MAXCHANNEL };

	enum CHANNEL_GROUP_ID { BGM_GROUP, SND1_GROUP, SND2_GROUP, MAX_CHANNEL_GROUP };

	enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_MB, DIM_MWU, DIM_MWD, DIM_END };

	enum MOUSEMOVESTATE {	DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };

}
#endif // Engine_Enum_h__
