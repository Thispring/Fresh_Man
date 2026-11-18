#include "pch.h"
#include "UIMgr.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include "imguiFunc.h"

#include "KeyMgr.h"
#include "LevelMgr.h"

#include "Engine.h"
#include "Device.h"
#include "PathMgr.h"

#include "SoundMgr.h" // 추가: UI에서 사운드 제어 호출

UIMgr::UIMgr()
    : m_ShowOptions(false)
    , m_ShowCredit(false)
    , m_isFullScreen(false)
    , m_ShowExitConfirm(false)  // ← 초기화 추가
    
    , m_eventShowOption(false)
    , m_bCloseOptionsRequest(false)

    , m_eventShowCredit(false)
    , m_bCloseCreditRequest(false)

    , m_OpenFrame(0)
    , m_CreditOpenFrame(0)

    , BGMvolume(0.f)
    , SFXvolume(0.f)

    , m_bBGMMute(false)
    , m_bSFXMute(false)

    , m_PrevOptionKey(KEY::END)
    , m_PrevCreditKey(KEY::END)
    , m_PrevOptionImGuiKey(ImGuiKey_None)
    , m_PrevCreditImGuiKey(ImGuiKey_None)

    , m_isRelease(false)
{
    GetStringFromFile();
}

UIMgr::~UIMgr()
{
}

void UIMgr::Init()
{
#ifndef Debug
    m_isRelease = true;
#endif // !Debug
    m_isRelease = false;
}

void UIMgr::Progress()
{
    // Exit? 확인창이 열려 있을 때는 ESC 토글 무시
    // ESC: 팝업 "열기"만 담당 (닫기는 RenderOptionsWindow 내부에서 처리)
    if ((KEY_TAP(KEY::ESC) || m_eventShowOption) && !m_ShowExitConfirm && !m_ShowOptions)
    {
        // 프레임 번호 기록
        m_OpenFrame = ImGui::GetFrameCount();

        m_ShowOptions = true;
        // Options 창 오픈 시 UI 값을 현재 SoundMgr 상태로 동기화 (0..100 정수)
        BGMvolume = SoundMgr::GetInst()->GetBGMVolume_UI();
        SFXvolume = SoundMgr::GetInst()->GetSFXVolume_UI();
        m_bBGMMute = SoundMgr::GetInst()->IsBGMMute();
        m_bSFXMute = SoundMgr::GetInst()->IsSFXMute();
        m_eventShowOption = false;
        ChangeLevelState(LEVEL_STATE::PAUSE);
        ImGui::OpenPopup("OptionsWindow");
    }

    // Credit: IsShowCredit() 이벤트로 열기 요청
    if (m_eventShowCredit && !m_ShowCredit)
    {
        m_CreditOpenFrame = ImGui::GetFrameCount();
        m_ShowCredit = true;
        m_eventShowCredit = false;
        ImGui::OpenPopup("CreditWindow");
    }

    RenderOptionsWindow();
    RenderCreditWindow();
}

void UIMgr::RenderOptionsWindow()
{
    if (!m_ShowOptions)
        return;

    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();

    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowFocus();
    ImGui::SetNextWindowSize(ImVec2(800.f, 450.f), ImGuiCond_Always);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoResize;

    if (ImGui::BeginPopupModal("OptionsWindow", nullptr, flags))
    {

        #pragma region Option Title
        Vec4 vColor = ColorConvertIntToVec4(82.f, 82.f, 250.f);
        ImGui::PushID(0);
        ImGui::PushStyleColor(ImGuiCol_Button, vColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, vColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, vColor);
        ImGui::Button("Options", Vec2(150.f, 50.f));
        ImGui::PopStyleColor(3);
        ImGui::PopID();
        #pragma endregion
        SPACING_UI(10);
        ImGui::Separator();

        if (!m_isRelease)
        {
            #pragma region FullScreen
            ImGui::Text("Screen Size");
            SPACING_UI(2);
            if (ImGui::Checkbox("FullScreen", &m_isFullScreen))
            {
                Engine::GetInst()->ToggleFullScreen();
            }
            #pragma endregion
            SPACING_UI(5);
            ImGui::Separator();
        }

        #pragma region Volume Slide
        ImGui::Text("Music");
        SPACING_UI(2);
        
        ImGui::Text("BGM Volume");
        ImGui::SameLine(600.f);
        ImGui::Text("Mute");
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
        // BGMvolume은 0..100 정수로 표현
        float fBgm = (float)BGMvolume;
        if (ImGui::SliderFloat("##BGM", &fBgm, 0.f, 100.f))
        {
            BGMvolume = (int)fBgm;
            SoundMgr::GetInst()->SetBGMVolume_UI(BGMvolume);
        }

        ImGui::SameLine(600.f);
        if (ImGui::Checkbox("##BGMMute", &m_bBGMMute))
        {
            SoundMgr::GetInst()->SetBGMMute(m_bBGMMute);
        }

        SPACING_UI(5);
        ImGui::Text("SFX Volume");
        ImGui::SameLine(600.f);
        ImGui::Text("Mute");
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
        // SFXvolume은 0..100 정수로 표현
        float fSfx = (float)SFXvolume;
        if (ImGui::SliderFloat("##SFX", &fSfx, 0.f, 100.f))
        {
            SFXvolume = (int)fSfx;
            SoundMgr::GetInst()->SetSFXVolume_UI(SFXvolume);
        }

        ImGui::SameLine(600.f);
        if (ImGui::Checkbox("##SFXMute", &m_bSFXMute))
        {
            SoundMgr::GetInst()->SetSFXMute(m_bSFXMute);
        }
        #pragma endregion

        SPACING_UI(10);
        ImGui::Separator();

        SPACING_UI(10);
        #pragma region OptionCloseBtn
        if (ImGuiFunc::ColoredButton("Close",
            ColorConvertIntToVec4(12.f, 129.f, 207.f), ImVec2(100.f, 25.f)))
        {
            m_ShowOptions = false;
            ChangeLevelState(LEVEL_STATE::PLAY);
            ImGui::SetWindowFocus(NULL);  // ImGui 포커스 해제 → 엔진 윈도우로 복귀
            ImGui::CloseCurrentPopup();
        }
        #pragma endregion
        ImGui::SameLine(330.f);

        #pragma region MainMenuBtn
        if (ImGuiFunc::ColoredButton("MainMenu",
            ColorConvertIntToVec4(12.f, 129.f, 207.f), ImVec2(100.f, 25.f)))
        {
            // MainMenu에서 호출했다면 ChangeMainMenu 호출하지 않고
            // Popup Close만 진행하기
            if (LevelMgr::GetInst()->GetCurLevel()->GetKey() != L"Level\\MainMenu.lv")
            {
                LevelMgr::GetInst()->ChangeMainMenu();
            }
            else
            {
                m_bCloseOptionsRequest = false;  // ← 요청 플래그 초기화
                ChangeLevelState(LEVEL_STATE::PLAY);
            }
            m_ShowOptions = false;
            ImGui::SetWindowFocus(NULL);  // ImGui 포커스 해제 → 엔진 윈도우로 복귀
            ImGui::CloseCurrentPopup();


        }
        #pragma endregion
        ImGui::SameLine(690.f);

        #pragma region ExitBtn
        if (ImGuiFunc::ColoredButton("Exit",
            ColorConvertIntToVec4(12.f, 129.f, 207.f), ImVec2(100.f, 25.f)))
        {
            m_ShowExitConfirm = true;
            ImGui::OpenPopup("Exit?");
        }

        //=============================================================
        // 포커스가 이미 ImGui쪽에 가있으므로
        // 함수 내에서 ESC 눌렀을 때 작동하도록 변경
        //
        // ImGui 키 감지 — Win32 포커스와 무관하게 동작
        // 열린 직후 2프레임은 무시 (KeyMgr ↔ ImGui 입력 파이프라인 지연 보상)
        //=============================================================
        bool bSafeToClose = (ImGui::GetFrameCount() - m_OpenFrame) >= 2;
        bool bPrevKeyPressed = (m_PrevOptionImGuiKey != ImGuiKey_None)
                            && ImGui::IsKeyPressed(m_PrevOptionImGuiKey, false);
        if (bSafeToClose
            && !m_ShowExitConfirm
            && (ImGui::IsKeyPressed(ImGuiKey_Escape, false) || bPrevKeyPressed))
        {
            m_bCloseOptionsRequest = false;  // ← 요청 플래그 초기화
            m_ShowOptions = false;
            ChangeLevelState(LEVEL_STATE::PLAY);
            ImGui::SetWindowFocus(NULL);
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup(); // ← 부모 EndPopup 나중
        #pragma endregion
    }


    #pragma region ExitBtn
    // ── ImGui가 ESC로 닫았을 때 m_ShowExitConfirm이 true면 즉시 재오픈 ──
    if (m_ShowExitConfirm && !ImGui::IsPopupOpen("Exit?"))
    {
        ImGui::OpenPopup("Exit?");
    }

    // ── 중첩 팝업: 반드시 부모 EndPopup() 이전에 위치 ────────
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Exit?", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Do you want to Exit?");
        ImGui::Separator();

        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            m_ShowExitConfirm = false;  // ← 플래그 해제
            PostQuitMessage(0);
            ImGui::CloseCurrentPopup();
        }

        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            m_ShowExitConfirm = false;  // ← 플래그 해제
            m_ShowOptions = false;
            ChangeLevelState(LEVEL_STATE::PLAY);
            ImGui::SetWindowFocus(NULL);  // ImGui 포커스 해제 → 엔진 윈도우로 복귀
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup(); // ← 자식 EndPopup 먼저
    }
    #pragma endregion
}

void UIMgr::RenderCreditWindow()
{
    // 모달 방식으로 구현, 위치 고정
    // 현재 Level을 일시정지로 전환
    if (!m_ShowCredit)
        return;

    // ── 1. 화면 중앙에 위치 고정 ──────────────────────────────
    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();

    // SetNextWindowPos(위치, 조건, 피벗)
    // ImGuiCond_Always : 매 프레임 강제 고정
    // pivot (0.5, 0.5) : 창의 중심을 기준으로 정렬
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowFocus(); // 강제 포커스 (최상위 입력 처리 보장)

    // ── 크기 지정 ──────────────────────────────────────────────
    // ImGuiCond_Always : 매 프레임 강제 고정
    // ImGuiCond_Once   : 처음 한 번만 적용 (이후 사용자가 조절 가능)
    ImGui::SetNextWindowSize(ImVec2(800.f, 450.f), ImGuiCond_Always);

    // ── 2. 모달 팝업 시작 ─────────────────────────────────────
    // ImGuiWindowFlags_NoMove     : 드래그 이동 불가
    // ImGuiWindowFlags_NoResize   : 크기 조절 불가 (선택)
    // ImGuiWindowFlags_NoTitleBar : 타이틀 바 숨김    (선택)
    // ── AlwaysAutoResize 제거, NoResize 추가 ───────────────────
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoResize;   // AlwaysAutoResize 삭제
    // AlwaysAutoResize를 유지하면 SetNextWindowSize가 무시됨

    if (ImGui::BeginPopupModal("CreditWindow", nullptr, flags))
    {
        // 크기 조절을 위해 OutputTitle 함수 사용 X
        #pragma region Credit Title
        Vec4 vColor = ColorConvertIntToVec4(82.f, 82.f, 250.f);
        ImGui::PushID(0);
        ImGui::PushStyleColor(ImGuiCol_Button, vColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, vColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, vColor);
        ImGui::Button("Credit", Vec2(150.f, 50.f));
        ImGui::PopStyleColor(3);
        ImGui::PopID();
        #pragma endregion

        SPACING_UI(10);
        ImGui::Separator();
        // NOTE(26-04-07):
        // Credit창에는 사용했던 Asset 목록을 .txt 파일로 저장하여
        // ImGui에서 이를 읽어와 .txt 문자열을 출력합니다.

        ImGui::Text("License");
        vector<string>::iterator iter(m_creditList.begin());

        for (; iter < m_creditList.end(); ++iter)
        {
            ImGui::Text((*iter).c_str());
        }
        ImGui::Separator();

        #pragma region OptionCloseBtn
        if (ImGuiFunc::ColoredButton("Close",
            ColorConvertIntToVec4(12.f, 129.f, 207.f), ImVec2(100.f, 25.f)))
        {
            m_ShowCredit = false;
            ImGui::CloseCurrentPopup();
        }
        #pragma endregion

        //if (ImGui::Button("Close"))
        //{
        //    m_ShowCredit = false;
        //    ImGui::CloseCurrentPopup(); // 모달 닫기
        //}
        //SPACING_UI(10);

        bool bSafeToClose = (ImGui::GetFrameCount() - m_CreditOpenFrame) >= 2;
        bool bPrevKeyPressed = (m_PrevCreditImGuiKey != ImGuiKey_None)
                            && ImGui::IsKeyPressed(m_PrevCreditImGuiKey, false);
        if (bSafeToClose
            && (ImGui::IsKeyPressed(ImGuiKey_Escape, false) || bPrevKeyPressed))
        {
            m_bCloseCreditRequest = false;
            m_ShowCredit = false;
            ImGui::SetWindowFocus(NULL);
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    else
    {
        // 팝업이 닫혀 있음(ESC로 닫혔다든지 등).
        // 상태 플래그를 실제 ImGui 상태에 맞춰 동기화.
        m_ShowCredit = false;
        m_bCloseCreditRequest = false;
        m_eventShowCredit = false;
        // 필요하면 m_CreditOpenFrame 초기화도 수행
        m_CreditOpenFrame = 0;
    }
}

void UIMgr::GetStringFromFile()
{
    wstring txtPath = CONTENT_PATH;
    txtPath += L"CREDIT.txt";

    m_fopen.open(txtPath);

    // 파일이 열리지 않았다면
    if (!m_fopen.is_open())
        return;

    while (!m_fopen.eof())	// 파일의 끝을 만날 때 까지
    {
        // 계산을 위한 임시 string
        string tmp;
        getline(m_fopen, tmp);
        m_creditList.push_back(tmp);
    }

    // 읽기가 끝났다면 파일 닫기
    m_fopen.close();
}

void UIMgr::IsShowOptions(KEY _key)
{
    m_PrevOptionKey = _key;
    m_PrevOptionImGuiKey = ImGuiFunc::KeyToImGuiKey(_key);

    if (m_ShowOptions)
        m_bCloseOptionsRequest = true;
    else
        m_eventShowOption = true;
}

void UIMgr::ResetOptionKey()
{
    m_PrevOptionKey = KEY::END;
    m_PrevOptionImGuiKey = ImGuiKey_None;
}

void UIMgr::IsShowCredit(KEY _key)
{
    m_PrevCreditKey = _key;
    m_PrevCreditImGuiKey = ImGuiFunc::KeyToImGuiKey(_key);

    if (m_ShowCredit)
        m_bCloseCreditRequest = true;
    else
        m_eventShowCredit = true;
}

void UIMgr::ResetCreditKey()
{
    m_PrevCreditKey = KEY::END;
    m_PrevCreditImGuiKey = ImGuiKey_None;
}
