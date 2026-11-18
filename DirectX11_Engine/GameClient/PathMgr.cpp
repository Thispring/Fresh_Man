#include "pch.h"
#include "PathMgr.h"

PathMgr::PathMgr() = default;

PathMgr::~PathMgr() = default;

HRESULT PathMgr::Init()
{
	wstring executablePath(MAX_PATH, L'\0');
	DWORD length = GetModuleFileNameW(nullptr, executablePath.data(), static_cast<DWORD>(executablePath.size()));
	while (length == executablePath.size())
	{
		executablePath.resize(executablePath.size() * 2);
		length = GetModuleFileNameW(nullptr, executablePath.data(), static_cast<DWORD>(executablePath.size()));
	}
	if (length == 0)
		return HRESULT_FROM_WIN32(GetLastError());

	// Game/Bin의 실행 파일을 기준으로 Game/Content를 찾습니다.
	executablePath.resize(length);
	m_ContentPath = (fs::path(executablePath).parent_path().parent_path() / L"Content").wstring() + L'\\';
	return S_OK;
}
