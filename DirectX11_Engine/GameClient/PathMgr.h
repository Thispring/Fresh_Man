#pragma once

// 프로젝트에 생성한 폴더 경로를 관리합니다.
class PathMgr
	: public Singleton<PathMgr>
{
	SINGLE(PathMgr)
private:
	wstring m_ContentPath;

public:
	//=========
	// 멤버 함수
	//=========
	HRESULT Init();


	//=========
	// Get, Set
	//=========
	const wchar_t* GetContentPath() const { return m_ContentPath.c_str(); }
};

// 자주 사용되는 함수 재정의
#define CONTENT_PATH PathMgr::GetInst()->GetContentPath()
// wstring 버전
#define WCONTENT_PATH wstring(PathMgr::GetInst()->GetContentPath())
