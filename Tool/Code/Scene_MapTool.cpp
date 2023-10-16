#include "Scene_MapTool.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "Terrain.h"
#include "Loading.h"

#include <mutex>

CScene_MapTool::CScene_MapTool(LPDIRECT3DDEVICE9 pGraphicDev)
    : Engine::CScene(pGraphicDev)
{
}

CScene_MapTool::~CScene_MapTool()
{

}

CScene_MapTool* CScene_MapTool::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_Scene()))
    {
        Safe_Release(pInstance);
        MSG_BOX("MapTool Scene Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CScene_MapTool::Free()
{
    SUPER::Free();
}

HRESULT CScene_MapTool::Ready_Scene()
{
    //g_bLockEsc = true;
    //m_bInitFrame = true;

	Ready_Texture();
    Ready_Proto();
    Ready_Layer_Completed();

    return S_OK;
}

_int CScene_MapTool::Update_Scene(const _float& fTimeDelta)
{
    // m_pLoading은 m_bInitFrame에 종속적입니다.
    //if (m_fSkipStartFrame.Update(fTimeDelta))
    //{
    //    if (m_bInitFrame)
    //    {
    //        // 로딩 쓰레드 생성하기
    //        //m_pLoading = CLoading::Create(m_pGraphicDev, CLoading::LOADING_STAGE);
    //        if (m_pLoading == nullptr)
    //            m_hrLoading = E_FAIL;
    //        else
    //        {
    //            m_hrLoading = S_OK;
    //            m_bInitFrame = false;
    //        }

    //        //사운드 로드하기
    //        //FAILED_CHECK_RETURN(Engine::Ready_SoundDev(), E_FAIL);
    //        //Engine::Play_BGM(L"FallenAces", L"Ambience_OldTimeyMusic6.mp3", 0.75f);
    //    }
    //}


    _int	iExit = SUPER::Update_Scene(fTimeDelta);

    return iExit;
}

void CScene_MapTool::LateUpdate_Scene()
{
    SUPER::LateUpdate_Scene();

    /*if (!m_bInitFrame && m_pLoading->Get_Finish())
    {
        g_bLockEsc = false;

           
    }*/
}

void CScene_MapTool::Render_Scene()
{
    // 디버그

}

HRESULT CScene_MapTool::Ready_Texture()
{
#pragma region Terrain, UI
	Load_Texture(L"../Client/Resource/Texture/Tile/Tile/22.jpg", TEX_NORMAL, L"Tile", L"22");
	Load_Texture(L"../Client/Resource/Texture/SkyBox/Test4.dds", TEX_CUBE, L"TestCube", L"4");
	//Load_Texture(L"../Client/Resource/Texture/SkyBox/Skybox.dds", TEX_CUBE, L"TestCube", L"4");
	Load_Texture(L"../Client/Resource/Texture/UI/PlayerHudBox_Main.png", TEX_NORMAL, L"UI", L"HudBox_Main");
#pragma endregion

#pragma region 싱글 텍스처
	// 왼손 달리기
	Load_Texture(L"../Client/Resource/Texture/Player/RunHand/LeftHand_Run (%d).png", TEX_NORMAL, L"Player", L"Left_RunHand", _range<_uint>(0U, 0U));
	// 오른손 달리기
	Load_Texture(L"../Client/Resource/Texture/Player/RunHand/RightHand_Run (%d).png", TEX_NORMAL, L"Player", L"Right_RunHand", _range<_uint>(0U, 0U));
	// 오픈 핸드 [오른손이 무언가를 할 때 아무것도 아닌상태] (왼손)
	Load_Texture(L"../Client/Resource/Texture/Player/OpenHand/OpenHand (%d).png", TEX_NORMAL, L"Player", L"OpenHand", _range<_uint>(0U, 0U));
#pragma endregion

#pragma region 멀티 텍스처
	// 왼손
	Load_Texture(L"../Client/Resource/Texture/Player/LeftHand/LeftHand (%d).png", TEX_NORMAL, L"Player", L"Left_Hand", _range<_uint>(0U, 1U));
	// 오른손
	Load_Texture(L"../Client/Resource/Texture/Player/RightHand/RightHand (%d).png", TEX_NORMAL, L"Player", L"Right_Hand", _range<_uint>(0U, 1U));
	// 주먹 차징 (오른손)
	Load_Texture(L"../Client/Resource/Texture/Player/RightHand_Charging/RightHand_Charging (%d).png", TEX_NORMAL, L"Player", L"RightHand_Charging", _range<_uint>(0U, 2U));
	// 버리기 (오른손)
	Load_Texture(L"../Client/Resource/Texture/Player/ThrowHand/UnderThrowRightHand (%d).png", TEX_NORMAL, L"Player", L"UnderThrow_RightHand", _range<_uint>(0U, 0U));
	// 쇠파이프
	Load_Texture(L"../Client/Resource/Texture/Player/Steel_Pipe/Steel_Pipe (%d).png", TEX_NORMAL, L"Player", L"Steel_Pipe", _range<_uint>(0U, 3U));
	// 쇠파이프 차징
	Load_Texture(L"../Client/Resource/Texture/Player/Steel_Pipe_Charging/Steel_Pipe_Charging (%d).png", TEX_NORMAL, L"Player", L"Steel_Pipe_Charging", _range<_uint>(0U, 3U));
	// 프라이팬
	Load_Texture(L"../Client/Resource/Texture/Player/FryingPan/FryingPan (%d).png", TEX_NORMAL, L"Player", L"FryingPan", _range<_uint>(0U, 3U));
	// 프라이팬 차징
	Load_Texture(L"../Client/Resource/Texture/Player/FryingPan_Charging/FryingPan_Charging (%d).png", TEX_NORMAL, L"Player", L"FryingPan_Charging", _range<_uint>(0U, 3U));
	// 라이터
	Load_Texture(L"../Client/Resource/Texture/Player/Zippo/Zippo (%d).png", TEX_NORMAL, L"Player", L"Righter", _range<_uint>(0U, 5U));
	// 플레이어 총
	Load_Texture(L"../Client/Resource/Texture/Player/Gun/Gun (%d).png", TEX_NORMAL, L"Player", L"Gun", _range<_uint>(0U, 3U));
	// 플레이어 총 회전
	Load_Texture(L"../Client/Resource/Texture/Player/GunSpin/Spin (%d).png", TEX_NORMAL, L"Player", L"Gun_Spin", _range<_uint>(0U, 3U));
	// 플레이어 톰슨 기관총
	Load_Texture(L"../Client/Resource/Texture/Player/Thompson/Thompson (%d).png", TEX_NORMAL, L"Player", L"Thompson", _range<_uint>(0U, 3U));
	// 맥주병
	Load_Texture(L"../Client/Resource/Texture/Player/BeerBottle/BeerBottle (%d).png", TEX_NORMAL, L"Player", L"BeerBottle", _range<_uint>(0U, 4U));
	// 발차기
	Load_Texture(L"../Client/Resource/Texture/Player/PlayerKick/PlayerKick (%d).png", TEX_NORMAL, L"Player", L"Kick", _range<_uint>(0U, 2U));
#pragma endregion

#pragma region Brown Monster
	// Monster Texture - Brown - Single : 3EA
	Load_Texture(L"../Client/Resource/Texture/Monster/Brown/Single/CrotchHit.png", TEX_NORMAL, L"Brown_Single", L"CrotchHit");
	Load_Texture(L"../Client/Resource/Texture/Monster/Brown/Stand/StandSouth.png", TEX_NORMAL, L"Brown_Single", L"Stand_South");
	Load_Texture(L"../Client/Resource/Texture/Monster/Brown/Single/Suspicious.png", TEX_NORMAL, L"Brown_Single", L"Suspicious");

	// Monster Texture - Brown - Multi : 18EA
	Load_Texture(L"../Client/Resource/Texture/Monster/Brown/BasicAttack/BasicAttack_%d.png", TEX_NORMAL, L"Brown_Multi", L"NormalAttack", _range<_uint>(0U, 4U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Brown/HeavyAttack/HeavyAttack_%d.png", TEX_NORMAL, L"Brown_Multi", L"HeavyAttack", _range<_uint>(0U, 6U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Brown/InchForward/InchForward_%d.png", TEX_NORMAL, L"Brown_Multi", L"InchForward", _range<_uint>(0U, 5U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Brown/Walk/North/WalkNorth_%d.png", TEX_NORMAL, L"Brown_Multi", L"Walk_North", _range<_uint>(0U, 31U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Brown/Chopped/KarateChopped_%d.png", TEX_NORMAL, L"Brown_Multi", L"Chopped", _range<_uint>(0U, 17U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Brown/FacePunch/FacePunch_%d.png", TEX_NORMAL, L"Brown_Multi", L"FacePunch", _range<_uint>(0U, 7U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Brown/Run/South/RunSouth_%d.png", TEX_NORMAL, L"Brown_Multi", L"RunSouth", _range<_uint>(0U, 20U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Brown/Headless/Headless_%d.png", TEX_NORMAL, L"Brown_Multi", L"Headless", _range<_uint>(0U, 15U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Brown/Walk/South/Walk_%d.png", TEX_NORMAL, L"Brown_Multi", L"Walk_South", _range<_uint>(0U, 23U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Brown/Strafing/Strafing_%d.png", TEX_NORMAL, L"Brown_Multi", L"Strafing", _range<_uint>(0U, 5U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Brown/Falling/Falling_%d.png", TEX_NORMAL, L"Brown_Multi", L"Falling", _range<_uint>(0U, 17U)); // FALLING + GETUP 
	Load_Texture(L"../Client/Resource/Texture/Monster/Brown/Rest/IdleReady_%d.png", TEX_NORMAL, L"Brown_Multi", L"Rest", _range<_uint>(0U, 6U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Brown/Dazed/Dazed_%d.png", TEX_NORMAL, L"Brown_Multi", L"Dazed", _range<_uint>(0U, 13U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Brown/Death/Death_%d.png", TEX_NORMAL, L"Brown_Multi", L"Death", _range<_uint>(0U, 4U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Brown/Getup/Getup_%d.png", TEX_NORMAL, L"Brown_Multi", L"GetUp", _range<_uint>(0U, 5U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Brown/Taunt/Taunt_%d.png", TEX_NORMAL, L"Brown_Multi", L"Taunt", _range<_uint>(0U, 5U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Brown/Hit/Hit_%d.png", TEX_NORMAL, L"Brown_Multi", L"Hit", _range<_uint>(0U, 5U));

#pragma endregion

#pragma region Gray Monster
	// Monster Texture - Gray - Single : 5EA (2 Pending Approval) 
	Load_Texture(L"../Client/Resource/Texture/Monster/Gray/Single/CrotchHit.png", TEX_NORMAL, L"Gray_Single", L"CrotchHit");
	Load_Texture(L"../Client/Resource/Texture/Monster/Gray/Single/Idle.png", TEX_NORMAL, L"Gray_Single", L"Idle"); // SOUTH 
	Load_Texture(L"../Client/Resource/Texture/Monster/Gray/Single/Suspicious.png", TEX_NORMAL, L"Gray_Single", L"Suspicious"); // SOUTH 


	// Monster Texture - Gray - Multi : 21EA
	Load_Texture(L"../Client/Resource/Texture/Monster/Gray/HeavyAttack/HeavyAttack_%d.png", TEX_NORMAL, L"Gray_Multi", L"HeavyAttack", _range<_uint>(0U, 13U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Gray/UpRightRun/UpRightRun_%d.png", TEX_NORMAL, L"Gray_Multi", L"UpRightRun", _range<_uint>(0U, 4U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Gray/WalkNorth/WalkNorth_%d.png", TEX_NORMAL, L"Gray_Multi", L"WalkNorth", _range<_uint>(0U, 22U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Gray/FacePunch/FacePunch_%d.png", TEX_NORMAL, L"Gray_Multi", L"FacePunch", _range<_uint>(0U, 7U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Gray/Frighten/Frighten_%d.png", TEX_NORMAL, L"Gray_Multi", L"Frighten", _range<_uint>(0U, 18U)); // 없는것 
	Load_Texture(L"../Client/Resource/Texture/Monster/Gray/Headless/Headless_%d.png", TEX_NORMAL, L"Gray_Multi", L"Headless", _range<_uint>(0U, 21U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Gray/HeadShot/HeadShot_%d.png", TEX_NORMAL, L"Gray_Multi", L"HeadShot", _range<_uint>(0U, 19U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Gray/SideWalk/SideWalk_%d.png", TEX_NORMAL, L"Gray_Multi", L"SideWalk", _range<_uint>(0U, 6U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Gray/Chopped/Chopped_%d.png", TEX_NORMAL, L"Gray_Multi", L"Chopped", _range<_uint>(0U, 17U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Gray/Falling/Falling_%d.png", TEX_NORMAL, L"Gray_Multi", L"Falling", _range<_uint>(0U, 10U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Gray/KeepEye/KeepEye_%d.png", TEX_NORMAL, L"Gray_Multi", L"KeepEye", _range<_uint>(0U, 26U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Gray/YouDie/YouDie_%d.png", TEX_NORMAL, L"Gray_Multi", L"YouDie", _range<_uint>(0U, 11U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Gray/Attack/Attack_%d.png", TEX_NORMAL, L"Gray_Multi", L"Attack", _range<_uint>(0U, 8U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Gray/Coward/Coward_%d.png", TEX_NORMAL, L"Gray_Multi", L"Coward", _range<_uint>(0U, 6U)); // Hit임 
	Load_Texture(L"../Client/Resource/Texture/Monster/Gray/WalkSouth/Walk_%d.png", TEX_NORMAL, L"Gray_Multi", L"Walk", _range<_uint>(0U, 18U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Gray/Taunt/Taunt_%d.png", TEX_NORMAL, L"Gray_Multi", L"Taunt", _range<_uint>(0U, 10U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Gray/Dazed/Dazed_%d.png", TEX_NORMAL, L"Gray_Multi", L"Dazed", _range<_uint>(0U, 13U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Gray/Throw/Throw_%d.png", TEX_NORMAL, L"Gray_Multi", L"Throw", _range<_uint>(0U, 9U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Gray/Block/Block_%d.png", TEX_NORMAL, L"Gray_Multi", L"Block", _range<_uint>(0U, 3U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Gray/Death/Death_%d.png", TEX_NORMAL, L"Gray_Multi", L"Death", _range<_uint>(0U, 9U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Gray/Rest/Rest_%d.png", TEX_NORMAL, L"Gray_Multi", L"Rest", _range<_uint>(0U, 4U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Gray/Run/Run_%d.png", TEX_NORMAL, L"Gray_Multi", L"Run", _range<_uint>(0U, 21U));



	//추가 IDLE 은 IDLE 폴더가 따로있음 

#pragma endregion

#pragma region Boss Monster
	// Monster Texture - Boss - Single
	Load_Texture(L"../Client/Resource/Texture/Monster/Boss/Stand/Idle_NorthWest.png", TEX_NORMAL, L"Boss_Single", L"Idle_NorthWest");
	Load_Texture(L"../Client/Resource/Texture/Monster/Boss/Stand/Idle_SouthWest.png", TEX_NORMAL, L"Boss_Single", L"Idle_SouthWest");
	Load_Texture(L"../Client/Resource/Texture/Monster/Boss/Stand/Idle_North.png", TEX_NORMAL, L"Boss_Single", L"Idle_North");
	Load_Texture(L"../Client/Resource/Texture/Monster/Boss/Stand/Idle_South.png", TEX_NORMAL, L"Boss_Single", L"Idle_South");
	Load_Texture(L"../Client/Resource/Texture/Monster/Boss/Stand/Idle_West.png", TEX_NORMAL, L"Boss_Single", L"Idle_West");

	Load_Texture(L"../Client/Resource/Texture/Monster/Boss/Single/Pre_Attack.png", TEX_NORMAL, L"Boss_Single", L"Pre_Attack");
	Load_Texture(L"../Client/Resource/Texture/Monster/Boss/Single/SideReady.png", TEX_NORMAL, L"Boss_Single", L"SideReady");
	Load_Texture(L"../Client/Resource/Texture/Monster/Boss/Single/CrotchHit.png", TEX_NORMAL, L"Boss_Single", L"CrotchHit");
	Load_Texture(L"../Client/Resource/Texture/Monster/Boss/Single/BackIdle.png", TEX_NORMAL, L"Boss_Single", L"BackIdle");
	Load_Texture(L"../Client/Resource/Texture/Monster/Boss/Single/Hit_A.png", TEX_NORMAL, L"Boss_Single", L"Hit_A");
	Load_Texture(L"../Client/Resource/Texture/Monster/Boss/Single/Hit_B.png", TEX_NORMAL, L"Boss_Single", L"Hit_B");
	Load_Texture(L"../Client/Resource/Texture/Monster/Boss/Single/Chase.png", TEX_NORMAL, L"Boss_Single", L"Chase");

	// Monster Texture - Boss - Multi
	Load_Texture(L"../Client/Resource/Texture/Monster/Boss/NorthWestWalk/NorthWestWalk_%d.png", TEX_NORMAL, L"Boss_Multi", L"NorthWestWalk", _range<_uint>(0U, 19U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Boss/CloseAttack/CloseAttack_%d.png", TEX_NORMAL, L"Boss_Multi", L"CloseAttack", _range<_uint>(0U, 5U));
	//Load_Texture(L"../Client/Resource/Texture/Monster/Boss/NorthWalk/NorthWalk_%d.png", TEX_NORMAL, L"Boss_Multi", L"NorthWalk", _range<_uint>(0U, 26U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Boss/Reloading/Reloading_%d.png", TEX_NORMAL, L"Boss_Multi", L"Reloading", _range<_uint>(0U, 17U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Boss/IdleReady/IdleReady_%d.png", TEX_NORMAL, L"Boss_Multi", L"IdleReady", _range<_uint>(0U, 7U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Boss/FacePunch/FacePunch_%d.png", TEX_NORMAL, L"Boss_Multi", L"FacePunch", _range<_uint>(0U, 7U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Boss/Shooting/Shooting_%d.png", TEX_NORMAL, L"Boss_Multi", L"Shooting", _range<_uint>(0U, 8U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Boss/Falling/Falling_%d.png", TEX_NORMAL, L"Boss_Multi", L"Falling", _range<_uint>(0U, 10U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Boss/Evasion/Evasion_%d.png", TEX_NORMAL, L"Boss_Multi", L"Evasion", _range<_uint>(0U, 3U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Boss/Dazed/Dazed_%d.png", TEX_NORMAL, L"Boss_Multi", L"Dazed", _range<_uint>(0U, 13U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Boss/Button/SetOn_%d.png", TEX_NORMAL, L"Boss_Multi", L"SetOn", _range<_uint>(0U, 3U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Boss/Death/Death_%d.png", TEX_NORMAL, L"Boss_Multi", L"Death", _range<_uint>(0U, 9U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Boss/Shout/Shout_%d.png", TEX_NORMAL, L"Boss_Multi", L"Shout", _range<_uint>(0U, 3U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Boss/Running/Run_%d.png", TEX_NORMAL, L"Boss_Multi", L"Run", _range<_uint>(0U, 9U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Boss/Roll/Roll_%d.png", TEX_NORMAL, L"Boss_Multi", L"Roll", _range<_uint>(0U, 19U));
	Load_Texture(L"../Client/Resource/Texture/Monster/Boss/Walk/Walk_%d.png", TEX_NORMAL, L"Boss_Multi", L"Walk", _range<_uint>(0U, 28U));

#pragma endregion

#pragma region Projectile
	// 디버그용
	Load_Texture(L"../Client/Resource/Texture/Object/PoolBalls_0.png", TEX_NORMAL, L"Debug", L"Sphere"); // 몬스터 공격 디버그용

	Load_Texture(L"../Client/Resource/Texture/Monster/Projectile/ThrowPipe_%d.png", TEX_NORMAL, L"Projectile", L"ThrowPipe", _range<_uint>(0U, 11U)); // Bomb
	Load_Texture(L"../Client/Resource/Texture/idk/Circle.png", TEX_NORMAL, L"Debug", L"Sphere"); // Bomb

#pragma endregion

#pragma region Object 
	// Object - Food
	Load_Texture(L"../Client/Resource/Texture/Item/Food/BananaPeel.png", TEX_NORMAL, L"Food", L"BananaPeel");
	Load_Texture(L"../Client/Resource/Texture/Item/Food/EatenApple.png", TEX_NORMAL, L"Food", L"EatenApple");
	Load_Texture(L"../Client/Resource/Texture/Item/Food/Medkit.png", TEX_NORMAL, L"Food", L"Medkit");
	Load_Texture(L"../Client/Resource/Texture/Item/Food/Banana.png", TEX_NORMAL, L"Food", L"Banana");
	Load_Texture(L"../Client/Resource/Texture/Item/Food/Apple.png", TEX_NORMAL, L"Food", L"Apple");
	Load_Texture(L"../Client/Resource/Texture/Item/Food/Cola.png", TEX_NORMAL, L"Food", L"Cola");

	// Object - Weapon
	Load_Texture(L"../Client/Resource/Texture/Item/Weapon/BrokenFryingPan.png", TEX_NORMAL, L"Weapon", L"BrokenFryingPan");
	Load_Texture(L"../Client/Resource/Texture/Item/Weapon/BrokenTommyGun.png", TEX_NORMAL, L"Weapon", L"BrokenTommyGun");
	Load_Texture(L"../Client/Resource/Texture/Item/Weapon/CrackedBottle.png", TEX_NORMAL, L"Weapon", L"CrackedBottle");
	Load_Texture(L"../Client/Resource/Texture/Item/Weapon/BrokenBottle.png", TEX_NORMAL, L"Weapon", L"BrokenBottle");
	Load_Texture(L"../Client/Resource/Texture/Item/Weapon/BrokenPistol.png", TEX_NORMAL, L"Weapon", L"BrokenPistol");
	Load_Texture(L"../Client/Resource/Texture/Item/Weapon/GasCanister.png", TEX_NORMAL, L"Weapon", L"GasCanister");
	Load_Texture(L"../Client/Resource/Texture/Item/Weapon/BrokenPipe.png", TEX_NORMAL, L"Weapon", L"BrokenPipe");
	Load_Texture(L"../Client/Resource/Texture/Item/Weapon/FryingPan.png", TEX_NORMAL, L"Weapon", L"FryingPan");
	Load_Texture(L"../Client/Resource/Texture/Item/Weapon/TommyGun.png", TEX_NORMAL, L"Weapon", L"TommyGun");
	Load_Texture(L"../Client/Resource/Texture/Item/Weapon/LeadPipe.png", TEX_NORMAL, L"Weapon", L"LeadPipe");
	Load_Texture(L"../Client/Resource/Texture/Item/Weapon/WallPipe.png", TEX_NORMAL, L"Weapon", L"WallPipe");
	Load_Texture(L"../Client/Resource/Texture/Item/Weapon/Pistol.png", TEX_NORMAL, L"Weapon", L"Pistol");
	Load_Texture(L"../Client/Resource/Texture/Item/Weapon/Bottle.png", TEX_NORMAL, L"Weapon", L"Bottle");

	//Object - Throw 
	Load_Texture(L"../Client/Resource/Texture/Item/Throw/ScrunchedPaper_%d.png", TEX_NORMAL, L"Throw", L"ScrunchedPaper", _range<_uint>(0U, 1U));
	Load_Texture(L"../Client/Resource/Texture/Item/Throw/PaperBox_%d.png", TEX_NORMAL, L"Throw", L"PaperBox", _range<_uint>(0U, 1U));
	Load_Texture(L"../Client/Resource/Texture/Item/Throw/Sack_%d.png", TEX_NORMAL, L"Throw", L"Sack", _range<_uint>(0U, 2U));
	Load_Texture(L"../Client/Resource/Texture/Item/Throw/Handbag.png", TEX_NORMAL, L"Throw", L"Handbag");
	Load_Texture(L"../Client/Resource/Texture/Item/Throw/Fedora.png", TEX_NORMAL, L"Throw", L"Fedora");
	Load_Texture(L"../Client/Resource/Texture/Item/Throw/Rope.png", TEX_NORMAL, L"Throw", L"Rope");

	Load_Texture(L"../Client/Resource/Texture/Item/Throw/Board_%d.png", TEX_NORMAL, L"Throw", L"Board", _range<_uint>(0U, 2U));
	Load_Texture(L"../Client/Resource/Texture/Item/Throw/Chair_%d.png", TEX_NORMAL, L"Throw", L"Chair", _range<_uint>(0U, 7U));
	Load_Texture(L"../Client/Resource/Texture/Item/Throw/UmbrellaStand.png", TEX_NORMAL, L"Throw", L"UmbrellaStand");
	Load_Texture(L"../Client/Resource/Texture/Item/Throw/WoodenBarrel.png", TEX_NORMAL, L"Throw", L"WoodenBarrel");
	Load_Texture(L"../Client/Resource/Texture/Item/Throw/WoodenStool.png", TEX_NORMAL, L"Throw", L"WoodenStool");
	Load_Texture(L"../Client/Resource/Texture/Item/Throw/CoatHanger.png", TEX_NORMAL, L"Throw", L"CoatHanger");
	Load_Texture(L"../Client/Resource/Texture/Item/Throw/Mannequin.png", TEX_NORMAL, L"Throw", L"Mannequin");
	Load_Texture(L"../Client/Resource/Texture/Item/Throw/CableReel.png", TEX_NORMAL, L"Throw", L"CableReel");
	Load_Texture(L"../Client/Resource/Texture/Item/Throw/CafeTable.png", TEX_NORMAL, L"Throw", L"CafeTable");
	Load_Texture(L"../Client/Resource/Texture/Item/Throw/CafeSign.png", TEX_NORMAL, L"Throw", L"CafeSign");
	Load_Texture(L"../Client/Resource/Texture/Item/Throw/BarStool.png", TEX_NORMAL, L"Throw", L"BarStool");
	Load_Texture(L"../Client/Resource/Texture/Item/Throw/Toolbox.png", TEX_NORMAL, L"Throw", L"Toolbox");
	Load_Texture(L"../Client/Resource/Texture/Item/Throw/Matches.png", TEX_NORMAL, L"Throw", L"Matches");

	Load_Texture(L"../Client/Resource/Texture/Item/Throw/GenericBottles_%d.png", TEX_NORMAL, L"Throw", L"GenericBottles", _range<_uint>(0U, 5U));
	Load_Texture(L"../Client/Resource/Texture/Item/Throw/Perfume_%d.png", TEX_NORMAL, L"Throw", L"Perfume", _range<_uint>(0U, 2U));
	Load_Texture(L"../Client/Resource/Texture/Item/Throw/Lamp_%d.png", TEX_NORMAL, L"Throw", L"Lamp", _range<_uint>(0U, 1U));
	Load_Texture(L"../Client/Resource/Texture/Item/Throw/TablePlantFlower.png", TEX_NORMAL, L"Throw", L"TablePlantFlower");
	Load_Texture(L"../Client/Resource/Texture/Item/Throw/TablePlantLeaf.png", TEX_NORMAL, L"Throw", L"TablePlantLeaf");
	Load_Texture(L"../Client/Resource/Texture/Item/Throw/WhiskeyGlass.png", TEX_NORMAL, L"Throw", L"WhiskeyGlass");
	Load_Texture(L"../Client/Resource/Texture/Item/Throw/MartiniGlass.png", TEX_NORMAL, L"Throw", L"MartiniGlass");
	Load_Texture(L"../Client/Resource/Texture/Item/Throw/BeerGlass.png", TEX_NORMAL, L"Throw", L"BeerGlass");
	Load_Texture(L"../Client/Resource/Texture/Item/Throw/Potplant.png", TEX_NORMAL, L"Throw", L"Potplant");
	Load_Texture(L"../Client/Resource/Texture/Item/Throw/BeerMug.png", TEX_NORMAL, L"Throw", L"BeerMug");

	Load_Texture(L"../Client/Resource/Texture/Item/Throw/trashbin_%d.png", TEX_NORMAL, L"Throw", L"Trashbin", _range<_uint>(0U, 2U));
	Load_Texture(L"../Client/Resource/Texture/Item/Throw/trashcan_%d.png", TEX_NORMAL, L"Throw", L"TrashCan", _range<_uint>(0U, 2U));
	Load_Texture(L"../Client/Resource/Texture/Item/Throw/PaintCan_%d.png", TEX_NORMAL, L"Throw", L"PaintCan", _range<_uint>(0U, 3U));
	Load_Texture(L"../Client/Resource/Texture/Item/Throw/ChampagneBucket.png", TEX_NORMAL, L"Throw", L"ChampagneBucket");
	Load_Texture(L"../Client/Resource/Texture/Item/Throw/TrafficBollard.png", TEX_NORMAL, L"Throw", L"TrafficBollard");
	Load_Texture(L"../Client/Resource/Texture/Item/Throw/TrafficCone.png", TEX_NORMAL, L"Throw", L"TrafficCone");
	Load_Texture(L"../Client/Resource/Texture/Item/Throw/MetalBarrel.png", TEX_NORMAL, L"Throw", L"MetalBarrel");
	Load_Texture(L"../Client/Resource/Texture/Item/Throw/Microphone.png", TEX_NORMAL, L"Throw", L"Microphone");
	Load_Texture(L"../Client/Resource/Texture/Item/Throw/OilBarrel.png", TEX_NORMAL, L"Throw", L"OilBarrel");
	Load_Texture(L"../Client/Resource/Texture/Item/Throw/Bucket.png", TEX_NORMAL, L"Throw", L"Bucket");

	Load_Texture(L"../Client/Resource/Texture/Item/Interaction/LightSwitch_Off.png", TEX_NORMAL, L"Interaction", L"LightSwitch_Off");
	Load_Texture(L"../Client/Resource/Texture/Item/Interaction/BlockSwitch_Off.png", TEX_NORMAL, L"Interaction", L"BlockSwitch_Off");
	Load_Texture(L"../Client/Resource/Texture/Item/Interaction/LightSwitch_On.png", TEX_NORMAL, L"Interaction", L"LightSwitch_On");
	Load_Texture(L"../Client/Resource/Texture/Item/Interaction/BlockSwitch_On.png", TEX_NORMAL, L"Interaction", L"BlockSwitch_On");
	Load_Texture(L"../Client/Resource/Texture/Item/Interaction/OneButton_Off.png", TEX_NORMAL, L"Interaction", L"OneButton_Off");
	Load_Texture(L"../Client/Resource/Texture/Item/Interaction/TwoButton_Off.png", TEX_NORMAL, L"Interaction", L"TwoButton_Off");
	Load_Texture(L"../Client/Resource/Texture/Item/Interaction/TwoButton_On.png", TEX_NORMAL, L"Interaction", L"TwoButton_On");
	Load_Texture(L"../Client/Resource/Texture/Item/Interaction/OneButton_On.png", TEX_NORMAL, L"Interaction", L"OneButton_On");
	Load_Texture(L"../Client/Resource/Texture/Item/Interaction/Padlock_Off.png", TEX_NORMAL, L"Interaction", L"Padlock_Off");
	Load_Texture(L"../Client/Resource/Texture/Item/Interaction/Key_Blue.png", TEX_NORMAL, L"Interaction", L"Building_Key");
	Load_Texture(L"../Client/Resource/Texture/Item/Interaction/Key_Red.png", TEX_NORMAL, L"Interaction", L"Bossroom_Key");
	Load_Texture(L"../Client/Resource/Texture/Item/Interaction/Newspaper.png", TEX_NORMAL, L"Interaction", L"Newspaper");
	Load_Texture(L"../Client/Resource/Texture/Item/Interaction/ComicBook.png", TEX_NORMAL, L"Interaction", L"ComicBook");

#pragma endregion

#pragma region Object - Inventory : 직교투영 해야함 

	Load_Texture(L"../Client/Resource/Texture/Item/Inventory/FryingPan.png", TEX_NORMAL, L"Inventory", L"FryingPan");
	Load_Texture(L"../Client/Resource/Texture/Item/Inventory/Revolver.png", TEX_NORMAL, L"Inventory", L"Revolver");
	Load_Texture(L"../Client/Resource/Texture/Item/Inventory/TommyGun.png", TEX_NORMAL, L"Inventory", L"TommyGun");
	Load_Texture(L"../Client/Resource/Texture/Item/Inventory/Bottle.png", TEX_NORMAL, L"Inventory", L"Bottle");
	Load_Texture(L"../Client/Resource/Texture/Item/Inventory/Banana.png", TEX_NORMAL, L"Inventory", L"Banana");
	Load_Texture(L"../Client/Resource/Texture/Item/Inventory/MedKit.png", TEX_NORMAL, L"Inventory", L"MedKit");
	Load_Texture(L"../Client/Resource/Texture/Item/Inventory/Apple.png", TEX_NORMAL, L"Inventory", L"Apple");
	Load_Texture(L"../Client/Resource/Texture/Item/Inventory/Pipe.png", TEX_NORMAL, L"Inventory", L"Pipe");
	Load_Texture(L"../Client/Resource/Texture/Item/Inventory/Cola.png", TEX_NORMAL, L"Inventory", L"Cola");

#pragma endregion

#pragma region Effect

	//몬스터 관련 이펙트 
	Load_Texture(L"../Client/Resource/Texture/AceEffect/SkillEffect/Stone/FallingStone1.png", TEX_NORMAL, L"Effect", L"Stone1");
	Load_Texture(L"../Client/Resource/Texture/AceEffect/SkillEffect/Stone/FallingStone2.png", TEX_NORMAL, L"Effect", L"Stone2");
	Load_Texture(L"../Client/Resource/Texture/AceEffect/SkillEffect/SlowThunder/SlowThunder_%d.png", TEX_NORMAL, L"Effect", L"SlowThunder", _range<_uint>(0U, 5U));
	Load_Texture(L"../Client/Resource/Texture/AceEffect/SkillEffect/MissTarget/MissTarget_%d.png", TEX_NORMAL, L"Effect", L"MissTarget", _range<_uint>(0U, 14U));
	Load_Texture(L"../Client/Resource/Texture/AceEffect/SkillEffect/EnergyBall/EnergyBall_%d.png", TEX_NORMAL, L"Effect", L"EnergyBall", _range<_uint>(0U, 7U));
	Load_Texture(L"../Client/Resource/Texture/AceEffect/SkillEffect/RedThunder/RedThunder_%d.png", TEX_NORMAL, L"Effect", L"RedThunder", _range<_uint>(0U, 5U));
	Load_Texture(L"../Client/Resource/Texture/AceEffect/SkillEffect/FireEffect/Fire_%d.png", TEX_NORMAL, L"Effect", L"FireEffect", _range<_uint>(0U, 5U));
	Load_Texture(L"../Client/Resource/Texture/AceEffect/SkillEffect/Stone/Stone_%d.png", TEX_NORMAL, L"Effect", L"FallingStone", _range<_uint>(0U, 3U));
	Load_Texture(L"../Client/Resource/Texture/AceEffect/SkillEffect/Barrier/Barrier_%d.png", TEX_NORMAL, L"Effect", L"Barrier", _range<_uint>(0U, 7U));
	Load_Texture(L"../Client/Resource/Texture/AceEffect/SkillEffect/GreenBuff/Buff_%d.png", TEX_NORMAL, L"Effect", L"Buff", _range<_uint>(0U, 7U));
	Load_Texture(L"../Client/Resource/Texture/AceEffect/SkillEffect/Razer/Razer_%d.png", TEX_NORMAL, L"Effect", L"Razer", _range<_uint>(0U, 18U));
	Load_Texture(L"../Client/Resource/Texture/AceEffect/SkillEffect/Holy/Holy_%d.png", TEX_NORMAL, L"Effect", L"Holy", _range<_uint>(0U, 3U));

	Load_Texture(L"../Client/Resource/Texture/AceEffect/Awareness/Awareness_15.png", TEX_NORMAL, L"Effect", L"MaxAware"); // 몬스터 공격 디버그용
	Load_Texture(L"../Client/Resource/Texture/AceEffect/Awareness/Awareness_%d.png", TEX_NORMAL, L"Effect", L"Awareness", _range<_uint>(0U, 15U));

	// 히트판정
	Load_Texture(L"../Client/Resource/Texture/AceEffect/FallingDust/FallingDust_%d.png", TEX_NORMAL, L"Effect", L"FallingDust", _range<_uint>(0U, 17U));
	Load_Texture(L"../Client/Resource/Texture/AceEffect/HitBlood/HitBlood_%d.png", TEX_NORMAL, L"Effect", L"HitBlood", _range<_uint>(0U, 13U));
	Load_Texture(L"../Client/Resource/Texture/AceEffect/Dazed/DazeSwirl_%d.png", TEX_NORMAL, L"Effect", L"DazeSwirl", _range<_uint>(0U, 3U));
	Load_Texture(L"../Client/Resource/Texture/AceEffect/HitDust/HitDust_%d.png", TEX_NORMAL, L"Effect", L"HitDust", _range<_uint>(0U, 16U));
	Load_Texture(L"../Client/Resource/Texture/AceEffect/HitPow/HitPow_%d.png", TEX_NORMAL, L"Effect", L"HitPow", _range<_uint>(0U, 3U));

#pragma endregion 

#pragma region Building - TEX_CUBE 
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Building/A.dds", TEX_CUBE, L"Building", L"A");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Building/B.dds", TEX_CUBE, L"Building", L"B");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Building/C.dds", TEX_CUBE, L"Building", L"C");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Building/D.dds", TEX_CUBE, L"Building", L"D");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Building/E.dds", TEX_CUBE, L"Building", L"E");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Building/F.dds", TEX_CUBE, L"Building", L"F");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Building/G.dds", TEX_CUBE, L"Building", L"G");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Building/H.dds", TEX_CUBE, L"Building", L"H");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Building/I.dds", TEX_CUBE, L"Building", L"I");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Building/J.dds", TEX_CUBE, L"Building", L"J");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Building/K.dds", TEX_CUBE, L"Building", L"K");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Building/L.dds", TEX_CUBE, L"Building", L"L");

#pragma endregion

#pragma region Furniture - TEX_CUBE 
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/WhiteCurtainTable.dds", TEX_CUBE, L"Furniture", L"WhiteCurtainTable");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/CigaretteMachine.dds", TEX_CUBE, L"Furniture", L"CigaretteMachine");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/DarkNavyCushion2.dds", TEX_CUBE, L"Furniture", L"DarkNavyCushion2");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/BrokenVent_Rect.dds", TEX_CUBE, L"Furniture", L"BrokenVent_Rect");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/DarkNavyCushion.dds", TEX_CUBE, L"Furniture", L"DarkNavyCushion");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/DarkRedCushion.dds", TEX_CUBE, L"Furniture", L"DarkRedCushion");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/BrokenVentFan.dds", TEX_CUBE, L"Furniture", L"BrokenVentFan");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/GreenCabinet2.dds", TEX_CUBE, L"Furniture", L"GreenCabinet2");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/WhiteStorage2.dds", TEX_CUBE, L"Furniture", L"WhiteStorage2");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/electricalBox.dds", TEX_CUBE, L"Furniture", L"electricalBox");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/GreenCabinet.dds", TEX_CUBE, L"Furniture", L"GreenCabinet");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/MarbleTable2.dds", TEX_CUBE, L"Furniture", L"MarbleTable2");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/WhiteStorage.dds", TEX_CUBE, L"Furniture", L"WhiteStorage");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/MarbleTable1.dds", TEX_CUBE, L"Furniture", L"MarbleTable1");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/BookShelves2.dds", TEX_CUBE, L"Furniture", L"BookShelves2");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/CockMachine.dds", TEX_CUBE, L"Furniture", L"CockMachine");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/GrayCabinet.dds", TEX_CUBE, L"Furniture", L"GrayCabinet");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/GreenCrate1.dds", TEX_CUBE, L"Furniture", L"GreenCrate1");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/WoodCabinet.dds", TEX_CUBE, L"Furniture", L"WoodCabinet");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/RedCushion2.dds", TEX_CUBE, L"Furniture", L"RedCushion2");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/BlindWindow.dds", TEX_CUBE, L"Furniture", L"BlindWindow");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/BlueCabinet.dds", TEX_CUBE, L"Furniture", L"BlueCabinet");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/BlueMachine.dds", TEX_CUBE, L"Furniture", L"BlueMachine");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/BookShelves.dds", TEX_CUBE, L"Furniture", L"BookShelves");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/BrokenGlass.dds", TEX_CUBE, L"Furniture", L"BrokenGlass");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/PhoneBooth.dds", TEX_CUBE, L"Furniture", L"PhoneBooth");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/BrownTrunk.dds", TEX_CUBE, L"Furniture", L"BrownTrunk");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/RedCurtain.dds", TEX_CUBE, L"Furniture", L"RedCurtain");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/RedCushion.dds", TEX_CUBE, L"Furniture", L"RedCushion");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/Television.dds", TEX_CUBE, L"Furniture", L"Television");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/WhiteStand.dds", TEX_CUBE, L"Furniture", L"WhiteStand");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/WhiteStand.dds", TEX_CUBE, L"Furniture", L"WhiteStand");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/VentIntro.dds", TEX_CUBE, L"Furniture", L"VentIntro");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/Barbottle.dds", TEX_CUBE, L"Furniture", L"BarBottle");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/BlueTrunk.dds", TEX_CUBE, L"Furniture", L"BlueTrunk");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/TableLeg.dds", TEX_CUBE, L"Furniture", L"TableLeg");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/VentFan1.dds", TEX_CUBE, L"Furniture", L"VentFan1");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/VentFan2.dds", TEX_CUBE, L"Furniture", L"VentFan2");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/LightBox.dds", TEX_CUBE, L"Furniture", L"LightBox");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/Radiator.dds", TEX_CUBE, L"Furniture", L"Radiator");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/BarFront.dds", TEX_CUBE, L"Furniture", L"BarFront");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/BarTable.dds", TEX_CUBE, L"Furniture", L"BarTable");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/Cabinet1.dds", TEX_CUBE, L"Furniture", L"Cabinet1");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/Cabinet2.dds", TEX_CUBE, L"Furniture", L"Cabinet2");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/Cabinet3.dds", TEX_CUBE, L"Furniture", L"Cabinet3");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/Column10.dds", TEX_CUBE, L"Furniture", L"Column10");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/Column1.dds", TEX_CUBE, L"Furniture", L"Column1");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/Column2.dds", TEX_CUBE, L"Furniture", L"Column2");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/Column3.dds", TEX_CUBE, L"Furniture", L"Column3");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/Column4.dds", TEX_CUBE, L"Furniture", L"Column4");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/Column5.dds", TEX_CUBE, L"Furniture", L"Column5");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/Column6.dds", TEX_CUBE, L"Furniture", L"Column6");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/Column7.dds", TEX_CUBE, L"Furniture", L"Column7");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/Column8.dds", TEX_CUBE, L"Furniture", L"Column8");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/Column9.dds", TEX_CUBE, L"Furniture", L"Column9");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/Closet.dds", TEX_CUBE, L"Furniture", L"Closet");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/Crate1.dds", TEX_CUBE, L"Furniture", L"Crate1");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/Crate2.dds", TEX_CUBE, L"Furniture", L"Crate2");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/Crate3.dds", TEX_CUBE, L"Furniture", L"Crate3");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/Crate4.dds", TEX_CUBE, L"Furniture", L"Crate4");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/Glass1.dds", TEX_CUBE, L"Furniture", L"Glass1");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/Glass2.dds", TEX_CUBE, L"Furniture", L"Glass2");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/Clock.dds", TEX_CUBE, L"Furniture", L"Clock");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/Radio.dds", TEX_CUBE, L"Furniture", L"Radio");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/Stage.dds", TEX_CUBE, L"Furniture", L"Stage");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/Train.dds", TEX_CUBE, L"Furniture", L"Train");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/Gate1.dds", TEX_CUBE, L"Furniture", L"Gate1");
	Load_Texture(L"../Client/Resource/Texture/CubeTexture/Furniture/Desk.dds", TEX_CUBE, L"Furniture", L"Desk");


#pragma endregion

	Wait_LoadTextureAsync();

    return S_OK;
}

HRESULT CScene_MapTool::Ready_Proto()
{
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_CubeBufferComp", CCubeBufferComp::Create(m_pGraphicDev)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_CalculatorComp", CCalculatorComponent::Create(m_pGraphicDev)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TerrainBufferComp", CTerrainBufferComp::Create(m_pGraphicDev, L"./Resource/Texture/Terrain/FlatLand.bmp")), E_FAIL);

    // 텍스처 컴포넌트
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TerrainTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"Tile", L"22")), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SkyBoxTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_CUBE, L"TestCube", L"4")), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UITextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"UI", L"HudBox_Main")), E_FAIL);

    // 플레이어 손
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerLeftTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"Player", L"Left_RunHand")), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerRightTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"Player", L"Right_RunHand")), E_FAIL);

    // 몬스터
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MonsterTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"Boss_Single", L"CrotchHit")), E_FAIL);

    // 오브젝트 
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ObjectTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"Food", L"Cola")), E_FAIL);
    // 충돌 컴포넌트
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ColliderSphereComp", CColliderComponent::Create(m_pGraphicDev, ECOLLISION::SPHERE)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ColliderBoxComp", CColliderComponent::Create(m_pGraphicDev, ECOLLISION::BOX)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ColliderCapsuleComp", CColliderComponent::Create(m_pGraphicDev, ECOLLISION::CAPSULE)), E_FAIL);

    // 이펙트 컴포넌트
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Effect_BeamTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"Effect", L"LightBeam")), E_FAIL);

    //가건물 용도 
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BuildingTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"Building", L"A")), E_FAIL);

    //투사체 용도
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ProjectileTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"Projectile", L"ThrowPipe")), E_FAIL);

	// 프로토타입 인스턴스를 등록한다.
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TransformComp", CTransformComponent::Create(m_pGraphicDev)), E_FAIL);	// 트랜스폼
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_RcTexBufferComp", CRcBufferComp::Create(m_pGraphicDev)), E_FAIL);		// 버퍼
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_LogoTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"UI", L"Logo")), E_FAIL);	// 텍스처
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TitleBackTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"UI", L"Title")), E_FAIL);	// 텍스처

	// 플레이어 오른손
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_RightHandTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"Player", L"Right_Hand")), E_FAIL);

	// 플레이어 왼손
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_LeftHandTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"Player", L"Left_Hand")), E_FAIL);


	// 사이즈 표시 텍스처
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ClientTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"UI", L"ClientSize")), E_FAIL);	// 텍스처

    return S_OK;
}

HRESULT CScene_MapTool::Ready_Layer_Completed()
{
    Engine::CLayer* pLayer = Engine::CLayer::Create(0.f);
    NULL_CHECK_RETURN(pLayer, E_FAIL);
    m_mapLayer.emplace(L"Main", pLayer);

    Engine::CGameObject* pGameObject = nullptr;

    // SkyBox
    //FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"SkyBox", CSkyBox::Create(m_pGraphicDev)), E_FAIL);

    // Terrain
    //FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Terrain", CTerrain::Create(m_pGraphicDev)), E_FAIL);

    return S_OK;
}


void CScene_MapTool::Load_TextureAsync(const _tchar* pfilePath, TEXTUREID eID, const _tchar* pGroupName, const _tchar* pTextureName, const _range<_uint>& iCntRange)
{
	Engine::Ready_Texture(pfilePath, eID, pGroupName, pTextureName, iCntRange);
}

void CScene_MapTool::Load_Texture(const _tchar* pfilePath, TEXTUREID eID, const _tchar* pGroupName, const _tchar* pTextureName, const _range<_uint>& iCntRange, _bool bAsync)
{
	if (bAsync)
		m_vecAsyncTexture.push_back(async(launch::async, &CScene_MapTool::Load_TextureAsync, this, pfilePath, eID, pGroupName, pTextureName, iCntRange));
	else
		Engine::Ready_Texture(pfilePath, eID, pGroupName, pTextureName, iCntRange);
}

void CScene_MapTool::Wait_LoadTextureAsync()
{
	for (_uint i = 0; i < m_vecAsyncTexture.size(); i++)
	{
		m_vecAsyncTexture[i].get();
	}
	m_vecAsyncTexture.clear();
}