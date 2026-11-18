#pragma once
#include "assets.h"
#include "PathMgr.h"
#include "ALevel.h"

// AssetMgr은 에셋이 추가 생성 될 때마다
// 알맞은 Type을 map 자료구조에 추가하여 관리합니다.
class AssetMgr
	: public Singleton<AssetMgr>
{
	SINGLE(AssetMgr)
private:
	map<wstring, Ptr<Asset>>	m_mapAsset[(UINT)ASSET_TYPE::END];
	bool						m_Changed;	// Asset 변경점 확인 (생성, 삭제...)

	// 초기 Loading 관련 멤버
	ASSET_TYPE			m_tempType;			// 어떤 Asset Type인지
	vector<wstring>		m_vecFileName;		// file로 부터 얻어온 문자열을 저장
	int					m_fileCount;		// 특정 폴더에 몇개의 file이 있는지를 저장

public:
	//=========
	// 멤버 함수
	//=========
	// Asset을 초기화 (현재는 정점으로 만든 Mesh)
	HRESULT Init();

	// map을 이용해 Asset을 찾는 함수
	void AddAsset(const wstring& _Key, Ptr<Asset> _Asset);
	Ptr<Asset> FindAsset(ASSET_TYPE _Type, const wstring& _Key);
	
	void CreateEngineMesh();
	HRESULT CreateEngineShader();
	void CreateEngineTexture();
	void CreateEngineMaterial();
	void CreateEngineSprite();
	void CreateEnginePrefab(Ptr<GameObject> _Object);

	// 매개변수를 받아, 외부에서 사용할 수 있도록 오버로딩
	void CreateEngineSprite(wstring _TextureName, Vec2 _Slice, int _StartLoop, int _EndLoop, wstring _SpriteName, int _OriIdx = 0, int _EndOriIdx = 0, int _Row = 0, int _Col = 0);
	void CreateEngineMaterial(wstring _MtrlName, wstring _TextureName, wstring _ShaderName, RENDER_DOMAIN _Domain);
	void CreateEngineFlipbook(wstring _SpriteName, wstring _FlipbookName, int _Start, int _End, bool _IsReverse);
	void CreateEngineTileMap(vector<Ptr<ASprite>>& _vecSprite, wstring _TileMapName, wstring _AtlasName, UINT _Row, UINT _Col, Vec2 _TileSize);
	void CreateEngineLevel(wstring _LevelName);

	// 코드로 Asset을 제작하는 부분을 모아서 정리 (백업용)
	void CreateAssetByCode();
	
	// 템플릿 버전
	template<typename T>
	Ptr<T> Find(const wstring& _Key);

	template<typename T>
	Ptr<T> Load(const wstring& _Key, const wstring& _RelativePath);
	
	/*************************************************
	* Level 시작 전에 1회 호출, 
	* 경로에 있는 각 Asset의 확장자를 문자열로 확인하고
	* 알맞은 Load를 호출합니다.
	* 
	* RunTime 도중 저장한 에셋은 다시 Load할 필요가
	* 없기에, 실행 시 1회만 호출합니다.
	*************************************************/
	void LoadContent();
	
	void GetAssetNames(ASSET_TYPE _type, vector<wstring>& _vec);
	bool IsChanged()
	{
		// 원본 멤버인 m_Changed를 다시 false로 바꿔서 재사용
		bool Changed = m_Changed;
		m_Changed = false;
		return Changed;
	}
};

/***************************************************************************
* [26-01-29, C++ 추가문법]
* 
* constexpr:
* const 보다 더 엄격한 상수 키워드
* 
* 변수 템플릿:
* EX) 
* template<typename T>
* bool g_IsTrue = true;
* {
*	g_IsTrue<int> = 100;
* }
* 와 같이 변수를 템플릿화 시킬 수 있다.
* 단, 지역에 선언된 변수는 <int>로 지정된 bool 타입의 변수
* 
* 템플릿 특수화:
* T1, T2 가 다르게 설정되면, 초기값을 
* T1, T2 타입이 같은 타입으로 설정되면 초기값을 200 으로 설정한 전역변수를 만든다.
* 마치 조건문과 비슷하게 사용이 가능하다.
* 
* if constexpr:
* if 구문 옆에 constexpr 를 붙이면, if 조건안에 반드시 컴파일 타임에 확신할 수 있는 
* 참 or 거짓이 들어와야만 한다. 컴파일 시간안에 확실할 수 있다면, 해당 조건구문만 
* 남기고 다른 코드는 전부 삭제(컴파일 최적화) 가능
* 
* 템플릿 특수화와 if constexpr의 특성을 같이 사용하면, 컴파일 타임에 모든게 결정되어,
* 런타임에서 불필요한 조건 검사를 줄일 수 있다.
***************************************************************************/
template<typename T>
ASSET_TYPE GetAssetType()
{
	if constexpr (std::is_same_v<T, AMesh>)
		return ASSET_TYPE::MESH;
	else if constexpr (std::is_same_v<T, AMaterial>)
		return ASSET_TYPE::MATERIAL;
	else if constexpr (std::is_same_v<T, AGraphicShader>)
		return ASSET_TYPE::GRAPHICSHADER;
	else if constexpr (std::is_same_v<T, ATexture>)
		return ASSET_TYPE::TEXTURE;
	else if constexpr (std::is_same_v<T, ASprite>)
		return ASSET_TYPE::SPRITE;
	else if constexpr (std::is_same_v<T, AFlipbook>)
		return ASSET_TYPE::FLIPBOOK;
	else if constexpr (std::is_same_v<T, ATileMap>)
		return ASSET_TYPE::TILEMAP;
	else if constexpr (std::is_same_v<T, ALevel>)
		return ASSET_TYPE::LEVEL;
	else if constexpr (std::is_same_v<T, APrefab>)
		return ASSET_TYPE::PREFAB;
	else if constexpr (std::is_same_v<T, ASound>)
		return ASSET_TYPE::SOUND;

	return ASSET_TYPE::END;
}

template<typename T>
Ptr<T> AssetMgr::Find(const wstring& _Key)
{
	ASSET_TYPE Type = GetAssetType<T>();

	map<wstring, Ptr<Asset>>::iterator iter = m_mapAsset[(UINT)Type].find(_Key);

	if (iter == m_mapAsset[(UINT)Type].end())
		return nullptr;

	return (T*)iter->second.Get();
}

template<typename T>
Ptr<T> AssetMgr::Load(const wstring& _Key, const wstring& _RelativePath)
{
	// 동일한 Key로 먼저 등록된 에셋이 있는지 확인
	Ptr<T> pAsset = Find<T>(_Key);
	
	// 동일한 Key로 먼저 등록된 에셋이 있으면 반환
	if (pAsset != nullptr)
		return pAsset;

	// 에셋 객체 생성
	pAsset = NEW T;

	// 입력된 경로로부터 에셋 로딩작업 진행	
	pAsset->Load(CONTENT_PATH + _RelativePath);

	// T 타입에 해당하는 실제 AssetType 확인
	ASSET_TYPE type = GetAssetType<T>();

	// 맵에 에셋등록
	m_mapAsset[(UINT)type].insert(make_pair(_Key, pAsset.Get()));

	// 에셋이 자신이 매니저에 등록될때 사용된 Key 와, 
	// 자신이 어떤 경로에 있는 파일로부터 로딩된 에셋인지 스스로 알 수 있도록 해줌
	pAsset->SetKey(_Key);
	pAsset->SetRelativePath(_RelativePath);

	// Load가 성공했으면 true로 변경
	m_Changed = true;
	
	return pAsset;
}

template<typename T>
Ptr<T> LoadAssetRef(FILE* _File)
{
	// 파일에 저장된 Asset정보가 null인지 여부를 확인, bool값으로 저장하여
	// false이면 가리키는 Asset이 없고, true면 가리키는 Asset이 있는 상태
	bool IsNull = false;
	fread(&IsNull, sizeof(bool), 1, _File);

	// 파일에 저장된 Key, 경로 확인
	if (IsNull)
	{
		wstring Key = LoadWString(_File);
		wstring RelativePath = LoadWString(_File);
		return AssetMgr::GetInst()->Load<T>(Key, RelativePath);
	}

	return nullptr;
}


#define FIND(Type, Key) AssetMgr::GetInst()->Find<Type>(Key)
#define LOAD(Type, AssetPath) AssetMgr::GetInst()->Load<Type>(AssetPath, AssetPath)
