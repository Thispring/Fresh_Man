#include "pch.h"
#include "Engine.h"
#include "Device.h"

#include "TimeMgr.h"
#include "KeyMgr.h"
#include "LevelMgr.h"
#include "RenderMgr.h"
#include "TaskMgr.h"
#include "EditorMgr.h"
#include "FontMgr.h"
#include "CinematicMgr.h"
#include "UIMgr.h"
#include "ScoreMgr.h"
#include "SoundMgr.h"
#include "GameMgr.h"

Engine::Engine()
	: // 명시되어 있지 않지만, 상속받은 부모의 생성자가 숨어있음
	m_hInst(nullptr)
	, m_hWnd(nullptr)
	, m_Resolution{}
	, m_EditorMode(true)
	, m_FullScreen(false)

	, m_FMODSystem(nullptr)
{
}

Engine::~Engine()
{
	if (nullptr != m_FMODSystem)
		m_FMODSystem->release();
}

int Engine::Progress() 
{
	// DeltaTime 계산
	TimeMgr::GetInst()->Tick();

	// 입력과 렌더링이 같은 클라이언트 픽셀 크기를 사용하도록 맞춥니다.
	RECT clientRect = {};
	if (GetClientRect(m_hWnd, &clientRect) && clientRect.right > 0 && clientRect.bottom > 0)
	{
		const Vec2 clientSize(static_cast<float>(clientRect.right), static_cast<float>(clientRect.bottom));
		Device* device = Device::GetInst();
		if (clientSize != device->GetRenderResolution() && FAILED(device->ResizeSwapChain(clientSize)))
			return E_FAIL;
	}

	// Key 상태 계산
	KeyMgr::GetInst()->Tick();

	// F5 전체화면 토글
	//if (KEY_TAP(KEY::F5))
	//	ToggleFullScreen();

	// Level 업데이트
	LevelMgr::GetInst()->Progress();

	// 렌더링
	RenderMgr::GetInst()->Progress();

	// FPS Render
	TimeMgr::GetInst()->Render();

	// 점수 렌더링
	if (LevelMgr::GetInst()->GetCurLevel() != nullptr
		&& LevelMgr::GetInst()->GetCurLevel()->GetKey() == L"Level\\MainMenu.lv"
		&& LevelMgr::GetInst()->GetLevelState() == LEVEL_STATE::PLAY)
	{
		ScoreMgr::GetInst()->Progress();
	}

	// Sound 매니저 업데이트
	SoundMgr::GetInst()->Progress();

	// CinematicMgr 업데이트
	CinematicMgr::GetInst()->Progress();

	// GameMgr 업데이트
	GameMgr::GetInst()->Progress();

	// Ending Level 용 Render
	FontMgr::GetInst()->PrintEnding();

	// GameOver
	FontMgr::GetInst()->PrintGameOver();

	// imgui Editor 관리
	if (m_EditorMode)
		EditorMgr::GetInst()->Progress();

	/******************************************************************************
	* 렌더링은 CPU를 이용하거나, GPU를 이용해야 합니다.
	*   => 알맞은 API 함수를 활용
	*	=> CPU를 쓰고싶다면 Windows에서 제공하는 GDI(Graphics Device Interface)함수 이용
	*	=> GPU를 쓰고싶다면 Directx API(함수)를 이용해서 GPU를 컨트롤하고 명령해야 합니다.
	*
	* CPU는 싱글 스레드(단일 코어 성능)에 최적
	*	=> 난제를 풀기에 특화
	* GPU는 병렬처리에 최적 (성능이 CPU보다 낮지만, 코어 숫자가 많음)
	*	=> 노가다에 특화
	*
	* 컴퓨터 화면은 여러개의 픽셀로 나뉘어져 있음 (1차원 배열 형태로)
	* 1920x1080 모니터는 2,073,600의 픽셀로 이루어져 있습니다.
	* 색상정보는 4바이트에 RGB+A 정보를 담고 있음
	* RGB 1바이트당 0~255 범위에서 숫자로 표현
	******************************************************************************/

	// 렌더타겟에 그려진 그림을, 윈도우 비트맵으로 복사
	Device::GetInst()->Present();

	// 다음 프레임에 적용될 작업 관리
	TaskMgr::GetInst()->Progress();

	return S_OK;
}

void Engine::ToggleFullScreen()
{
	m_FullScreen = !m_FullScreen;

	if (m_FullScreen)
	{
		// 현재 창이 위치한 모니터 정보를 얻음 (가장 가까운 모니터)
		HMONITOR hMon = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi = {};
		mi.cbSize = sizeof(mi);
		GetMonitorInfo(hMon, &mi);

		// 전체 모니터 영역 사용 (작업 표시줄 제외를 원하면 mi.rcWork 사용)
		int monitorLeft = mi.rcMonitor.left;
		int monitorTop = mi.rcMonitor.top;
		int monitorW = mi.rcMonitor.right - mi.rcMonitor.left;
		int monitorH = mi.rcMonitor.bottom - mi.rcMonitor.top;

		// 모니터 크기에 맞춰 16:9 비율로 최대 크기 계산
		int targetW = monitorW;
		int targetH = (targetW * 9) / 16;

		if (targetH > monitorH)
		{
			// 높이가 넘치면 높이에 맞추고 너비 조정
			targetH = monitorH;
			targetW = (targetH * 16) / 9;
		}

		// 중앙 배치 오프셋 계산
		int offsetX = monitorLeft + (monitorW - targetW) / 2;
		int offsetY = monitorTop + (monitorH - targetH) / 2;

		// 윈도우 스타일을 무테(WS_POPUP)로 변경하고 위치/크기 적용
		SetWindowLongPtr(m_hWnd, GWL_STYLE, WS_POPUP);
		SetWindowPos(m_hWnd, HWND_TOP, offsetX, offsetY, targetW, targetH, SWP_FRAMECHANGED | SWP_SHOWWINDOW);

		if (m_EditorMode)
			EditorMgr::GetInst()->SetUIVisible(false);
	}
	else
	{
		// 타이틀 창 복원, 기본 해상도(1600x900)로 복귀
		UINT uStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
		SetWindowLongPtr(m_hWnd, GWL_STYLE, uStyle);

		RECT rt = { 0, 0, (LONG)m_Resolution.x, (LONG)m_Resolution.y };
		AdjustWindowRect(&rt, uStyle, !!GetMenu(m_hWnd));
		SetWindowPos(m_hWnd, nullptr, 0, 0, rt.right - rt.left, rt.bottom - rt.top, SWP_FRAMECHANGED | SWP_SHOWWINDOW);

		if (m_EditorMode)
			EditorMgr::GetInst()->SetUIVisible(true);
	}

	// 릴리즈 빌드에서 아래 주석 해제
	// Ensure our KeyMgr processes keys while editor UI is hidden
	//KeyMgr::GetInst()->SetActive(true);
}
