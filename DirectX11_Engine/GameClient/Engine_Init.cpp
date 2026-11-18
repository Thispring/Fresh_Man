#include "pch.h"

#include "Engine.h"
#include "Device.h"

#include "PathMgr.h"
#include "TimeMgr.h"
#include "KeyMgr.h"
#include "AssetMgr.h"
#include "LevelMgr.h"
#include "RenderMgr.h"
#include "EditorMgr.h"
#include "FontMgr.h"
#include "UIMgr.h"
#include "CinematicMgr.h"
#include "ScoreMgr.h"
#include "SoundMgr.h"

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int Engine::Init(HINSTANCE _hInst, UINT _Width, UINT _Height, bool _EditorMode)
{
    m_hInst = _hInst;

    m_Resolution = Vec2(_Width, _Height);

    m_EditorMode = _EditorMode;

    /****************************************
    * 생성시킬 윈도우(창) 옵션 설정
    * 기본 제공은 함수를 통해 구조체를 초기화하고
    * 해당 구조체의 주소를 리턴합니다.
    ****************************************/
    WNDCLASSEXW wcex = {};

    // wcex에 lpszClassName는 WCHAR이기 때문에, 문자열 앞에 L을 붙여야합니다.
    wcex.lpszClassName = L"MyGame";

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc; // 아래 있는 프로시저 함수, 함수 포인터 등록
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = m_hInst;
    wcex.hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_GAMECLIENT));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr; // MAKEINTRESOURCEW(IDC_GAMECLIENT);   // 메뉴바, nullptr로 설정 시 없어짐
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    RegisterClassExW(&wcex);

    /******************************************************************
    * 위에서 구조체로 등록한 윈도우 설정 데이터를 기반으로, 윈도우(창) 하나 생성
    * CreatewindowW의 szWindowClass이 일치하지 않으면 창 생성이 안됩니다.
    *
    * CreateWindowW는 11개의 요소를 가진것으로 정의되어있습니다.
    * 정의된 이름을 보고 윈도우 창 설정을 변경할 수 있습니다.
    ******************************************************************/

    // PopUp 모드로 타이틀 창 제거
    //HWND hWnd = CreateWindowW(L"MyGame", L"MyGame", WS_POPUP/*WS_OVERLAPPEDWINDOW*/, CW_USEDEFAULT, 0, CW_USEDEFAULT
    //                         , 0, nullptr, nullptr, hInstance, nullptr);        
    UINT Style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;


    // 커널 오브젝트 - OS가 관리하는 객체
    // 핸들        - 커널 오브젝트의 ID 개념
    m_hWnd = CreateWindowW(L"MyGame",       // lpClassName
        L"MyGame",                          // lpWindowName
        Style,                              // dwStyle => 타이틀 창, WS_POP으로 하면 타이틀 창이 안보임
        CW_USEDEFAULT,                      // x
        0,                                  // y
        CW_USEDEFAULT,                      // nWidth   => 모니터 해상도에 맞춤, 만약 1920을 Width로 가진모니터에서 
                                            //             1920을 초과한 값을 넣어도 모니터에 넘어서 출력할 수 없습니다.
        0,                                  // nHeight
        nullptr,                            // hWndParent
        nullptr,                            // hMenu
        m_hInst,                            // hInstance
        nullptr);                           // lpParam


    // 만약 창 생성이 실패하면 1을 반환하여 프로그램 종료
    if (!m_hWnd)
    {
        return E_FAIL;
    }

    /**********************************************************************
    * 생성한 윈도우를 화면에 보이도록 설정
    * ShowWindow(hWnd, nCmdShow); // 첫 번째 "화면에 출력하고 싶은 HWND핸들", 
    * 두 번째는 "인자로 받은 HWND핸들을 보여줄지 않보여줄지"를 설정
    * SetWindowsPos는 어느 위치에 어느 크기로 HWND 핸들을 배치할지 설정하는 함수
    **********************************************************************/
    ShowWindow(m_hWnd, true);
    UpdateWindow(m_hWnd);

    // 메뉴바가 있는지 확인해주는 함수
    //GetMenu(m_hWnd);

    RECT rt = {0, 0, m_Resolution.x , m_Resolution.y};

    /**********************************************************
    * 실제 게임 창은 윈도우 크기보다 작을 수 있기에 해결하기 위한 함수
    * 타이틀 창의 크기를 고려해서, 의도한 윈도우 크기를 반환
    **********************************************************/
    AdjustWindowRect(&rt, Style, !!GetMenu(m_hWnd));

    /***************************************************
    * 윈도우 크기 및 위치 변경
    * 윈도우는 좌상단이 (0, 0) 픽셀기준으로 아래로 내려갑니다.     
    ***************************************************/
    SetWindowPos(m_hWnd, nullptr, 0, 0, rt.right - rt.left, rt.bottom - rt.top, 0);

    //// ===================================
    //// CPU를 이용해 검은색으로 모든 픽셀 채우기
    //HDC dc = GetDC(m_hWnd);
    //HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
    //SelectObject(dc, hBrush);
    //Rectangle(dc, 0, 0, 2000, 2000);
    //// ===================================

    // Dx11로 라이브러리를 이용한 Device를 참조해 초기화
    if (FAILED(Device::GetInst()->Init(m_hWnd, m_Resolution)))
    {
        return E_FAIL;
    }

    // 실행파일과 같이 있는 Content 폴더의 경로를 계산
    if (FAILED(PathMgr::GetInst()->Init()))
    {
        MessageBoxW(m_hWnd, L"실행 파일 경로를 확인할 수 없습니다.", L"콘텐츠 경로 초기화 실패", MB_OK | MB_ICONERROR);
        return E_FAIL;
    }

    // 1프레임 동안 동작하는데 걸리는 시간(Delta Time) 계산
    TimeMgr::GetInst()->Init();

    // 고정 프레임 설정
    TimeMgr::GetInst()->SetTargetFPS(240);

    // 키보드 동작 상태 계산
    KeyMgr::GetInst()->Init();

    // FMOD 초기화	
    // Sound 에셋을 가지고 오려면 미리 해야함
    {
        FMOD::System_Create(&m_FMODSystem);
        assert(m_FMODSystem);

        // 32개 채널 생성
        FMOD_RESULT result = m_FMODSystem->init(32, FMOD_DEFAULT, nullptr);
    }
    
    // 엔진 기본 Asset들 생성
    if (FAILED(AssetMgr::GetInst()->Init()))
        return E_FAIL;

    // Asset Load
    AssetMgr::GetInst()->LoadContent();

    // Level 초기화
    LevelMgr::GetInst()->Init();

    // Render 초기화
    RenderMgr::GetInst()->Init();

    // SoundMgr 초기화
    SoundMgr::GetInst()->Init();

    // CinematicMgr 초기화
    CinematicMgr::GetInst()->Init();

    // Font 매니저 초기화
    FontMgr::GetInst()->Init(L"Font\\_bitmap_font____romulus_by_pix3m-d6aokem.ttf", L"Romulus");

    // 콘텐츠 점수 매니저 초기화
    ScoreMgr::GetInst()->Init();

    // Editor 초기화(imgui)
    if (m_EditorMode)
        EditorMgr::GetInst()->Init();

    // UI 매니저 초기화
    UIMgr::GetInst()->Init();

    return S_OK;
}
