#include "pch.h"

#include "CCamera.h"
#include "CTransform.h"

#include "LevelMgr.h"
#include "RenderMgr.h"
#include "CinematicMgr.h"

#include "ALevel.h"
#include "Layer.h"

#include "GameObject.h"
#include "Device.h"

CCamera::CCamera()
	: Component(COMPONENT_TYPE::CAMERA)
	, m_LayerCheck(0)
	, m_OrthoScale(1.f)
	, m_IsUICam(false)
{
}

CCamera::~CCamera()
{
}


void CCamera::LayerCheckAll()
{
	/*******************************************************
	* 레이어 비트 unsinged int 자리를 모두 1로 채움
	* 16진수 f는 1바이트를 1로 모두 채운 수 * 8(부호 없는 int)
	*******************************************************/
	m_LayerCheck = 0xffffffff;
}

void CCamera::LayerCheckClear()
{
	// 모든 레이어 비트를 0으로 초기화
	m_LayerCheck = 0;
}

void CCamera::LayerCheck(int _Idx)
{
	/**********************************************
	* _Idx는 레이어의 인덱스를 나타냄
	* 비트 연산으로 어떤 레이어의 비트를 킬 것인지 설정
	*
	* x or 연산자를 사용해, 이미 켜져있는 비트가 있다면 
	* 해당 비트를 0으로 설정
	**********************************************/
	m_LayerCheck ^= (1 << _Idx);
}

void CCamera::Render()
{
	g_Trans.matView = m_matView;
	g_Trans.matProj = m_matProj;

	// Domain 순서대로 렌더링 진행
	// SortObject 함수에서, 렌더링을 할 수 있는 상태인지와 카메라 렌더링할 Layer를 모두 체크 하였음
	for (size_t i = 0; i < m_vecOpaque.size(); ++i)
		m_vecOpaque[i]->Render();

	for (size_t i = 0; i < m_vecMasked.size(); ++i)
		m_vecMasked[i]->Render();

	for (size_t i = 0; i < m_vecTransparent.size(); ++i)
		m_vecTransparent[i]->Render();

	for (size_t i = 0; i < m_vecPostProcess.size(); ++i)
		m_vecPostProcess[i]->Render();
}

void CCamera::SortObejct()
{
	// 지정한 도메인 순서별로 렌더링 순서를 정렬하는 함수 

	// 정렬 전, 이전 프레임에 등록된 게임오브젝트들을 지운다.
	m_vecOpaque.clear();
	m_vecMasked.clear();
	m_vecTransparent.clear();
	m_vecPostProcess.clear();

	Ptr<ALevel> pCurLevel = LevelMgr::GetInst()->GetCurLevel();
	// pCurLevel이 null이면 반환 (Editor 모드 일 때, nullptr 이도록 설정)
	if (pCurLevel == nullptr)
		return;

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		// 카메라가 레이어를 볼 수 있어야 함
		if (false == (m_LayerCheck & (1 << i)))
			continue;

		// 레이어에 소속된 모든 오브젝트를 가져온다
		Layer* pLayer = pCurLevel->GetLayer(i);
		const vector<Ptr<GameObject>>& vecObjects = pLayer->GetAllObjects();

		for (size_t j = 0; j < vecObjects.size(); ++j)
		{
			// 오브젝트가 렌더링을 할 수 있는 상태인지 확인
			if (nullptr == vecObjects[j]->GetRenderCom()
				|| nullptr == vecObjects[j]->GetRenderCom()->GetMesh()
				|| nullptr == vecObjects[j]->GetRenderCom()->GetMaterial())
			{
				continue;
			}

			// RENDER_DOMAIN 종류별로 분류하여 알맞은 vector에 삽입
			RENDER_DOMAIN domain = vecObjects[j]->GetRenderCom()->GetMaterial()->GetDomain();

			switch (domain)
			{
			case RENDER_DOMAIN::DOMAIN_OPAQUE:
				m_vecOpaque.push_back(vecObjects[j].Get());
				break;
			case RENDER_DOMAIN::DOMAIN_MASKED:
				m_vecMasked.push_back(vecObjects[j].Get());
				break;
			case RENDER_DOMAIN::DOMAIN_TRANSPARENT:
				m_vecTransparent.push_back(vecObjects[j].Get());
				break;
			case RENDER_DOMAIN::DOMAIN_POSTPROCESS:
				m_vecPostProcess.push_back(vecObjects[j].Get());
				break;

			}
		}
	}
}

void CCamera::Init()
{
	CinematicMgr::GetInst()->RegisterCamera(this);
}

void CCamera::Begin()
{
	// 레벨이 시작될때 호출됨
	// RenderMgr에 카메라(본인)를 등록
	RenderMgr::GetInst()->RegisterCamera(this);

	// 조건분기
	if (m_IsUICam == true)
		RenderMgr::GetInst()->RegisterUICamera(this);

	// CINEMATIC 연출용 카메라 등록
	// Clone된 레벨의 Begin() 시점에 호출되므로, 항상 현재 레벨의 카메라가 등록됨
	// UI 카메라는 연출 대상에서 제외
	if (!m_IsUICam)
		CinematicMgr::GetInst()->RegisterCamera(this);
}

void CCamera::FinalTick()
{
	// 뷰 행렬 계산
	// 카메라의 위치
	Vec3 vPos = Transform()->GetRelativePos();
	
	/********************************************
	* 이동
	* 카메라와 물체 사이의 상대적인 좌표를 구하기 위함
	* 카메라 위치를 원점으로 되돌리는 만큼의 이동행렬
	********************************************/
	Matrix matTrans = XMMatrixIdentity();
	matTrans._41 = -vPos.x;
	matTrans._42 = -vPos.y;
	matTrans._43 = -vPos.z;

	m_matView = matTrans;

	/**************************************************************************************************
	* View 행렬 회전
	* 카메라가 바라보는 방향을 z축이 되도록 회전하는 부분이 추가되어야함
	* 항등원과 역원의 개념을 행렬에 이용
	*
	* 카메라의 Right, Up, Front 방향 벡터에 회전행렬을 곱하면
	* 이게 다시 x축, y축, z축이 되는 회전행렬을 구해야 함
	*
	* vR				  ( 1 0 0)
	* vU    x    R   =    ( 0 1 0)
	* vZ				  ( 0 0 1)
	*
	*			vR
	* R 행렬은   vU     의 역행렬
	*			vZ	
	*
	*	vR
	*   vU   행렬은 행 끼리의 관계가 직교상태이기 때문에, 전치(Transpose) 를 통해서 역행렬을 쉽게 구할 수 있음
	*	vZ
	*
	* 전치한 행렬과 곱해보면, 자기자신과 내적을 한 경우 결과가 1, 다른 직교벡터랑 내적을 한 경우 0 이 나오기 때문
	* 내적은 각 성분끼리의 곱을 합친 결과, 
	* 내적 결과값의 의미는 (벡터의 길이) x (길이) x (두 벡터가 이루는 각도의 cos 값) 
	*
	*		 ( vR.x   vU.x   vF.x    0 ) 
	* R ==  ( vR.y   vU.y   vF.y    0 )
	*		 ( vR.z   vU.z   vF.z    0 )
	*       (   0     0       0     1 )
	**************************************************************************************************/

	Vec3 vR = Transform()->GetDir(DIR::RIGHT);
	Vec3 vU = Transform()->GetDir(DIR::UP);
	Vec3 vF = Transform()->GetDir(DIR::FRONT);

	Matrix matRot = XMMatrixIdentity();
	matRot._11 = vR.x;  matRot._12 = vU.x;  matRot._13 = vF.x;
	matRot._21 = vR.y;	matRot._22 = vU.y;	matRot._23 = vF.y;
	matRot._31 = vR.z;	matRot._32 = vU.z;	matRot._33 = vF.z;

	// 카메라가 원점인 공간으로 이동, 카메라가 바라보는 방향을 z 축으로 회전하는 회전을 적용
	
	// 월드 행렬과 변환 순서가 다름
	// View 행렬은 이동 -> 회전 순으로 변화
	m_matView = matTrans * matRot;

	// 투영 방식은 하나만 사용함
	// 직교투영 행렬 계산
	if (PROJ_TYPE::ORTHOGRAPHIC == m_ProjType)
		m_matProj = XMMatrixOrthographicLH(m_Width * m_OrthoScale, (m_Width / m_AspectRatio) * m_OrthoScale, 1.f, m_Far);
	else
	// 원근투영
		m_matProj = XMMatrixPerspectiveFovLH(m_FOV, m_AspectRatio, 1.f, m_Far);
}

// 멤버중 행렬 정보와 GameObject 벡터 정보는
// 엔진 시작 시 결정되기 때문에 파일 저장 X
void CCamera::SaveToLevelFile(FILE* _File)
{
	fwrite(&m_LayerCheck, sizeof(UINT), 1, _File);
	fwrite(&m_ProjType, sizeof(PROJ_TYPE), 1, _File);
	fwrite(&m_Far, sizeof(float), 1, _File);
	fwrite(&m_Width, sizeof(float), 1, _File);
	fwrite(&m_AspectRatio, sizeof(float), 1, _File);
	fwrite(&m_FOV, sizeof(float), 1, _File);
	fwrite(&m_OrthoScale, sizeof(float), 1, _File);
	fwrite(&m_IsUICam, sizeof(bool), 1, _File);
}

void CCamera::LoadFromLevelFile(FILE* _File)
{
	fread(&m_LayerCheck, sizeof(UINT), 1, _File);
	fread(&m_ProjType, sizeof(PROJ_TYPE), 1, _File);
	fread(&m_Far, sizeof(float), 1, _File);
	fread(&m_Width, sizeof(float), 1, _File);
	fread(&m_AspectRatio, sizeof(float), 1, _File);
	fread(&m_FOV, sizeof(float), 1, _File);
	fread(&m_OrthoScale, sizeof(float), 1, _File);
	fread(&m_IsUICam, sizeof(bool), 1, _File);
}

Vec3 CCamera::ScreenToWorldPos(const Vec2& _ScreenPos, float _ZNormalized /*= 0.f*/)
{
	const Vec2 res = Device::GetInst()->GetRenderResolution();

	// XMVector3Unproject expects screen coords: (x, y, z), viewport and matrices
	XMVECTOR screenVec = XMVectorSet(_ScreenPos.x, _ScreenPos.y, _ZNormalized, 0.f);

	XMVECTOR worldVec = XMVector3Unproject(
		screenVec,
		0.f, 0.f, res.x, res.y,
		0.0f, 1.0f,
		m_matProj,
		m_matView,
		XMMatrixIdentity() // world matrix (여기서는 identity)
	);

	Vec3 out(
		XMVectorGetX(worldVec),
		XMVectorGetY(worldVec),
		XMVectorGetZ(worldVec)
	);

	return out;
}

Vec2 CCamera::WorldToScreenPos(const Vec3& _WorldPos)
{
    const Vec2 res = Device::GetInst()->GetRenderResolution();

    XMVECTOR worldVec = XMVectorSet(_WorldPos.x, _WorldPos.y, _WorldPos.z, 1.f);

    // XMVector3Project: 월드 → 클립 → NDC → 뷰포트(픽셀) 변환
    XMVECTOR screenVec = XMVector3Project(
        worldVec,
        0.f, 0.f, res.x, res.y,    // 뷰포트 (x, y, w, h)
        0.f, 1.f,                   // 깊이 min/max
        m_matProj,
        m_matView,
        XMMatrixIdentity()
    );

    return Vec2(XMVectorGetX(screenVec), XMVectorGetY(screenVec));
}
