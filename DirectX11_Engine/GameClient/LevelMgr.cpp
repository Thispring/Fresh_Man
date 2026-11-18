#include "pch.h"
#include "GameObject.h"
#include "Device.h"

#include "LevelMgr.h"
#include "AssetMgr.h"
#include "TimeMgr.h"
#include "CollisionMgr.h"
#include "GameMgr.h"
#include "RandomMgr.h"
#include "KeyMgr.h"
#include "UIMgr.h"
#include "CinematicMgr.h"
#include "SoundMgr.h"

#include "Source/Scripts/CCamMoveScript.h"

#include "Source/Scripts/CEnemySpawner.h"
#include "Source/Scripts/CPlayerAnimator.h"
#include "CSpriteRender.h"

LevelMgr::LevelMgr()
	: m_LevelState(LEVEL_STATE::STOP)
{
}

LevelMgr::~LevelMgr()
{
}

// 모든 Layer를 검색해서
// 매개변수로 받은 이름의 오브젝트 래퍼런스를 반환
Ptr<GameObject> LevelMgr::FindObjectByName(const wstring& _Name)
{
	return m_CurLevel->FindObjectByName(_Name);
}

void LevelMgr::AddNewObject(Ptr<GameObject> _Object, Ptr<ALevel> _Level, int _Layer)
{
	// assert는 false로 걸릴 조건을 등록
	assert(_Object != nullptr, _Level != nullptr, _Layer >= 0 && _Layer <= 32);
	

	// TaskMgr로 CREATE_OBJECT 등록
	// TaskMgr에서는 현재 Level에 Object를 등록
	CreateObject(_Object.Get(), _Layer);
}

void LevelMgr::ChangeMainMenu()
{
	// Main만 아니면 됨
	if (m_CurLevel->GetKey() == L"Level\\MainMenu.lv")
		return;

	::ChangeLevel(L"Level\\MainMenu.lv");
	::ChangeLevelState(LEVEL_STATE::PLAY);
}

void LevelMgr::GameStart()
{
	// MainMenu가 아니면 실행 X
	// 이미 Normal_Stage 여도 실행 X
	if (m_CurLevel->GetKey() != L"Level\\MainMenu.lv" ||
		m_CurLevel->GetKey() == L"Level\\Normal_Stage_0.lv")
		return;

	// func.cpp의 ChangeLevel 함수를 호출하려면 스코프연산자::를 앞에 붙입니다.
	// Level_0으로 전환 전, MainMenu에서 등록된 Option/Credit Key 초기화
	UIMgr::GetInst()->ResetOptionKey();
	UIMgr::GetInst()->ResetCreditKey();

	// Play transition SFX when moving from MainMenu to Level 0
	SoundMgr::GetInst()->PlaySFX(L"Level_Move");

	::ChangeLevel(L"Level\\Normal_Stage_0.lv");
	::ChangeLevelState(LEVEL_STATE::CINEMATIC);
}

void LevelMgr::ChangeEnding()
{
	// Main에서는 전환 X
	if (m_CurLevel->GetKey() == L"Level\\MainMenu.lv" ||
		m_CurLevel->GetKey() == L"Level\\Ending.lv" ||
		m_CurLevel->GetKey() == L"Level\\GameOver.lv")
		return;

	::ChangeLevel(L"Level\\Ending.lv");
	::ChangeLevelState(LEVEL_STATE::PLAY);
}

void LevelMgr::ChangeGameOver()
{
	// Main, Ending 에서 전환 X
	if (m_CurLevel->GetKey() == L"Level\\MainMenu.lv" ||
		m_CurLevel->GetKey() == L"Level\\Ending.lv")
		return;

	::ChangeLevel(L"Level\\GameOver.lv");
	::ChangeLevelState(LEVEL_STATE::PLAY);
}

void LevelMgr::ChangeLevelState(LEVEL_STATE _NextState)
{
	// 이미 같은 상태였다면 함수 실행 X
	if (m_LevelState == _NextState)
		return;

	/**********************************************************************************
	* Level 복사 후, Clone 함수 호출전에 
	* GameMgr::GetInst()->LevelPlayInit() 이 호출되면 복사 되기전 Player가 GameMgr에 등록
	**********************************************************************************/

	// MainMenu Level이 PLAY 상태에서 전환됨
	// PLAY -> CINEMATIC 전환
	if (m_LevelState == LEVEL_STATE::STOP && _NextState == LEVEL_STATE::CINEMATIC)
	{
		// 상태를 먼저 설정한 뒤 Clone/Begin 수행
		m_LevelState = _NextState;

		GameMgr::GetInst()->ClearLevelPlay();

		m_CurLevel = m_ShardLevel->Clone();
		m_CurLevel->SetChanged();
		m_CurLevel->Begin();

		// 레벨 오브젝트 준비 완료 후 카메라 탐색 및 원점 기록
		CinematicMgr::GetInst()->Init();

		return;
	}

	//====================
	// PLAY <-> PAUSE 전환
	//====================
	if (m_LevelState == LEVEL_STATE::PLAY && _NextState == LEVEL_STATE::PAUSE)
	{
		// 이미 Progress 에서 LevelState가 PLAY여야 Tick과 같은 로직을 수행하게 했으므로
		// 전달받은 Level 상태만 변경합니다.
		m_LevelState = _NextState;
		return;
	}

	if (m_LevelState == LEVEL_STATE::PAUSE && _NextState == LEVEL_STATE::PLAY)
	{
		m_LevelState = _NextState;
		return;
	}

	// Stop -> Play 전환
	if (m_LevelState == LEVEL_STATE::STOP && _NextState == LEVEL_STATE::PLAY)
	{
		// Begin() 내부에서 PLAY 상태를 전제하는 코드가 있으므로
		// Clone/Begin 이전에 먼저 상태를 설정한다.
		m_LevelState = _NextState;

		// 복사하기전 Level에 있는 발사체 오브젝트 삭제
		GameMgr::GetInst()->ClearLevelPlay();

		// 원본 Level의 복제본을 만들고 현재 Level로 가리키게 한다.
		m_CurLevel = m_ShardLevel->Clone();
		m_CurLevel->SetChanged();
		m_CurLevel->Begin();
	}
	// Play or Pause -> Stop 전환
	else if ((m_LevelState == LEVEL_STATE::PLAY || m_LevelState == LEVEL_STATE::PAUSE)
		&& _NextState == LEVEL_STATE::STOP)
	{
		m_CurLevel = m_ShardLevel;
		m_CurLevel->SetChanged();
	}

	// 다음 시작할 Level이 Play 상태라면 GameMgr 초기화
	if (_NextState == LEVEL_STATE::PLAY)
	{
		// NOTE(26-04-09): CINEMATIC 전환 이후, 한 번만 이곳을 호출해야 하며,
		// PLAY <-> PAUSE 전환은 바로 return을 하기에 이곳에 들어올 수 없음

		// 이전에 재생하던 BGM이 있다면 정지하고
		SoundMgr::GetInst()->StopPrevBGM();
		// BGM 재생
		SoundMgr::GetInst()->PlayBGM(m_CurLevel->GetKey());
		
		// 콘텐츠 관리 매니저 초기화
		GameMgr::GetInst()->Init();
		// 콘텐츠에 사용할 난수 초기화
		RandomMgr::GetInst()->Init();

		// 콘텐츠 관련 시간 초기화
		TimeMgr::GetInst()->InitPlayTime();
		TimeMgr::GetInst()->InitGoalTime();
	}

	m_LevelState = _NextState;
}

void LevelMgr::ChangeLevel(Ptr<ALevel> _NextLevel)
{
	m_CurLevel = m_ShardLevel = _NextLevel;

	// 변경점이 있다면 파일로 저장
	m_CurLevel->Save(CONTENT_PATH + m_CurLevel->GetKey());

	m_LevelState = LEVEL_STATE::STOP;
	_NextLevel->SetChanged();
}

void LevelMgr::Init()
{
	// NOTE(26-04-10): 릴리즈 빌드에서
	// 해당 주석 해제
#ifndef _DEBUG
	// 릴리즈 빌드에서만 실행되는 코드
	::ChangeLevel(L"Level\\Intro.lv");
	::ChangeLevelState(LEVEL_STATE::PLAY);
#endif

}

void LevelMgr::Progress()
{
	if (m_CurLevel == nullptr)
		return;

	// 이전에 등록된 모든 오브젝트들 vector에서 제거
	m_CurLevel->Deregister();

	// 레벨 안에 있는 오브젝트들이 이번 DT 동안 할 일 수행
	// (26-03-03): 레벨의 상태가 Play일 때만 Level의 Tick을 수행합니다.
	if (m_LevelState == LEVEL_STATE::PLAY)
		m_CurLevel->Tick();

	// 카메라 연출 진행 중: 목표 위치 도달 시 Play 전환
	if (m_LevelState == LEVEL_STATE::CINEMATIC)
	{
		if (CinematicMgr::GetInst()->CameraMove())
			::ChangeLevelState(LEVEL_STATE::PLAY);
	}
	
	// Object의 Component가 호출하는 것이므로(Render 관련 연산을 FinalTick에서 진행), 
	// FinalTick은 계속 호출합니다.
	m_CurLevel->FinalTick();

	// 게임 오브젝트들의 충돌 판정
	if (m_LevelState == LEVEL_STATE::PLAY)
		CollisionMgr::GetInst()->Progress(m_CurLevel);


	// 호출 순서상 맨 아래에 배치해야
	// 검은색으로 렌더가 되는 현상 방지할 수 있음
}
