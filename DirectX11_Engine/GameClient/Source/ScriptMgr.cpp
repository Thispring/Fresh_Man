#include "pch.h"
#include "ScriptMgr.h"
#include <cstring>

#include "Scripts/CCamMoveScript.h"
#include "Scripts/CEndingLevelController.h"
#include "Scripts/CEnemyAnimator.h"
#include "Scripts/CEnemyData.h"
#include "Scripts/CEnemyEyes.h"
#include "Scripts/CEnemySpawner.h"
#include "Scripts/CEnemyStateManager.h"
#include "Scripts/CEnergyBlast.h"
#include "Scripts/CFlowerProjectile.h"
#include "Scripts/CGameOverController.h"
#include "Scripts/CIntroLevelController.h"
#include "Scripts/CMainMenuController.h"
#include "Scripts/CMovingPlatform.h"
#include "Scripts/CParallaxLayer.h"
#include "Scripts/CPlayerAnimator.h"
#include "Scripts/CPlayerController.h"
#include "Scripts/CPlayerData.h"
#include "Scripts/CPlayerMeleeTrigger.h"
#include "Scripts/CPlayerStateManager.h"
#include "Scripts/CPortal.h"
#include "Scripts/CSavePoint.h"
#include "Scripts/CUICamMoveScript.h"
#include "Scripts/CUIOverlayController.h"

void ScriptMgr::GetScriptInfo(vector<wstring>& _vec)
{
	_vec.push_back(L"CCamMoveScript");
	_vec.push_back(L"CEndingLevelController");
	_vec.push_back(L"CEnemyAnimator");
	_vec.push_back(L"CEnemyData");
	_vec.push_back(L"CEnemyEyes");
	_vec.push_back(L"CEnemySpawner");
	_vec.push_back(L"CEnemyStateManager");
	_vec.push_back(L"CEnergyBlast");
	_vec.push_back(L"CFlowerProjectile");
	_vec.push_back(L"CGameOverController");
	_vec.push_back(L"CIntroLevelController");
	_vec.push_back(L"CMainMenuController");
	_vec.push_back(L"CMovingPlatform");
	_vec.push_back(L"CParallaxLayer");
	_vec.push_back(L"CPlayerAnimator");
	_vec.push_back(L"CPlayerController");
	_vec.push_back(L"CPlayerData");
	_vec.push_back(L"CPlayerMeleeTrigger");
	_vec.push_back(L"CPlayerStateManager");
	_vec.push_back(L"CPortal");
	_vec.push_back(L"CSavePoint");
	_vec.push_back(L"CUICamMoveScript");
	_vec.push_back(L"CUIOverlayController");
}

CScript * ScriptMgr::GetScript(const wstring& _strScriptName)
{
	if (L"CCamMoveScript" == _strScriptName)
		return NEW CCamMoveScript;
	if (L"CEndingLevelController" == _strScriptName)
		return NEW CEndingLevelController;
	if (L"CEnemyAnimator" == _strScriptName)
		return NEW CEnemyAnimator;
	if (L"CEnemyData" == _strScriptName)
		return NEW CEnemyData;
	if (L"CEnemyEyes" == _strScriptName)
		return NEW CEnemyEyes;
	if (L"CEnemySpawner" == _strScriptName)
		return NEW CEnemySpawner;
	if (L"CEnemyStateManager" == _strScriptName)
		return NEW CEnemyStateManager;
	if (L"CEnergyBlast" == _strScriptName)
		return NEW CEnergyBlast;
	if (L"CFlowerProjectile" == _strScriptName)
		return NEW CFlowerProjectile;
	if (L"CGameOverController" == _strScriptName)
		return NEW CGameOverController;
	if (L"CIntroLevelController" == _strScriptName)
		return NEW CIntroLevelController;
	if (L"CMainMenuController" == _strScriptName)
		return NEW CMainMenuController;
	if (L"CMovingPlatform" == _strScriptName)
		return NEW CMovingPlatform;
	if (L"CParallaxLayer" == _strScriptName)
		return NEW CParallaxLayer;
	if (L"CPlayerAnimator" == _strScriptName)
		return NEW CPlayerAnimator;
	if (L"CPlayerController" == _strScriptName)
		return NEW CPlayerController;
	if (L"CPlayerData" == _strScriptName)
		return NEW CPlayerData;
	if (L"CPlayerMeleeTrigger" == _strScriptName)
		return NEW CPlayerMeleeTrigger;
	if (L"CPlayerStateManager" == _strScriptName)
		return NEW CPlayerStateManager;
	if (L"CPortal" == _strScriptName)
		return NEW CPortal;
	if (L"CSavePoint" == _strScriptName)
		return NEW CSavePoint;
	if (L"CUICamMoveScript" == _strScriptName)
		return NEW CUICamMoveScript;
	if (L"CUIOverlayController" == _strScriptName)
		return NEW CUIOverlayController;
	return nullptr;
}

CScript * ScriptMgr::GetScript(UINT _iScriptType)
{
	switch (_iScriptType)
	{
	case (UINT)SCRIPT_TYPE::CAMMOVESCRIPT:
		return NEW CCamMoveScript;
		break;
	case (UINT)SCRIPT_TYPE::ENDINGLEVELCONTROLLER:
		return NEW CEndingLevelController;
		break;
	case (UINT)SCRIPT_TYPE::ENEMYANIMATOR:
		return NEW CEnemyAnimator;
		break;
	case (UINT)SCRIPT_TYPE::ENEMYDATA:
		return NEW CEnemyData;
		break;
	case (UINT)SCRIPT_TYPE::ENEMYEYES:
		return NEW CEnemyEyes;
		break;
	case (UINT)SCRIPT_TYPE::ENEMYSPAWNER:
		return NEW CEnemySpawner;
		break;
	case (UINT)SCRIPT_TYPE::ENEMYSTATEMANAGER:
		return NEW CEnemyStateManager;
		break;
	case (UINT)SCRIPT_TYPE::ENERGYBLAST:
		return NEW CEnergyBlast;
		break;
	case (UINT)SCRIPT_TYPE::FLOWERPROJECTILE:
		return NEW CFlowerProjectile;
		break;
	case (UINT)SCRIPT_TYPE::GAMEOVERCONTROLLER:
		return NEW CGameOverController;
		break;
	case (UINT)SCRIPT_TYPE::INTROLEVELCONTROLLER:
		return NEW CIntroLevelController;
		break;
	case (UINT)SCRIPT_TYPE::MAINMENUCONTROLLER:
		return NEW CMainMenuController;
		break;
	case (UINT)SCRIPT_TYPE::MOVINGPLATFORM:
		return NEW CMovingPlatform;
		break;
	case (UINT)SCRIPT_TYPE::PARALLAXLAYER:
		return NEW CParallaxLayer;
		break;
	case (UINT)SCRIPT_TYPE::PLAYERANIMATOR:
		return NEW CPlayerAnimator;
		break;
	case (UINT)SCRIPT_TYPE::PLAYERCONTROLLER:
		return NEW CPlayerController;
		break;
	case (UINT)SCRIPT_TYPE::PLAYERDATA:
		return NEW CPlayerData;
		break;
	case (UINT)SCRIPT_TYPE::PLAYERMELEETRIGGER:
		return NEW CPlayerMeleeTrigger;
		break;
	case (UINT)SCRIPT_TYPE::PLAYERSTATEMANAGER:
		return NEW CPlayerStateManager;
		break;
	case (UINT)SCRIPT_TYPE::PORTAL:
		return NEW CPortal;
		break;
	case (UINT)SCRIPT_TYPE::SAVEPOINT:
		return NEW CSavePoint;
		break;
	case (UINT)SCRIPT_TYPE::UICAMMOVESCRIPT:
		return NEW CUICamMoveScript;
		break;
	case (UINT)SCRIPT_TYPE::UIOVERLAYCONTROLLER:
		return NEW CUIOverlayController;
		break;
	}
	return nullptr;
}

const wchar_t * ScriptMgr::GetScriptName(CScript * _pScript)
{
	switch ((SCRIPT_TYPE)_pScript->GetScriptType())
	{
	case SCRIPT_TYPE::CAMMOVESCRIPT:
		return L"CCamMoveScript";
		break;

	case SCRIPT_TYPE::ENDINGLEVELCONTROLLER:
		return L"CEndingLevelController";
		break;

	case SCRIPT_TYPE::ENEMYANIMATOR:
		return L"CEnemyAnimator";
		break;

	case SCRIPT_TYPE::ENEMYDATA:
		return L"CEnemyData";
		break;

	case SCRIPT_TYPE::ENEMYEYES:
		return L"CEnemyEyes";
		break;

	case SCRIPT_TYPE::ENEMYSPAWNER:
		return L"CEnemySpawner";
		break;

	case SCRIPT_TYPE::ENEMYSTATEMANAGER:
		return L"CEnemyStateManager";
		break;

	case SCRIPT_TYPE::ENERGYBLAST:
		return L"CEnergyBlast";
		break;

	case SCRIPT_TYPE::FLOWERPROJECTILE:
		return L"CFlowerProjectile";
		break;

	case SCRIPT_TYPE::GAMEOVERCONTROLLER:
		return L"CGameOverController";
		break;

	case SCRIPT_TYPE::INTROLEVELCONTROLLER:
		return L"CIntroLevelController";
		break;

	case SCRIPT_TYPE::MAINMENUCONTROLLER:
		return L"CMainMenuController";
		break;

	case SCRIPT_TYPE::MOVINGPLATFORM:
		return L"CMovingPlatform";
		break;

	case SCRIPT_TYPE::PARALLAXLAYER:
		return L"CParallaxLayer";
		break;

	case SCRIPT_TYPE::PLAYERANIMATOR:
		return L"CPlayerAnimator";
		break;

	case SCRIPT_TYPE::PLAYERCONTROLLER:
		return L"CPlayerController";
		break;

	case SCRIPT_TYPE::PLAYERDATA:
		return L"CPlayerData";
		break;

	case SCRIPT_TYPE::PLAYERMELEETRIGGER:
		return L"CPlayerMeleeTrigger";
		break;

	case SCRIPT_TYPE::PLAYERSTATEMANAGER:
		return L"CPlayerStateManager";
		break;

	case SCRIPT_TYPE::PORTAL:
		return L"CPortal";
		break;

	case SCRIPT_TYPE::SAVEPOINT:
		return L"CSavePoint";
		break;

	case SCRIPT_TYPE::UICAMMOVESCRIPT:
		return L"CUICamMoveScript";
		break;

	case SCRIPT_TYPE::UIOVERLAYCONTROLLER:
		return L"CUIOverlayController";
		break;

	}
	return nullptr;
}

const char* ScriptMgr::GetScriptName(SCRIPT_TYPE _Type)
{
	switch (_Type)
	{
	case CAMMOVESCRIPT: return "CCamMoveScript";
		break;

	case ENDINGLEVELCONTROLLER: return "CEndingLevelController";
		break;

	case ENEMYANIMATOR: return "CEnemyAnimator";
		break;

	case ENEMYDATA: return "CEnemyData";
		break;

	case ENEMYEYES: return "CEnemyEyes";
		break;

	case ENEMYSPAWNER: return "CEnemySpawner";
		break;

	case ENEMYSTATEMANAGER: return "CEnemyStateManager";
		break;

	case ENERGYBLAST: return "CEnergyBlast";
		break;

	case FLOWERPROJECTILE: return "CFlowerProjectile";
		break;

	case GAMEOVERCONTROLLER: return "CGameOverController";
		break;

	case INTROLEVELCONTROLLER: return "CIntroLevelController";
		break;

	case MAINMENUCONTROLLER: return "CMainMenuController";
		break;

	case MOVINGPLATFORM: return "CMovingPlatform";
		break;

	case PARALLAXLAYER: return "CParallaxLayer";
		break;

	case PLAYERANIMATOR: return "CPlayerAnimator";
		break;

	case PLAYERCONTROLLER: return "CPlayerController";
		break;

	case PLAYERDATA: return "CPlayerData";
		break;

	case PLAYERMELEETRIGGER: return "CPlayerMeleeTrigger";
		break;

	case PLAYERSTATEMANAGER: return "CPlayerStateManager";
		break;

	case PORTAL: return "CPortal";
		break;

	case SAVEPOINT: return "CSavePoint";
		break;

	case UICAMMOVESCRIPT: return "CUICamMoveScript";
		break;

	case UIOVERLAYCONTROLLER: return "CUIOverlayController";
		break;

	case SCRIPT_TYPE_END: return "End";
		break;

	default:
		break;
	}

	return nullptr;
}


CScript * ScriptMgr::GetScript(const char* _strScriptName)
{
	if (0 == strcmp(_strScriptName, "CCamMoveScript"))
		return NEW CCamMoveScript;
	if (0 == strcmp(_strScriptName, "CEndingLevelController"))
		return NEW CEndingLevelController;
	if (0 == strcmp(_strScriptName, "CEnemyAnimator"))
		return NEW CEnemyAnimator;
	if (0 == strcmp(_strScriptName, "CEnemyData"))
		return NEW CEnemyData;
	if (0 == strcmp(_strScriptName, "CEnemyEyes"))
		return NEW CEnemyEyes;
	if (0 == strcmp(_strScriptName, "CEnemySpawner"))
		return NEW CEnemySpawner;
	if (0 == strcmp(_strScriptName, "CEnemyStateManager"))
		return NEW CEnemyStateManager;
	if (0 == strcmp(_strScriptName, "CEnergyBlast"))
		return NEW CEnergyBlast;
	if (0 == strcmp(_strScriptName, "CFlowerProjectile"))
		return NEW CFlowerProjectile;
	if (0 == strcmp(_strScriptName, "CGameOverController"))
		return NEW CGameOverController;
	if (0 == strcmp(_strScriptName, "CIntroLevelController"))
		return NEW CIntroLevelController;
	if (0 == strcmp(_strScriptName, "CMainMenuController"))
		return NEW CMainMenuController;
	if (0 == strcmp(_strScriptName, "CMovingPlatform"))
		return NEW CMovingPlatform;
	if (0 == strcmp(_strScriptName, "CParallaxLayer"))
		return NEW CParallaxLayer;
	if (0 == strcmp(_strScriptName, "CPlayerAnimator"))
		return NEW CPlayerAnimator;
	if (0 == strcmp(_strScriptName, "CPlayerController"))
		return NEW CPlayerController;
	if (0 == strcmp(_strScriptName, "CPlayerData"))
		return NEW CPlayerData;
	if (0 == strcmp(_strScriptName, "CPlayerMeleeTrigger"))
		return NEW CPlayerMeleeTrigger;
	if (0 == strcmp(_strScriptName, "CPlayerStateManager"))
		return NEW CPlayerStateManager;
	if (0 == strcmp(_strScriptName, "CPortal"))
		return NEW CPortal;
	if (0 == strcmp(_strScriptName, "CSavePoint"))
		return NEW CSavePoint;
	if (0 == strcmp(_strScriptName, "CUICamMoveScript"))
		return NEW CUICamMoveScript;
	if (0 == strcmp(_strScriptName, "CUIOverlayController"))
		return NEW CUIOverlayController;
	return nullptr;
}


SCRIPT_TYPE ScriptMgr::GetScriptType(const std::wstring& _strScriptName)
{
	if (L"CCamMoveScript" == _strScriptName)
		return CAMMOVESCRIPT;
	if (L"CEndingLevelController" == _strScriptName)
		return ENDINGLEVELCONTROLLER;
	if (L"CEnemyAnimator" == _strScriptName)
		return ENEMYANIMATOR;
	if (L"CEnemyData" == _strScriptName)
		return ENEMYDATA;
	if (L"CEnemyEyes" == _strScriptName)
		return ENEMYEYES;
	if (L"CEnemySpawner" == _strScriptName)
		return ENEMYSPAWNER;
	if (L"CEnemyStateManager" == _strScriptName)
		return ENEMYSTATEMANAGER;
	if (L"CEnergyBlast" == _strScriptName)
		return ENERGYBLAST;
	if (L"CFlowerProjectile" == _strScriptName)
		return FLOWERPROJECTILE;
	if (L"CGameOverController" == _strScriptName)
		return GAMEOVERCONTROLLER;
	if (L"CIntroLevelController" == _strScriptName)
		return INTROLEVELCONTROLLER;
	if (L"CMainMenuController" == _strScriptName)
		return MAINMENUCONTROLLER;
	if (L"CMovingPlatform" == _strScriptName)
		return MOVINGPLATFORM;
	if (L"CParallaxLayer" == _strScriptName)
		return PARALLAXLAYER;
	if (L"CPlayerAnimator" == _strScriptName)
		return PLAYERANIMATOR;
	if (L"CPlayerController" == _strScriptName)
		return PLAYERCONTROLLER;
	if (L"CPlayerData" == _strScriptName)
		return PLAYERDATA;
	if (L"CPlayerMeleeTrigger" == _strScriptName)
		return PLAYERMELEETRIGGER;
	if (L"CPlayerStateManager" == _strScriptName)
		return PLAYERSTATEMANAGER;
	if (L"CPortal" == _strScriptName)
		return PORTAL;
	if (L"CSavePoint" == _strScriptName)
		return SAVEPOINT;
	if (L"CUICamMoveScript" == _strScriptName)
		return UICAMMOVESCRIPT;
	if (L"CUIOverlayController" == _strScriptName)
		return UIOVERLAYCONTROLLER;
	return SCRIPT_TYPE_END;
}


SCRIPT_TYPE ScriptMgr::GetScriptType(const char* _strScriptName)
{
	if (0 == strcmp(_strScriptName, "CCamMoveScript"))
		return CAMMOVESCRIPT;
	if (0 == strcmp(_strScriptName, "CEndingLevelController"))
		return ENDINGLEVELCONTROLLER;
	if (0 == strcmp(_strScriptName, "CEnemyAnimator"))
		return ENEMYANIMATOR;
	if (0 == strcmp(_strScriptName, "CEnemyData"))
		return ENEMYDATA;
	if (0 == strcmp(_strScriptName, "CEnemyEyes"))
		return ENEMYEYES;
	if (0 == strcmp(_strScriptName, "CEnemySpawner"))
		return ENEMYSPAWNER;
	if (0 == strcmp(_strScriptName, "CEnemyStateManager"))
		return ENEMYSTATEMANAGER;
	if (0 == strcmp(_strScriptName, "CEnergyBlast"))
		return ENERGYBLAST;
	if (0 == strcmp(_strScriptName, "CFlowerProjectile"))
		return FLOWERPROJECTILE;
	if (0 == strcmp(_strScriptName, "CGameOverController"))
		return GAMEOVERCONTROLLER;
	if (0 == strcmp(_strScriptName, "CIntroLevelController"))
		return INTROLEVELCONTROLLER;
	if (0 == strcmp(_strScriptName, "CMainMenuController"))
		return MAINMENUCONTROLLER;
	if (0 == strcmp(_strScriptName, "CMovingPlatform"))
		return MOVINGPLATFORM;
	if (0 == strcmp(_strScriptName, "CParallaxLayer"))
		return PARALLAXLAYER;
	if (0 == strcmp(_strScriptName, "CPlayerAnimator"))
		return PLAYERANIMATOR;
	if (0 == strcmp(_strScriptName, "CPlayerController"))
		return PLAYERCONTROLLER;
	if (0 == strcmp(_strScriptName, "CPlayerData"))
		return PLAYERDATA;
	if (0 == strcmp(_strScriptName, "CPlayerMeleeTrigger"))
		return PLAYERMELEETRIGGER;
	if (0 == strcmp(_strScriptName, "CPlayerStateManager"))
		return PLAYERSTATEMANAGER;
	if (0 == strcmp(_strScriptName, "CPortal"))
		return PORTAL;
	if (0 == strcmp(_strScriptName, "CSavePoint"))
		return SAVEPOINT;
	if (0 == strcmp(_strScriptName, "CUICamMoveScript"))
		return UICAMMOVESCRIPT;
	if (0 == strcmp(_strScriptName, "CUIOverlayController"))
		return UIOVERLAYCONTROLLER;
	return SCRIPT_TYPE_END;
}


void ScriptMgr::GenerateScriptFiles(const std::wstring& _strScriptName)
{
	if (_strScriptName.empty()) return;
	std::wstring scriptFolder = L"C:\\Users\\OGS\\Documents\\GitHub\\Fresh_Man\\DirectX11_Engine\\GameClient\\Source\\Scripts\\";
	if (scriptFolder.empty()) return;

	std::wstring headerPath = scriptFolder + _strScriptName + L".h";
	std::wstring cppPath    = scriptFolder + _strScriptName + L".cpp";

	FILE* pH = nullptr;
	if (_wfopen_s(&pH, headerPath.c_str(), L"w") == 0 && pH != nullptr)
	{
		fclose(pH);
	}
	FILE* pC = nullptr;
	if (_wfopen_s(&pC, cppPath.c_str(), L"w") == 0 && pC != nullptr)
	{
		fclose(pC);
	}
}
