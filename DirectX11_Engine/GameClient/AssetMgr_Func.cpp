#include "pch.h"

#include "AssetMgr.h"
#include "PathMgr.h"

#include "GameObject.h"
#include "Device.h"
#include "Source/Scripts/CCamMoveScript.h"

void AssetMgr::CreateEngineMesh()
{
	/*************************************
	* 정점 순서는 좌측최상단 부터, 시작
	* 왼쪽 -> 오른쪽 -> 다음줄 왼쪽 -> ...
	* 내려가면서 순차적으로 작성
	*
	* 정점 좌표에 따라 UV 좌표 설정
	*************************************/

	//============
	// 정사각형 매쉬 
	//============
	Ptr<AMesh> pSquareMesh = nullptr;
	Vtx squarVtx[4] = {};

	squarVtx[0].vPos = Vec3(-0.5f, 0.5f, 0.f);
	squarVtx[0].vUV = Vec2(0.f, 0.f);
	squarVtx[0].vColor = Vec4(1.f, 0.f, 0.f, 0.f);

	squarVtx[1].vPos = Vec3(0.5f, 0.5f, 0.f);
	squarVtx[1].vUV = Vec2(1.f, 0.f);
	squarVtx[1].vColor = Vec4(0.f, 0.f, 1.f, 0.f);

	squarVtx[2].vPos = Vec3(0.5f, -0.5f, 0.f);
	squarVtx[2].vUV = Vec2(1.f, 1.f);
	squarVtx[2].vColor = Vec4(0.f, 1.f, 0.f, 0.f);

	squarVtx[3].vPos = Vec3(-0.5f, -0.5f, 0.f);
	squarVtx[3].vUV = Vec2(0.f, 1.f);
	squarVtx[3].vColor = Vec4(1.f, 0.f, 0.f, 0.f);

	// 인덱스 버퍼
	UINT arrSquareIdx[6] = { 0, 2, 3, 0, 1, 2 };

	// g_RectMesh 포인터를 활용해서, 동적할당
	// 사각형 매쉬 생성
	pSquareMesh = NEW AMesh;
	pSquareMesh->Create(squarVtx, 4, arrSquareIdx, 6);

	// 에셋 매니저 등록
	AddAsset(L"SquareMesh", pSquareMesh.Get());


	// ====================
	// SquareMesh_LineStrip
	// ====================
	arrSquareIdx[0] = 0; 	arrSquareIdx[1] = 1;	arrSquareIdx[2] = 2;	arrSquareIdx[3] = 3; arrSquareIdx[4] = 0;
	pSquareMesh = NEW AMesh;
	pSquareMesh->Create(squarVtx, 4, arrSquareIdx, 5);
	AddAsset(L"SquareMesh_LineStrip", pSquareMesh.Get());


	//============
	// 직사각형 매쉬
	//============
	Ptr<AMesh> pRectMesh = nullptr;
	Vtx rectVtx[4] = {};

	rectVtx[0].vPos = Vec3(-0.5f, 0.5f, 0.f);
	rectVtx[0].vUV = Vec2(0.f, 0.f);
	rectVtx[0].vColor = Vec4(0.f, 0.f, 0.f, 0.f);

	rectVtx[1].vPos = Vec3(0.25f, 0.5f, 0.f);
	rectVtx[1].vUV = Vec2(1.f, 0.f);
	rectVtx[1].vColor = Vec4(0.f, 0.f, 0.f, 0.f);

	rectVtx[2].vPos = Vec3(0.25f, -0.5f, 0.f);
	rectVtx[2].vUV = Vec2(1.f, 1.f);
	rectVtx[2].vColor = Vec4(0.f, 0.f, 0.f, 0.f);

	rectVtx[3].vPos = Vec3(-0.5f, -0.5f, 0.f);
	rectVtx[3].vUV = Vec2(0.f, 1.f);
	rectVtx[3].vColor = Vec4(0.f, 0.f, 0.f, 0.f);

	UINT arrRectIdx[6] = { 0, 2, 3, 0, 1, 2 };
	pRectMesh = NEW AMesh;
	pRectMesh->Create(rectVtx, 4, arrRectIdx, 6);
	AddAsset(L"RectMesh", pRectMesh.Get());


	// ==================
	// RectMesh_LineStrip
	// ==================
	arrRectIdx[0] = 0; 	arrRectIdx[1] = 1;	arrRectIdx[2] = 2;	arrRectIdx[3] = 3; arrRectIdx[4] = 0;
	pRectMesh = NEW AMesh;
	pRectMesh->Create(rectVtx, 4, arrRectIdx, 5);
	AddAsset(L"RectMesh_LineStrip", pRectMesh.Get());


	// ===========================
	// 직사각형 정점 (Full), 배경활용
	// ===========================
	Ptr<AMesh> pFRectMesh = nullptr;
	Vtx FRectVtx[4] = {};

	FRectVtx[0].vPos = Vec3(-1.f, 1.f, 0.f);
	FRectVtx[0].vUV = Vec2(0.f, 0.f);
	FRectVtx[0].vColor = Vec4(0.f, 0.f, 0.f, 0.f);

	FRectVtx[1].vPos = Vec3(1.f, 1.f, 0.f);
	FRectVtx[1].vUV = Vec2(1.f, 0.f);
	FRectVtx[1].vColor = Vec4(0.f, 0.f, 0.f, 0.f);

	FRectVtx[2].vPos = Vec3(1.f, -1.f, 0.f);
	FRectVtx[2].vUV = Vec2(1.f, 1.f);
	FRectVtx[2].vColor = Vec4(0.f, 0.f, 0.f, 0.f);

	FRectVtx[3].vPos = Vec3(-1.f, -1.f, 0.f);
	FRectVtx[3].vUV = Vec2(0.f, 1.f);
	FRectVtx[3].vColor = Vec4(0.f, 0.f, 0.f, 0.f);

	pFRectMesh = NEW AMesh;
	pFRectMesh->Create(FRectVtx, 4, arrRectIdx, 6);
	AddAsset(L"FullRectMesh", pFRectMesh.Get());


	//=========
	// 원 만들기
	//=========
	/***************************************************
	* 원점을 기준으로 설정한 반지름까지의 하나의 정점을
	* 해장 정점을 360도 회전할때의 회전 경로에 수많은 정점을
	* 찍어 연결한다면, 원처럼 표현이 가능합니다.
	***************************************************/
	vector<Vtx> vecVtx;
	vector<UINT> vecIdx;

	// 중점
	Vtx v;
	v.vPos = Vec3(0.f, 0.f, 0.f);
	v.vUV = Vec2(0.5f, 0.5f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	vecVtx.push_back(v);

	float Theta = 0.f;
	float Radius = 0.5f;
	float Slice = 20.f;	// 정점의 개수 결정

	// 원의 테두리 정점 추가
	for (int i = 0; i < (int)Slice + 1; i++)
	{
		v.vPos = Vec3(Radius * cosf(Theta), Radius * sinf(Theta), 0.f);
		//v.vUV = Vec2(0.5f, 0.5f);
		v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
		vecVtx.push_back(v);

		Theta += XM_2PI / Slice;
	}

	// 인덱스
	for (int i = 0; i < (int)Slice + 1; i++)
	{
		vecIdx.push_back(0);
		vecIdx.push_back(i + 2);
		vecIdx.push_back(i + 1);
	}

	// 매쉬 생성
	Ptr<AMesh> pMesh = nullptr;
	pMesh = NEW AMesh;
	pMesh->Create(vecVtx.data(), vecVtx.size(), vecIdx.data(), vecIdx.size());
	AddAsset(L"CircleMesh", pMesh.Get());


	// ====================
	// CircleMesh_LineStrip
	// ====================
	vecIdx.clear();
	for (int i = 0; i < (int)Slice + 1; ++i)
	{
		vecIdx.push_back(i + 1);
	}

	pMesh = NEW AMesh;
	pMesh->Create(vecVtx.data(), vecVtx.size(), vecIdx.data(), vecIdx.size());
	AddAsset(L"CircleMesh_LineStrip", pMesh.Get());


	// =====================
	// ConeMesh (부채꼴, 채움)
	// =====================
	// 꼭짓점이 원점, +Y 방향으로 퍼지는 부채꼴 (반각 45도, 총 90도)
	vecVtx.clear();
	vecIdx.clear();

	const float fConeHalfAngle = XM_PIDIV4;   // 반각 45도
	const float fConeRadius    = 0.5f;
	const int   nConeSlice     = 20;

	// 꼭짓점 (tip)
	v.vPos   = Vec3(0.f, 0.f, 0.f);
	v.vUV    = Vec2(0.5f, 0.5f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	vecVtx.push_back(v);

	// 호(arc) 정점: XY 평면상에서 +Y 기준 ±halfAngle
	float fStartAngle = XM_PIDIV2 - fConeHalfAngle;
	float fEndAngle   = XM_PIDIV2 + fConeHalfAngle;
	for (int i = 0; i <= nConeSlice; ++i)
	{
		float theta  = fStartAngle + (fEndAngle - fStartAngle) / nConeSlice * i;
		v.vPos   = Vec3(fConeRadius * cosf(theta), fConeRadius * sinf(theta), 0.f);
		v.vUV    = Vec2(0.5f + cosf(theta), 0.5f - sinf(theta));
		v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
		vecVtx.push_back(v);
	}

	// 삼각형 팬 인덱스
	for (int i = 0; i < nConeSlice; ++i)
	{
		vecIdx.push_back(0);
		vecIdx.push_back(i + 1);
		vecIdx.push_back(i + 2);
	}

	pMesh = NEW AMesh;
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddAsset(L"ConeMesh", pMesh.Get());


	// ===================================
	// ConeMesh_LineStrip (윤곽선, 디버그용)
	// ===================================
	// 인덱스만 교체: tip → 오른쪽 호 → 왼쪽 호 → tip (닫힌 경로)
	vecIdx.clear();
	vecIdx.push_back(0);                     // tip
	for (int i = 1; i <= nConeSlice + 1; ++i)
		vecIdx.push_back(i);                 // 호 정점들
	vecIdx.push_back(0);                     // 다시 tip으로 닫기

	pMesh = NEW AMesh;
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddAsset(L"ConeMesh_LineStrip", pMesh.Get());

	// =================================
	// LargeBaseConeMesh (넓고 낮은 원뿔)
	// =================================
	vecVtx.clear();
	vecIdx.clear();

	constexpr float fLbcHalfAngle = LargeBaseConeConst::HalfAngle;
	constexpr float fLbcRadiusX   = LargeBaseConeConst::RadiusX;  // 반지름(밑면 반경)
	constexpr float fLbcRadiusY   = LargeBaseConeConst::RadiusY;  // 높이
	constexpr int   nLbcSlice     = LargeBaseConeConst::Slice;

	// 꼭짓점 (tip)
	v.vPos   = Vec3(0.f, 0.f, 0.f);
	v.vUV    = Vec2(0.5f, 0.5f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	vecVtx.push_back(v);

	// 호(arc) 정점
	float fLbcStartAngle = XM_PIDIV2 - fLbcHalfAngle;
	float fLbcEndAngle   = XM_PIDIV2 + fLbcHalfAngle;
	for (int i = 0; i <= nLbcSlice; ++i)
	{
		float theta  = fLbcStartAngle + (fLbcEndAngle - fLbcStartAngle) / nLbcSlice * i;
		v.vPos   = Vec3(fLbcRadiusX * cosf(theta), fLbcRadiusY * sinf(theta), 0.f);
		v.vUV    = Vec2(0.5f + cosf(theta), 0.5f - sinf(theta));
		v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
		vecVtx.push_back(v);
	}

	// 삼각형
	for (int i = 0; i < nLbcSlice; ++i)
	{
		vecIdx.push_back(0);
		vecIdx.push_back(i + 1);
		vecIdx.push_back(i + 2);
	}

	pMesh = NEW AMesh;
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddAsset(L"LargeBaseConeMesh", pMesh.Get());

	// ===================================
	// LargeBaseConeMesh_LineStrip
	// ===================================
	vecIdx.clear();
	vecIdx.push_back(0);
	for (int i = 1; i <= nLbcSlice + 1; ++i)
		vecIdx.push_back(i);
	vecIdx.push_back(0);

	pMesh = NEW AMesh;
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddAsset(L"LargeBaseConeMesh_LineStrip", pMesh.Get());
}

HRESULT AssetMgr::CreateEngineShader()
{
	// ===========================
	// 기본 2D 파이프라인(쉐이더) 생성
	// ===========================
	Ptr<AGraphicShader> pShader = nullptr;

	pShader = NEW AGraphicShader;
	if (FAILED(pShader->CreateVertexShader(L"Shader\\std2d.fx", "VS_Std2D"))
		|| FAILED(pShader->CreatePixelShader(L"Shader\\std2d.fx", "PS_Std2D")))
		return E_FAIL;
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetBSType(BS_TYPE::ALPHABLEND);
	pShader->AddShaderParam(SHADER_PARAM::VEC4, 0, L"TintColor");
	pShader->AddShaderParam(SHADER_PARAM::TEX, 0, L"OutColor");

	AddAsset(L"Std2DShader", pShader.Get());


	// =====================
	// 2D ONE_ONE 쉐이더 생성
	// =====================
	pShader = nullptr;
	pShader = NEW AGraphicShader;
	if (FAILED(pShader->CreateVertexShader(L"Shader\\std2d.fx", "VS_Std2D"))
		|| FAILED(pShader->CreatePixelShader(L"Shader\\std2d.fx", "PS_Std2D")))
		return E_FAIL;
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetBSType(BS_TYPE::ONE_ONE);
	pShader->AddShaderParam(SHADER_PARAM::VEC4, 0, L"TintColor");
	pShader->AddShaderParam(SHADER_PARAM::TEX, 0, L"OutColor");
	AddAsset(L"OOStd2DShader", pShader.Get());


	//=================
	// DbgRenderShader
	// 디버그 쉐이더 추가
	//=================
	pShader = nullptr;
	pShader = NEW AGraphicShader;
	if (FAILED(pShader->CreateVertexShader(L"Shader\\dbg.fx", "VS_Debug"))
		|| FAILED(pShader->CreatePixelShader(L"Shader\\dbg.fx", "PS_Debug")))
		return E_FAIL;
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);	// 토폴로지 설정
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);				// Debug Shader 타입
	pShader->SetBSType(BS_TYPE::DEFAULT);
	AddAsset(L"DbgShader", pShader.Get());


	/****************************************************
	* (26-02-26): 각각의 RenderComponent에서 자신이 사용할
	* 전용 쉐이더를 생성하는 부분을 Init에서 진행
	****************************************************/
	// ===============
	// BillboardShader
	// ===============
	pShader = NEW AGraphicShader;
	pShader->SetName(L"BillboardShader");
	if (FAILED(pShader->CreateVertexShader(L"Shader\\billboard.fx", "VS_Billboard"))
		|| FAILED(pShader->CreatePixelShader(L"Shader\\billboard.fx", "PS_Billboard")))
		return E_FAIL;
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->AddShaderParam(SHADER_PARAM::VEC4, 0, L"TintColor");
	pShader->AddShaderParam(SHADER_PARAM::TEX, 0, L"OutColor");
	AssetMgr::GetInst()->AddAsset(pShader->GetName(), pShader.Get());


	// ============
	// SpriteShader
	// ============
	pShader = NEW AGraphicShader;
	pShader->SetName(L"SpriteShader");
	if (FAILED(pShader->CreateVertexShader(L"Shader\\sprite.fx", "VS_Sprite"))
		|| FAILED(pShader->CreatePixelShader(L"Shader\\sprite.fx", "PS_Sprite")))
		return E_FAIL;
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->AddShaderParam(SHADER_PARAM::VEC4, 0, L"TintColor");
	pShader->AddShaderParam(SHADER_PARAM::TEX, 0, L"OutColor");
	AssetMgr::GetInst()->AddAsset(pShader->GetName(), pShader.Get());


	// ==============
	// FlipbookShader
	// ==============
	pShader = NEW AGraphicShader;
	pShader->SetName(L"FlipbookShader");
	if (FAILED(pShader->CreateVertexShader(L"Shader\\flipbook.fx", "VS_Flipbook"))
		|| FAILED(pShader->CreatePixelShader(L"Shader\\flipbook.fx", "PS_Flipbook")))
		return E_FAIL;
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->AddShaderParam(SHADER_PARAM::VEC4, 0, L"TintColor");
	pShader->AddShaderParam(SHADER_PARAM::TEX, 0, L"OutColor");
	AssetMgr::GetInst()->AddAsset(pShader->GetName(), pShader.Get());


	// ==========================
	// SavePoint (FlipbookShader) 
	// ==========================
	pShader = NEW AGraphicShader;
	pShader->SetName(L"SavePointShader");
	if (FAILED(pShader->CreateVertexShader(L"Shader\\savePoint.fx", "VS_Flipbook"))
		|| FAILED(pShader->CreatePixelShader(L"Shader\\savePoint.fx", "PS_Flipbook")))
		return E_FAIL;
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->AddShaderParam(SHADER_PARAM::VEC4, 0, L"TintColor");
	pShader->AddShaderParam(SHADER_PARAM::TEX, 0, L"OutColor");
	AssetMgr::GetInst()->AddAsset(pShader->GetName(), pShader.Get());


	// =============
	// TileMapShader
	// =============
	pShader = NEW AGraphicShader;
	pShader->SetName(L"TileShader");
	if (FAILED(pShader->CreateVertexShader(L"Shader\\tile.fx", "VS_Tile"))
		|| FAILED(pShader->CreatePixelShader(L"Shader\\tile.fx", "PS_Tile")))
		return E_FAIL;
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->AddShaderParam(SHADER_PARAM::VEC4, 0, L"TintColor");
	pShader->AddShaderParam(SHADER_PARAM::TEX, 0, L"OutColor");
	AssetMgr::GetInst()->AddAsset(pShader->GetName(), pShader.Get());

	return S_OK;
}

void AssetMgr::CreateEngineTexture()
{
	// Default Texture
	Load<ATexture>(L"cubeImg", L"Texture\\cube.png");
	Load<ATexture>(L"gridImg", L"Texture\\Grid.png");

	// Player Character Atlas
	Load<ATexture>(L"cat1_Atlas", L"Texture\\Player\\cat1_base.png");
	Load<ATexture>(L"cat2_Atlas", L"Texture\\Player\\cat2_base.png");
	Load<ATexture>(L"catAR_Atlas", L"Texture\\Player\\cat_ar_base.png");
	Load<ATexture>(L"energyFX_Atlas", L"Texture\\Player\\energy_effect_base.png");

	// Enemy Atlas
	Load<ATexture>(L"mon1_Atlas", L"Texture\\Enemy\\mon1_sprite_base.png");
	Load<ATexture>(L"mon2_Atlas", L"Texture\\Enemy\\mon2_sprite_base.png");
	Load<ATexture>(L"mon3_Atlas", L"Texture\\Enemy\\mon3_sprite_base.png");
	Load<ATexture>(L"mon4_Atlas", L"Texture\\Enemy\\mon4_sprite_base.png");

	Load<ATexture>(L"boss1_Atlas", L"Texture\\Enemy\\mechaneko-sheet1-r3-alpha.png");
	Load<ATexture>(L"boss2_Atlas", L"Texture\\Enemy\\mechaneko-sheet2-r1-alpha.png");

	Load<ATexture>(L"bomb_Atlas", L"Texture\\Enemy\\pirate_bomb_image_sequence.png");


	//============
	// BackGround
	//============
	Load<ATexture>(L"groundImg", L"Texture\\BackGround\\far-grounds.png");

	Load<ATexture>(L"fullSeaImg", L"Texture\\BackGround\\fullSea.png");
	Load<ATexture>(L"fullSkyImg", L"Texture\\BackGround\\fullSky.png");
	Load<ATexture>(L"fullCloudImg", L"Texture\\BackGround\\fullCloud.png");

	Load<ATexture>(L"tile_00_Img", L"Texture\\BackGround\\tile_00.png");
	Load<ATexture>(L"backWallsImg", L"Texture\\BackGround\\backWalls.png");
	Load<ATexture>(L"foregroundImg", L"Texture\\BackGround\\foreground.png");
	Load<ATexture>(L"underSeaImg", L"Texture\\BackGround\\underSea.png");
	
	Load<ATexture>(L"LargeGround_00_Img", L"Texture\\BackGround\\LargeGround_00.png");
	Load<ATexture>(L"LargeGround_01_Img", L"Texture\\BackGround\\LargeGround_01.png");
	Load<ATexture>(L"LargeStoneGround_00_Img", L"Texture\\BackGround\\LargeStoneGround_00.png");


	// TileMap Atlas
	Load<ATexture>(L"remapTile_Atlas", L"Texture\\BackGround\\remapTileSet.png");

	// UI
	Load<ATexture>(L"z_in_Img", L"Texture\\UI\\Key\\z_in.png");
	Load<ATexture>(L"x_in_Img", L"Texture\\UI\\Key\\x_in.png");
	Load<ATexture>(L"c_in_Img", L"Texture\\UI\\Key\\c_in.png");

	Load<ATexture>(L"z_out_Img", L"Texture\\UI\\Key\\z_out.png");
	Load<ATexture>(L"x_out_Img", L"Texture\\UI\\Key\\x_out.png");
	Load<ATexture>(L"c_out_Img", L"Texture\\UI\\Key\\c_out.png");

	Load<ATexture>(L"key_pressed_Img", L"Texture\\UI\\Key\\key_pressed_sheet.png");
	Load<ATexture>(L"key_released_Img", L"Texture\\UI\\Key\\key_released_sheet.png");
	Load<ATexture>(L"key_input_Img", L"Texture\\UI\\Key\\key_input_sheet.png");
	
	Load<ATexture>(L"move_key_Img", L"Texture\\UI\\Key\\move_key_sheet.png");
	Load<ATexture>(L"space_key_Img", L"Texture\\UI\\Key\\space_key_sheet.png");
	Load<ATexture>(L"arrow_key_Img", L"Texture\\UI\\Key\\arrow_key_sheet.png");

	Load<ATexture>(L"HP_Fill_Img", L"Texture\\UI\\HP\\hpFill.png");
	Load<ATexture>(L"HP_Background_Img", L"Texture\\UI\\HP\\hpBackground.png");


	// SavePoint
	Load<ATexture>(L"savePoint_Atlas", L"Texture\\Object\\meteor_spritesheet.png");
	Load<ATexture>(L"yellowSavePoint_Atlas", L"Texture\\Object\\yellow_meteor_spritesheet.png");

	// Title
	Load<ATexture>(L"title0Img", L"Texture\\UI\\Title\\title0.png");
	Load<ATexture>(L"title1Img", L"Texture\\UI\\Title\\title1.png");


	// MainMenu
	Load<ATexture>(L"startBtnImg", L"Texture\\UI\\MainMenu\\startBtn.png");
	Load<ATexture>(L"optionBtnImg", L"Texture\\UI\\MainMenu\\optionBtn.png");
	Load<ATexture>(L"creditBtnImg", L"Texture\\UI\\MainMenu\\creditBtn.png");
	
	// Ending / GameOver
	Load<ATexture>(L"mainMenuBtnImg", L"Texture\\UI\\MainMenu\\mainMenuBtn.png");

	// Portal
	Load<ATexture>(L"portalRings2Atlas", L"Texture\\Object\\portalRings2.png");

	// Logo
	Load<ATexture>(L"LogoAtlas", L"Texture\\Logo\\Logo_Flipbook.png");
	Load<ATexture>(L"FMODLogoAtlas", L"Texture\\Logo\\FMOD_Flipbook.png");
	Load<ATexture>(L"LogoImg", L"Texture\\Logo\\Logo.png");
	Load<ATexture>(L"FMODLogoImg", L"Texture\\Logo\\FMOD Logo White - Transparent Background.png");


}

void AssetMgr::CreateEngineMaterial()
{
	#pragma region Material 생성하는 방법
	// File로 저장이 완료된 Asset은 Load 함수 호출부만 남겨두기

	wstring FilePath = CONTENT_PATH;
	Ptr<AMaterial> pMtrl = nullptr;

	// =========
	// Std2DMtrl 
	// =========
	pMtrl = NEW AMaterial;
	pMtrl->SetName(L"Material\\Std2DMtrl.mtrl");
	pMtrl->SetShader(Find<AGraphicShader>(L"Std2DShader"));
	pMtrl->SetTexture(TEX_0, Find<ATexture>(L"cubeImg"));
	pMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_MASKED);
	AddAsset(pMtrl->GetName(), pMtrl.Get());
	pMtrl->Save(FilePath + pMtrl->GetKey());

	// =====
	// 디버그 
	// =====
	pMtrl = NEW AMaterial;
	pMtrl->SetName(L"Material\\DbgMtrl.mtrl");
	pMtrl->SetShader(Find<AGraphicShader>(L"DbgShader"));
	pMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_DEBUG);
	AddAsset(pMtrl->GetName(), pMtrl.Get());
	pMtrl->Save(FilePath + pMtrl->GetKey());

	Load<AMaterial>(L"Material\\Std2DMtrl.mtrl", L"Material\\Std2DMtrl.mtrl");
	Load<AMaterial>(L"Material\\DbgMtrl.mtrl", L"Material\\DbgMtrl.mtrl");
	#pragma endregion
}

void AssetMgr::CreateEngineSprite()
{
	/***********************************************
	* NOTE(26-02-27): Sprite를 파일로 저장하기 위해
	* 생성 시, 경로 + 확장자 명을 문자열로 추가합니다.
	* 해당 문자열은 생성된 Sprite의 Key값으로 설정 합니다.
	***********************************************/

}

void AssetMgr::CreateEnginePrefab(Ptr<GameObject> _Object)
{
	// 매개변수로 전달받은 _Object을 파일로 저장
	Ptr<APrefab> pPrefab = NEW APrefab;
	pPrefab->SetObject(_Object);
	
	wstring objectName = _Object->GetName();

	AddAsset((L"Prefab\\" + objectName + L".pref"), pPrefab.Get());
	wstring FilePath = CONTENT_PATH + pPrefab->GetKey();
	pPrefab->Save(FilePath);
}	

void AssetMgr::CreateEngineSprite(wstring _TextureName, Vec2 _Slice, int _StartLoop, int _EndLoop, wstring _SpriteName, int _OriIdx, int _EndOriIdx, int _Row, int _Col)
{
	#pragma region Atlas 이미지에서 Texture를 생성하는 방법
	Ptr<ATexture> pAtlas = FIND(ATexture, _TextureName);		// Texture 생성 시 설정한 이름 String을 입력합니다. 

	// Texture의 가로 세로 길이를 Getter로 얻어옵니다.
	float Width = pAtlas->GetWidth();
	float Height = pAtlas->GetHeight();

	// Texture 이미지를 얼마만큼의 길이로 자를 것인지 설정합니다.
	// UV 좌표 기준으로 값을 설정합니다. (자를 길이(높이, 너비)를 Vec2 값에 설정) 
	// 매개변수: _Slice 사용

	// 위에서 설정한 값과 반복문을 이용하여
	// Texture 이미지를 잘라 Sprite를 생성합니다. 
	Ptr<ASprite> pSprite = nullptr;
	
	// EndOriIdx가 디폴트로 0이 들어왔다면, 1번만 반복하게
	// EndOriIdx를 OriIdx + 1로 지정합니다.
	if (_EndOriIdx == 0)
		_EndOriIdx = _OriIdx + 1;

	int NameCount = _StartLoop;
	for (_OriIdx; _OriIdx < _EndOriIdx; ++_OriIdx)
	{
		for (int i = _StartLoop; i < _EndLoop; ++i)	// 몇 번 반복할지는, 몇 개의 이미지를 만들지에 맞게 결정합니다.
		{
			// Sprite의 이름을 설정합니다.
			// 만약 파일 형식으로 제작을 원한다면, 폴더 경로 + 확장자명을 적어둡니다.
			wchar_t Buff[50] = {};

			//==================================================================
			// NOTE(26-03-09): _Path 매개변수로 받았을 때, %d를 어떻게 합칠지 고민하기
			//==================================================================
			// _Path는 0%d 이전까지만 받고, 이후는 함수에서 +연산으로 붙이게 하기
			// std::to_wstring(i) 로 붙이는 방법이 있음
			// 10 이하이면 "0%d"로 네이밍 ex) 02.sprite
			wstring spritePath = {};

			// _Row, _Cow이 디폴트로 들어왔을 때 -> 00 2자리 네이밍 사용
			if (NameCount < 10 && _Row == 0 && _Col == 0) spritePath = L"Sprite\\" + _SpriteName + L"_" + to_wstring(0) + to_wstring(NameCount) + L".sprite";
			else spritePath = L"Sprite\\" + _SpriteName + L"_" + to_wstring(NameCount) + L".sprite";

			// _Row, _Cow이 정해졌다는 건, 100개 이상의 sprite 제작 -> 000 3자리 네이밍 사용
			if (NameCount < 10 && NameCount >= 0 && _Row != 0 && _Col != 0) spritePath = L"Sprite\\" + _SpriteName + L"_" + to_wstring(0) + to_wstring(0) + to_wstring(NameCount) + L".sprite";
			else if (NameCount < 100 && NameCount >= 10 && _Row != 0 && _Col != 0) spritePath = L"Sprite\\" + _SpriteName + L"_" + to_wstring(0) + to_wstring(NameCount) + L".sprite";
			else if (NameCount >= 100 && _Row != 0 && _Col != 0) spritePath = L"Sprite\\" + _SpriteName + L"_" + to_wstring(NameCount) + L".sprite";

			pSprite = NEW ASprite;
			pSprite->SetName(spritePath);
			pSprite->SetAtlas(pAtlas);

			// UV 기준 원점 설정
			// 원점 공식:
			// 파라미터로 받은 인덱스 * (_Slice.y / Height)
			//float Origin = _OriIdx * (_Slice.y / Height);
			//pSprite->SetLeftTopUV(Vec2((_Slice.x / Width) * (float)i, Origin));

			// [수정 제안]
			// i 값을 이용해 현재 몇 번째 열(col), 몇 번째 행(row)인지 계산
			int CountPerRow = (int)(Width / _Slice.x); // 한 줄에 들어가는 개수 (27개)

			// 현재 행(Row) 인덱스: 200 / 27 = 7
			int CurrentRow = i / CountPerRow;

			// 현재 열(Col) 인덱스: 200 % 27 = 11
			int CurrentCol = i % CountPerRow;

			// X 좌표: (1/27) * 11 = 0.4074... (정상 범위)
			float UV_X = (_Slice.x / Width) * (float)CurrentCol;

			// NOTE(26-03-18): 아래 공식으로 인해 Height 기준 어디서 부터 자를지가 모두 0으로 통일됨
			// 임시로 Origin 매개변수가 0이 아니면 (설정되었다면) UV_Y를 다른 공식으로 계산하기
			//
			// Y 좌표: (1/9) * 7 = 0.7777...
			float UV_Y = (_Slice.y / Height) * (float)CurrentRow;
		
			if (_OriIdx != 0)
			{
				float Origin = _OriIdx * (_Slice.y / Height);
				UV_Y = ((float)i, Origin);
			}

			pSprite->SetLeftTopUV(Vec2(UV_X, UV_Y));

			// x, y축 각각 '자를 해상도'에서 '아틀라스 해상도'를 나누기
			pSprite->SetSliceUV(_Slice / Vec2(Width, Height));

			// BackgorundUV는 자를 해상도(UV 기준 정규화) * 2로 설정 
			//==================================================================
			// NOTE(26-03-09): _Slice 매개변수를 UV 기준 정규화하는 기능 구현하기
			//==================================================================
			Vec2 bgUV = { ((_Slice.x / Width) * 2), ((_Slice.y / Height) * 2) };
			pSprite->SetBackgroundUV(bgUV);

			// 만들어진 Sprite를 등록합니다.
			// 
			// AddAsset => 런타임에만 등록
			AddAsset(pSprite->GetName(), pSprite.Get());
			// Save => 파일 형태로 등록
			pSprite->Save(CONTENT_PATH + pSprite->GetKey());	// 경로가 곧 Key 값

			// 반복이 끝나면 NameCount 1 증가
			++NameCount;
		}
	}

	#pragma endregion
}

void AssetMgr::CreateEngineMaterial(wstring _MtrlName, wstring _TextureName, wstring _ShaderName, RENDER_DOMAIN _Domain)
{
	#pragma region Material 생성하는 방법
	Ptr<AMaterial> pMtrl = nullptr;

	pMtrl = NEW AMaterial;
	_MtrlName = L"Material\\" + _MtrlName + L".mtrl";
	pMtrl->SetName(_MtrlName);
	pMtrl->SetShader(Find<AGraphicShader>(_ShaderName));

	// TextureName을 설정안했으면, Texture 설정 X
	if (_TextureName != L"")
		pMtrl->SetTexture(TEX_0, Find<ATexture>(_TextureName));

	pMtrl->SetDomain(_Domain);
	AddAsset(pMtrl->GetName(), pMtrl.Get());
	pMtrl->Save(CONTENT_PATH + pMtrl->GetKey());
	#pragma endregion
}

void AssetMgr::CreateEngineFlipbook(wstring _SpriteName, wstring _FlipbookName, int _Start, int _End, bool _IsReverse)
{
	#pragma region Flipbook 생성하는 방법
	// Flipbook 객체 생성 후, 이름을 설정하고
	// 반복문을 통해, 이어서 보여줄 sprite들을 가르키게 합니다.
	Ptr<AFlipbook> pFlipbook = nullptr;

	pFlipbook = NEW AFlipbook;
	_FlipbookName = L"Flipbook\\" + _FlipbookName + L".flip";
	pFlipbook->SetName(_FlipbookName);

	if (_IsReverse)
	{
		// 반복문 순서를 거꾸로 뒤집어서 Sprite 추가
		// 큰 값이 시작점이 되고, 작은 값이 조건이 됨
		for (int i = _Start; i >= _End; --i)
		{
			wstring spritePath = {};

			if (i < 10) spritePath = L"Sprite\\" + _SpriteName + L"_" + to_wstring(0) + to_wstring(i) + L".sprite";
			else spritePath = L"Sprite\\" + _SpriteName + L"_" + to_wstring(i) + L".sprite";

			pFlipbook->AddSprite(LOAD(ASprite, spritePath));
		}
	}
	else
	{
		for (int i = _Start; i < _End; ++i)
		{
			wstring spritePath = {};

			if (i < 10) spritePath = L"Sprite\\" + _SpriteName + L"_" + to_wstring(0) + to_wstring(i) + L".sprite";
			else spritePath = L"Sprite\\" + _SpriteName + L"_" + to_wstring(i) + L".sprite";

			pFlipbook->AddSprite(LOAD(ASprite, spritePath));
		}
	}

	AddAsset(pFlipbook->GetName(), pFlipbook.Get());
	// Save => 파일 형태로 등록 
	pFlipbook->Save(CONTENT_PATH + pFlipbook->GetKey());	// 경로가 곧 Key 값
	#pragma endregion
}

void AssetMgr::CreateEngineTileMap(vector<Ptr<ASprite>>& _vecSprite, wstring _TileMapName, wstring _AtlasName, UINT _Row, UINT _Col, Vec2 _TileSize)
{
	#pragma region TileMap 생성하는 방법
	// =======
	// TileMap
	// =======
	Ptr<ATileMap> pTileMap = nullptr;

	pTileMap = NEW ATileMap;
	_TileMapName = L"TileMap\\" + _TileMapName + L".tile";
	pTileMap->SetAtlas(FIND(ATexture, _AtlasName));
	pTileMap->SetName(_TileMapName);
	pTileMap->SetRowCol(_Row, _Col);
	pTileMap->SetTileSize(_TileSize);

	UINT Count = 0;
	// Col = 세로
	// Row = 가로
	// 반복문 작성 전에 꼭 확인하기
	for (UINT j = 0; j < _Row; ++j)
	{
		for (UINT i = 0; i < _Col; ++i)
		{
			// NOTE(26-03-20): SetSprite 인자를 잘못 전달하고 있는것 확인
			// SetSprite(row, col,... )
			pTileMap->SetSprite(j, i, _vecSprite[Count]);
			Count++;
		}
	}

	AddAsset(pTileMap->GetName(), pTileMap.Get());
	pTileMap->Save(CONTENT_PATH + pTileMap->GetKey());
	#pragma endregion
}

void AssetMgr::CreateEngineLevel(wstring _LevelName)
{
	// Level 에셋 생성은, 받은 이름으로 Level을 만듭니다.
	// 기본 오브젝트로 Directional Light와 MainCamera 만 제공합니다.
	// 필요한 GameObject나 Layer 이름은 Editor를 이용하여 추가합니다.
	
	Ptr<ALevel> pLevel = NEW ALevel;
	_LevelName = L"Level\\" + _LevelName + L".lv";
	pLevel->SetName(_LevelName);

	#pragma region 카메라, 광원 오브젝트
	Ptr<GameObject> pObject = nullptr;

	//======
	// 카메라
	//======
	pObject = NEW GameObject;
	pObject->SetName(L"MainCamera");

	pObject->AddComponent(NEW CTransform);
	pObject->AddComponent(NEW CCamera);
	pObject->AddComponent(NEW CCamMoveScript);

	pObject->Camera()->LayerCheckAll();
	// Layer 번호 31은 UI 레이어로 설정, editer 로 설정했기 때문에(개발용)
	//pObject->Camera()->LayerCheck(31);

	pObject->Transform()->SetRelativePos({ 0.f, 0.f, -100.f });
	pObject->Transform()->SetRelativeRot({ 0.f, 0.f, 0.f });

	pObject->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
	pObject->Camera()->SetFar(1000.f);
	pObject->Camera()->SetFOV(90.f);
	pObject->Camera()->SetOrthoScale(1.f);
	Vec2 vResolution = Device::GetInst()->GetRenderResolution();
	pObject->Camera()->SetAspectRatio(vResolution.x / vResolution.y);	// 종횡비(AspectRatio)
	pObject->Camera()->SetWidth(vResolution.x);							// height 멤버가 없는대신, width 정보만 설정하고 종횡비를 사용

	// 생성된 오브젝트 등록 (0번 레이어에, 위에 생성한 오브젝트 등록)
	pLevel->AddObject(0, pObject);


	//============
	// 광원 오브젝트
	//============
	pObject = NEW GameObject;
	pObject->SetName(L"Light");
	pObject->AddComponent(NEW CTransform);
	pObject->AddComponent(NEW CLight2D);

	pObject->Light2D()->SetLightType(LIGHT_TYPE::DIRECTIONAL);
	pObject->Light2D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
	pObject->Light2D()->SetAmbient(Vec3(0.15f, 0.15f, 0.15f));

	pObject->Transform()->SetRelativePos(Vec3(0.f, 0.f, 0.f));

	pLevel->AddObject(0, pObject);
	#pragma endregion
	
	// 레벨 변경점 체크
	pLevel->SetChanged();

	// 생성한 Level을 Asset으로 등록
	AddAsset(pLevel->GetName(), pLevel.Get());
	pLevel->Save(CONTENT_PATH + pLevel->GetKey());

	// TaskMgr에 요청하는 Level 변경은
	// 생성 단계에서 실행하지 않습니다. 
}
