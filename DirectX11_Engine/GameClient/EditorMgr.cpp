#include "pch.h"
#include "EditorMgr.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#include "Engine.h"
#include "Device.h"

#include "KeyMgr.h"

#include "Inspector.h"
#include "Outliner.h"
#include "Menu.h"
#include "KeyList.h"
#include "ListUI.h"
#include "ContentUI.h"
#include "TreeUI.h"
#include "UIMgr.h"

#include "SpriteMaker.h"
#include "FlipbookMaker.h"
#include "TileMapMaker.h"
#include "MaterialMaker.h"
#include "GameObjectMaker.h"
#include "PrefabMaker.h"
#include "LevelMaker.h"

#include "CEditorCamMoveScript.h"
#include "RenderMgr.h"


EditorMgr::EditorMgr()
    : m_ShowDemo(false)     // ImGui Demo UI 활성/비활성
    , m_UIVisible(true)     // 기본값: UI 표시
    , m_isRelease(false)
{
}

EditorMgr::~EditorMgr()
{
    // imgui에서 동적할당 해제 관련 함수를
    // EditorMgr의 소멸자에서 호출
    
    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    // Window 해제 부분은
    // 이 프로젝트에서 담당하기 때문에 사용 X
}

void EditorMgr::Tick()
{
    //==============
    // Editor Object
    //==============
    // Editor Object 함수 호출
    for (const auto& Object : m_vecEditorObj)
        Object->Tick();

    for (const auto& Object : m_vecEditorObj)
        Object->FinalTick_Editor();


    //==========
    // Editor UI
    //==========
    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // UI 매니저 업데이트
    // 전체화면 모드 사용과 상관없이 인게임에서 사용될 UI
    UIMgr::GetInst()->Progress();

    // 전체화면 모드 시 UI 창을 그리지 않음 (ImGui 프레임은 유지)
    if (!m_UIVisible)
        return;

    m_FocusedUI = nullptr;
    
    if (nullptr != m_FocusedUI)
        KeyMgr::GetInst()->SetActive(false);
    else
        KeyMgr::GetInst()->SetActive(true);

    if (m_isRelease)
        return;

    // DemoUI 활성/비활성화
    // Enable/Disable DemoUI
    //if (KEY_TAP(KEY::F8))
    //    m_ShowDemo ? m_ShowDemo = false : m_ShowDemo = true;

    // Enter를 누르면 현재 선택된 ImGui의 포커스를 해제시킴
    // Clears the focus of the currently selected ImGui item when Enter is pressed.
    //if (KEY_TAP(KEY::ENTER))
    //    ImGui::SetWindowFocus(nullptr);

    // DemoUI, ImGui의 기능을 살펴보기 위해 사용
    // Used to explore the features of DemoUI and ImGui.
    if (m_ShowDemo)
        ImGui::ShowDemoWindow(&m_ShowDemo);

    // EditorUI
    for (const auto& pair : m_mapUI)
    {
        if (pair.second->IsActive())
            pair.second->Tick();
    }

    
    /*******************************************************
    * ImGui는 Begin과 End 사이에 원하는 기능을 추가하여,
    * 매 프레임 호출되어, 해당 내용에 대한 Render를 진행합니다.
    *******************************************************/
}

void EditorMgr::Render()
{
    // Main Window 렌더링
    // Rendering
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Main Window를 벗어났을 때의 렌더링
    // ImGui 자신만의 SwapChain을 생성해 렌더링을 진행
    // Update and Render additional Platform Windows
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

void EditorMgr::CreateEditorUI()
{
    // 파생 클래스를 동적할당으로 생성하고 등록
    Ptr<EditorUI> pUI = nullptr;
    
    //===========================
    // 새로운 ImGui 창은 여기서 등록
    //===========================
    pUI = NEW Menu;
    AddUI(pUI->GetUIName(), pUI);

    pUI = NEW Inspector;
    AddUI(pUI->GetUIName(), pUI);

    pUI = NEW Outliner;
    AddUI(pUI->GetUIName(), pUI);

    pUI = NEW KeyList;
    AddUI(pUI->GetUIName(), pUI);

    pUI = NEW ListUI;
    pUI->SetModal(true);    // Modal 방식으로 설정하려면 여기에서 true로 설정
    pUI->SetActive(false);
    AddUI(pUI->GetUIName(), pUI);

    pUI = NEW ContentUI;
    AddUI(pUI->GetUIName(), pUI);

    // Maker UI들은 디폴트로 비활성화 처리
    pUI = NEW SpriteMaker;
    pUI->SetActive(false);
    AddUI(pUI->GetUIName(), pUI);

    pUI = NEW FlipbookMaker;
    pUI->SetActive(false);
    AddUI(pUI->GetUIName(), pUI);

    pUI = NEW TileMapMaker;
    pUI->SetActive(false);
    AddUI(pUI->GetUIName(), pUI);

    pUI = NEW MaterialMaker;
    pUI->SetActive(false);
    AddUI(pUI->GetUIName(), pUI);

    pUI = NEW GameObjectMaker;
    pUI->SetActive(false);
    AddUI(pUI->GetUIName(), pUI);

    pUI = NEW PrefabMaker;
    pUI->SetActive(false);
    AddUI(pUI->GetUIName(), pUI);

    pUI = NEW LevelMaker;
    pUI->SetActive(false);
    AddUI(pUI->GetUIName(), pUI);

}

void EditorMgr::CreateEditorObject()
{
    // Editor에서 사용하는 카메라 오브젝트 생성
    Ptr<GameObject> pObject = nullptr;
    pObject = NEW GameObject;
    pObject->SetName(L"EditorCamera");

    pObject->AddComponent(NEW CTransform);
    pObject->AddComponent(NEW CCamera);
    pObject->AddComponent(NEW CEditorCamMoveScript);
    
    pObject->Transform()->SetRelativePos(Vec3(0.f, 0.f, -350.f));

    pObject->Camera()->LayerCheckAll();

    pObject->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
    pObject->Camera()->SetFar(1000.f);
    pObject->Camera()->SetFOV(90.f);
    pObject->Camera()->SetOrthoScale(1.f);

    Vec2 vResolution = Device::GetInst()->GetRenderResolution();
    pObject->Camera()->SetAspectRatio(vResolution.x / vResolution.y);	// 종횡비(AspectRatio)
    pObject->Camera()->SetWidth(vResolution.x);							// height 멤버가 없는대신, width 정보만 설정하고 종횡비를 사용

    m_vecEditorObj.push_back(pObject);

    // Editor 용 카메라를 RenderMgr에 등록
    RenderMgr::GetInst()->RegisterEditorCamera(pObject->Camera());
}

void EditorMgr::Init()
{
#ifdef _DEBUG
    // 디버그 빌드에서만 실행되는 코드
    m_isRelease = false;
#endif

#ifndef _DEBUG
    // 릴리즈 빌드에서만 실행되는 코드
    m_isRelease = true;
#endif

    float main_scale = ImGui_ImplWin32_GetDpiScaleForHwnd(Engine::GetInst()->GetMainWndHwnd());

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;
    //io.ConfigDockingAlwaysTabBar = true;
    //io.ConfigDockingTransparentPayload = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)
    io.ConfigDpiScaleFonts = true;          // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. This will scale fonts but _NOT_ scale sizes/padding for now.
    io.ConfigDpiScaleViewports = true;      // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(Engine::GetInst()->GetMainWndHwnd());
    ImGui_ImplDX11_Init(DEVICE, CONTEXT);

    // EditorUI 생성
    CreateEditorUI();

    // Editor용 Object 생성
    CreateEditorObject();
}

void EditorMgr::Progress()
{
    Tick();
    Render();
}

void EditorMgr::AddUI(const string& _UIName, Ptr<EditorUI> _UI)
{
    Ptr<EditorUI> pUI = FindUI(_UIName);
    assert(nullptr == pUI);
    m_mapUI.insert(make_pair(_UIName, _UI));
}

Ptr<EditorUI> EditorMgr::FindUI(const string& _UIName)
{
    map<string, Ptr<EditorUI>>::iterator iter = m_mapUI.find(_UIName);

    if (iter == m_mapUI.end())
        return nullptr;

    return iter->second;
}
