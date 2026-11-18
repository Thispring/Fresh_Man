/***************************************************
* 포트폴리오 제작을 위한 엔진 분리 .Ver
***************************************************/

// GameClient.cpp : 애플리케이션에 대한 진입점을 정의합니다.
#include "pch.h"    
#include "Engine.h"

#include "Entity.h"
#include "Asset.h"
#include "AMesh.h"
#include "KeyMgr.h"
#include "TimeMgr.h"
#include "imgui/imgui_impl_win32.h"

// 전역 변수:
HINSTANCE hInst;                                    // 현재 인스턴스입니다.
WCHAR szTitle[100] = L"GameClient";                 // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[100] = L"MyGame";               // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

/***************************************
* Windows의 전용 main()함수 => wWinMain
* SAL : 주석 언어 => _In_, _In_opt_
***************************************/
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,  // 레거시 코드
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    // ↓ 프로그램 시작 시점 기록
    auto t_ProgramStart = std::chrono::steady_clock::now();


    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);   // 메모리 누수 추적
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    
    /************************************************************************************
    * 함수 호출규약
    * __cdecl: 호출자가 스택을 정리
    * __stdcall: 호출된 함수쪽에서 스택을 정리
    *
    * 함수 포인터
    * 반환값이 없고, __cdecl 호출 규약이면서,
    * 인자를 받지않는 함수의 주소만 받을 수 있는 포인터 선언
    * void(__cdecl * pFunc)(void);
    * int(*pIntFunc)(void);
    *
    * 프로세스는 여러개의 윈도우를 가질 수도, 하나도 가지지 않을 수도 있다.
    * ex) 작업관리자에서 창 없이 실행되는 백그라운드 프로그램들은 창을 가지지 않는다.
    * 게임 화면을 출력시킬 윈도우가 반드시 필요한 프로그램
    * 따라서 윈도우가 생성되지 않거나, 윈도우를 종료하면, 곧 프로그램 종료로 싱크를 맞춰둔 상태
    ***********************************************************************************/

    // 윈도우 창 생성 완료 시점
    hInst = hInstance;

    // 메인 창을 만들기 전에 모니터별 DPI 인식을 활성화합니다.
    ImGui_ImplWin32_EnableDpiAwareness();

    // Engine 초기화
    if (FAILED(Engine::GetInst()->Init(hInstance, 1600, 900, true)))
        return 0;

    /*****************************************************************************************************************
    * 메시지 루프
    * 매크로에서 ##은 글자를 이어붙이겠다는 뜻
    *
    * H~가 붙은 커널 오브젝트들은 핸들의 역할을함
    * 
    * 커널 오브젝트는 Windows에서 사용자에게 공개하지않고 ID를 제공합니다.
    * 하지만 ID의 숫자로만 봐서는 어떤 종류의 커널 오브젝트인지 구분 할 수 없고 (중복 문제가 발생할 수 있음, 가독성이 낮음)
    * 따라서 서로 호환되지 않게 하기 위해 커널 오브젝트 별로 포인터 타입 자료형을 만들어서 이름을 재정의 합니다.
    * => 정의를 확인해보면 구조체, 포인터, typedef를 활용하고 있음
    *
    * 커널 오브젝트들은 DECLARE_HANDLE로 정의되어 있음
    * #define DECLARE_HANDLE(name) struct name##__{int unused;}; typedef struct name##__ *name
    * HPEN은 DECLARE_HANDLE(HPEN)로 정의되어 있고, 
    * DECLARE_HANDLE(HPEN)은 아래와 같다.
    *****************************************************************************************************************/
    struct HPEN__
    { 
        int unused; 
    }; 
    typedef struct HPEN__* HPEN;

    /*************************************
    * HANDLE 커널 오브젝트는 void* 타입으로
    * 다른 핸들 포인터를 받을 수 있게 설계
    * typedef void *HANDLE;
    *************************************/

    // hAccelTable은 Windows에 정의되어있는 단축키 테이블 핸들
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GAMECLIENT));

    // 윈도우에 발생한 사건(이벤트, 메시지)를 받을 수 있는 구조체
    MSG msg = {};

    /************************************************************************************
    * 윈도우에 발생한 이벤트, 사건, 메시지를 가져온다.
    * 메시지 큐에서 메시지를 가져온다.
    * 가져오는 메세지가 WM_QUIT 이면, false 를 반환한다.
    *
    * GetMessage 함수는 메세지 큐에 메세지가 없으면, 리턴하지 않는다
    * 메세지가 있으면, 반환한다. 메세지가 WM_QUIT 이 아니면 true, 
    * 메세지가 WM_QUIT 인 경우에만 리턴을 false 리턴한다.
    * while (GetMessage(&msg, nullptr, 0, 0))
    * {
    *     // 단축키와 관련된 내용이면 TranslateAccelerator에서 처리
    *     if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
    *     {
    *         // 단축키와 관련된 내용이 아니면 아래 함수를 이용해 처리
    *         TranslateMessage(&msg);
    *         DispatchMessage(&msg);
    *     }
    * }
    *
    * PeekMessage 함수는 메세지 큐에 메시지가 있어도 없어도 함수를 리턴 시킨다.
    * 메세지가 있었으면 true, 없었으면 false 를 반환
    * Peek - 몰래 보다
    * 확인한 메세지를 GetMessage 함수처럼 제거하려면, PM_REMOVE 옵션을 추가인자로 넣어줘야 합니다.
    ************************************************************************************/
    while (true)
    {
        //===============================================================================
        // NOTE(26-04-02):
        // ImGui 쪽 지역 함수에 string 객체 생성 시
        // 누수 발생 확인, 다른 함수에서도 string 객체 생성 때 누수 발생 가능성 있음
        //===============================================================================
        //_CrtSetBreakAlloc(101411);
        // 메세지 큐에서 메세지를 꺼낸게 있다.
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;

            // 단축키 관련된 내용이면 TranslateAccelerator 함수에서 처리
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                // 단축키 관련된 이벤트가 아니면 TranslateMessage, DispatchMessage 함수를 이용해서 처리
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        // 메세지 큐에 메세지가 없었다.
        else
        {
            // 목표 FPS에 도달하지 못했으면 이번 루프는 건너뜀
            if (TimeMgr::GetInst()->WaitForTargetFrame())
                continue;

            // Game 실행
            // 메시지가 없는 시간동안 계속 실행
            if (FAILED(Engine::GetInst()->Progress()))
                break;
        }
    }

    // ↓ while 루프 탈출 직후 종료 시점 기록
    auto t_ProgramEnd = std::chrono::steady_clock::now();

    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t_ProgramEnd - t_ProgramStart).count();
    double elapsed_s = elapsed_ms / 1000.0;

    char logBuf[256];
    snprintf(logBuf, sizeof(logBuf),
        "\n"
        "===============================================================\n"
        "[Runtime] Total Elapsed Time : %lld ms (%.2f seconds)\n"
        "===============================================================\n",
        elapsed_ms, elapsed_s);

    // VS Output 창 출력
    OutputDebugStringA(logBuf);

    // PowerShell / 터미널에서 실행한 경우, 부모 콘솔에 출력
    //if (AttachConsole(ATTACH_PARENT_PROCESS))
    //{
    //    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    //    DWORD written = 0;
    //    WriteConsoleA(hOut, logBuf, static_cast<DWORD>(strlen(logBuf)), &written, nullptr);
    //}

    /*************************************************
    * 실제 누수가 있는지 확인하려면, 아래 주석을 해제하고
    * 메모리 누수가 의심되는 구간에서 프로그램을 종료합니다.
    * 4 byte만 누수가 발생하는지 확인합니다.
    *************************************************/
    //int* p = NEW int;
    //long long* p2 = new long long;
    
    // NOTE(26-04-02): 아래 함수를 main 이 부분에 넣으면
    // static 키워드가 누수로 잡힘, static은 프로그램이 끝나야 해제가됨
    //_CrtDumpMemoryLeaks();
    // 창이 꺼지면 while 루프가 나가지면서 프로그램 종료
    return (int) msg.wParam;
}

// imgui
LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


/**************************************************
* 프로시저 함수
* 윈도우에 발생한 사건(이벤트, 메시지)를 처리해주는 함수
**************************************************/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // 마우스 휠은 윈도우 이벤트이기 때문에, main에서 처리
    KeyMgr::GetInst()->ProcessMessage(message, wParam, lParam);
    
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return true;

    // 매개변수로 들어온 메시지를 조건문을 통해 찾음
    // 마우스 클릭, 키보드 입력 등이 메시지로 정의되어 있음
    switch (message)
    {
    case WM_LBUTTONDOWN:
        //PostQuitMessage(0); // 윈도우 종료함수
        break;
    case WM_COMMAND:
        {
            //if (HIWORD(wParam) == BN_CLICKED)
            //{
            //    switch (LOWORD(wParam))   // 버튼 ID
            //    {
            //    case 2001:
            //        //Engine::GetInst()->ToggleFullScreen();
            //        break;
            //    }
            //}
            //break;

            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    //case WM_CLOSE:
    //    // 아무것도 하지 않거나, 확인 창을 띄움
    //    // return 0; 을 하면 프로그램이 종료되지 않습니다.
    //    return 0;
    //    break;
    case WM_SYSKEYDOWN:
        if (wParam == VK_MENU) // Alt 키만 눌렸을 때
            return 0; // 메뉴 포커스(또는 짧은 beep) 방지
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
