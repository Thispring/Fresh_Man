#include "pch.h"
#include "AssetMgr.h"
#include "PathMgr.h"

HRESULT AssetMgr::Init()
{
	CreateEngineMesh();

	const HRESULT result = CreateEngineShader();
	if (FAILED(result))
		return result;

	CreateEngineTexture();

	//CreateEngineMaterial();

	//CreateEngineSprite();

	//CreateEnginePrefab();

	return S_OK;
}

void AssetMgr::CreateAssetByCode()
{
	#pragma region Mesh 생성하는 방법

	#pragma endregion

	#pragma region Shader 생성하는 방법

	#pragma endregion

	#pragma region Texture 생성하는 방법

	#pragma endregion

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

	#pragma region Atlas 이미지에서 Texture를 생성하는 방법
	Ptr<ATexture> pAtlas = FIND(ATexture, L"TestImage");		// Texture 생성 시 설정한 이름 String을 입력합니다. 
	
	// Texture의 가로 세로 길이를 Getter로 얻어옵니다.
	float Width = pAtlas->GetWidth();
	float Height = pAtlas->GetHeight();

	// Texture 이미지를 얼마만큼의 길이로 자를 것인지 설정합니다.
	// UV 좌표 기준으로 값을 설정합니다. (자를 길이(높이, 너비)를 Vec2 값에 설정) 
	Vec2 SlicePixel = Vec2(1.f, 1.f);

	// 위에서 설정한 값과 반복문을 이용하여
	// Texture 이미지를 잘라 Sprite를 생성합니다. 
	Ptr<ASprite> pSprite = nullptr;
	for (int i = 0; i < 10; ++i)	// 몇 번 반복할지는, 몇 개의 이미지를 만들지에 맞게 결정합니다.
	{
		// Sprite의 이름을 설정합니다.
		// 만약 파일 형식으로 제작을 원한다면, 폴더 경로 + 확장자명을 적어둡니다.
		wchar_t Buff[50] = {};
		swprintf_s(Buff, L"Sprite\\Test_Spirte_%d.sprite", i);		

		pSprite = NEW ASprite;
		pSprite->SetName(Buff);
		pSprite->SetAtlas(pAtlas);

		// 원점 설정
		pSprite->SetLeftTopUV(Vec2((SlicePixel.x / Width) * (float)i, 0.f));

		// x, y축 각각 '자를 해상도'에서 '아틀라스 해상도'를 나누기
		pSprite->SetSliceUV(SlicePixel / Vec2(Width, Height));
		
		// 만들어진 Sprite를 등록합니다.
		// 
		// AddAsset => 런타임에만 등록
		AddAsset(pSprite->GetName(), pSprite.Get());
		// Save => 파일 형태로 등록
		//pSprite->Save(CONTENT_PATH + pSprite->GetName());
		pSprite->Save(CONTENT_PATH + pSprite->GetKey());	// 경로가 곧 Key 값
	}
	#pragma endregion

	#pragma region Flipbook 생성하는 방법
	// Flipbook 객체 생성 후, 이름을 설정하고
	// 반복문을 통해, 이어서 보여줄 sprite들을 가리키게 합니다.
	Ptr<AFlipbook> pFlipbook = nullptr;

	pFlipbook = NEW AFlipbook;
	pFlipbook->SetName(L"Flipbook\\Test_Fb.flip");

	// 해당 Flipbook에 몇개의 Sprite를 연속으로 재생할것인지를
	// 반복 횟수로 정합니다. ex)8장이면 8번 반복
	for (int i = 0; i < 8; ++i)
	{
		wchar_t Buff[50] = {};
		swprintf_s(Buff, L"Sprite\\Test_%d.sprite", i);
		pFlipbook->AddSprite(LOAD(ASprite, Buff));
	}
	AddAsset(pFlipbook->GetName(), pFlipbook.Get());
	// Save => 파일 형태로 등록
	pFlipbook->Save(CONTENT_PATH + pFlipbook->GetKey());
	#pragma endregion

	#pragma region TileMap 생성하는 방법
	// =======
	// TileMap
	// =======
	Ptr<ATileMap> pTileMap = nullptr;

	pTileMap = NEW ATileMap;
	pTileMap->SetName(L"TileMap\\TestTileMap.tile");
	pTileMap->SetRowCol(2, 2);
	pTileMap->SetTileSize(Vec2(16.f, 16.f));
	pTileMap->SetAtlas(FIND(ATexture, L"TileAtlas"));

	int Count = 0;
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 2; ++j, ++Count)
		{
			wchar_t Buff[50] = {};
			swprintf_s(Buff, L"Sprite\\TileSprite_%d.sprite", Count);
			pTileMap->SetSprite(i, j, FIND(ASprite, Buff));
		}
	}

	AddAsset(pTileMap->GetName(), pTileMap.Get());
	pTileMap->Save(pTileMap->GetKey());
	#pragma endregion
}
