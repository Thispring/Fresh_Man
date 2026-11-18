#include "pch.h"
#include "ScoreMgr.h"

#include "FontMgr.h"
#include "LevelMgr.h"
#include "RenderMgr.h"
#include "Device.h"

int ScoreMgr::m_Score = 0;
int ScoreMgr::m_HighScore = 0;

ScoreMgr::ScoreMgr()
{

}

ScoreMgr::~ScoreMgr()
{
	// 프로그램 종료 시 자동 저장
	SaveHighScore();
}

void ScoreMgr::LoadHighScore()
{
	FILE* pFile = nullptr;
	_wfopen_s(&pFile, HIGHSCORE_FILE, L"rb");

	if (pFile == nullptr)
	{
		// 파일 없으면 최초 실행 → 0으로 유지
		m_HighScore = 0;
		return;
	}

	fread(&m_HighScore, sizeof(int), 1, pFile);
	fclose(pFile);
}

void ScoreMgr::SaveHighScore()
{
	FILE* pFile = nullptr;
	_wfopen_s(&pFile, HIGHSCORE_FILE, L"wb");

	if (pFile == nullptr)
		return;

	fwrite(&m_HighScore, sizeof(int), 1, pFile);
	fclose(pFile);
}


void ScoreMgr::Init()
{
	// 프로그램 시작 시 저장된 HighScore 불러오기
	LoadHighScore();
}

void ScoreMgr::Progress()
{
	RenderHighScore();
}

void ScoreMgr::SaveScore(float _RemainingTime, int _DeathCount)
{
	// 게임 Ending Level에서 호출 or Ending Level 전환 전에
	// 남은 시간과 플레이어 DeathCount에 비례해서 점수 계산
	// 
	// 점수 계산 공식:
	// GoalTime이 많이 남아 있을 수록 빨리 클리어 했다는 것이므로 높은 점수
	// DeathCount가 적을 수록, 적게 죽은 것이므로 높은 점수
    // 1. 시간 점수 계산
	// _RemainingTime을 분과 초(정수)로 변환하여 MMSS 형태로 점수화 (예: 5분43초 -> 543)
	// 최대값은 10분(1000)
	int totalSeconds = static_cast<int>(_RemainingTime);
	if (totalSeconds < 0) totalSeconds = 0;
	int minutes = totalSeconds / 60;
	int seconds = totalSeconds % 60;

	int timeScore = minutes * 100 + seconds;
	if (timeScore > 1000) timeScore = 1000;

	// 2. 생존 점수 계산 - 최대 1000점에서 고정 차감
	const int maxSurvival = 1000;
	const int deathPenalty = 100; // 한 번 죽을 때마다 100점 차감
	int survivalScore = maxSurvival - (_DeathCount * deathPenalty);
	if (survivalScore < 0) survivalScore = 0;

	// 3. 최종 합산 및 검증 (최대 2000점)
	int finalScore = timeScore + survivalScore;
	if (finalScore > 2000)
	{
		// 2000을 초과하면 오류가 있으므로 경고
		wchar_t buf[256];
		swprintf_s(buf, L"ScoreMgr Warning: final score %d exceeds maximum 2000\n", finalScore);
		OutputDebugStringW(buf);
	}
	m_Score = finalScore;

	if (m_Score > m_HighScore)
	{
		m_HighScore = m_Score;
		SaveHighScore();
	}
}

void ScoreMgr::RenderHighScore()
{
	// 최고 점수가 아직 기록되지 않았고,
	// MainMenu가 아니라면 렌더 X
	if (m_HighScore <= 0)
		return;

	const Vec2 res = Device::GetInst()->GetRenderResolution();
	wstring score = L"HighScore:  " + to_wstring(m_HighScore);

	// 텍스트 크기 측정
	Vec2 textSize = FontMgr::GetInst()->MeasureText(score.c_str(), 48.f);

	// X축 중앙 정렬
	float posX = (res.x - textSize.x) / 2.f;
	float posY = res.y * 0.25f;

	FontMgr::GetInst()->DrawFontOutline(score.c_str(),
		posX, posY, 48.f,
		FONT_RGBA(255, 255, 0, 255),
		FONT_RGBA(0, 0, 0, 255),
		1.5f);

}

void ScoreMgr::RenderCurScore()
{
	const Vec2 res = Device::GetInst()->GetRenderResolution();
	wstring score = L"Current Score:  " + to_wstring(m_Score);

	// 텍스트 크기 측정
	Vec2 textSize = FontMgr::GetInst()->MeasureText(score.c_str(), 48.f);

	// X축 중앙 정렬
	float posX = (res.x - textSize.x) / 2.f;
	float posY = res.y * 0.3f;

	FontMgr::GetInst()->DrawFontOutline(score.c_str(),
		posX, posY, 48.f,
		FONT_RGBA(201, 201, 201, 255),
		FONT_RGBA(0, 0, 0, 255),
		1.5f);

}

