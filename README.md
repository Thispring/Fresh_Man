# Fresh Man

<p align="center">
  <img src="ScreenShot/s1.jpg" width="85%" alt="Fresh Man 메인 화면">
</p>

DirectX 11과 C++로 2D 게임 엔진 구조와 ImGui 에디터를 구현하고, 그 위에서 2D 액션 플랫포머 게임을 제작한 개인 프로젝트입니다.

죽을 때마다 공격 키 배치가 무작위로 바뀌고, 직접 눌러 보기 전까지는 일부 공격 키를 알 수 없는 것이 이 게임의 핵심 규칙입니다.

---

## 프로젝트 소개

| 항목 | 내용 |
| --- | --- |
| 플랫폼 | Windows |
| 개발 언어 | C++ |
| 그래픽 API | DirectX 11 |
| 사용 라이브러리 | Dear ImGui, FMOD, DirectXTex, FW1FontWrapper |
| 개발 인원 | 1명 |
| 담당 | 엔진 · 에디터 · 게임 콘텐츠 프로그래밍 전반 |

---

## Gameplay

| 스크린샷 |
| :---: |
| <img src="ScreenShot/s2.jpg" width="49%" alt="게임 플레이 화면 1"> <img src="ScreenShot/s3.jpg" width="49%" alt="게임 플레이 화면 2"> |

[Fresh Man Gameplay Video](https://youtu.be/giiFv2sXMus?si=xqEO6lNW48JX_c4-)

---

## Download

- [Windows 빌드 다운로드 (itch.io)](https://thispring.itch.io/fresh-man)

---

## My Role

### 엔진 · 에디터

- GameObject / Component / Script 구조와 객체 복제(Clone)
- 렌더 도메인별 분류 렌더링, 게임 · UI · 에디터 카메라 분리
- 레이어 충돌 행렬과 도형별 2D 충돌 판정(OBB · 원 · 부채꼴)
- TaskMgr를 통한 오브젝트 생성 · 삭제 · 레벨 전환의 프레임 끝 일괄 처리
- Play / Pause / Stop 시 레벨 복제와 복원
- ImGui 에디터와 Sprite · Flipbook · TileMap · Material · Prefab · Level 제작 창
- 빌드 전 코드 자동 생성 도구(ComponentAuto)

### 게임 콘텐츠

- 플레이어 · 적 상태 기반 행동(FSM)
- 무작위 공격 키 배치와 키 공개 UI
- 경사면 이동, 벽 · 바닥 접촉 처리, 중력
- 시야 콜라이더를 이용한 적의 플레이어 감지
- 세이브 포인트와 리스폰, 제한 시간, 포털을 통한 레벨 종료
- 인트로 · 메인 메뉴 · 게임 오버 · 엔딩 레벨, UI, 사운드

---

# 주요 구현 내용

## GameObject · Component 구조와 객체 복제

`GameObject`가 컴포넌트를 타입별 배열로, 스크립트를 벡터로 보유하고 자식 오브젝트를 트리로 관리합니다.

- 복제 시 컴포넌트 · 스크립트 · 자식 오브젝트를 모두 `Clone()`으로 새로 생성해 원본과 데이터를 공유하지 않습니다.
- 모든 객체는 고유 ID와 참조 카운트를 가진 `Entity`를 상속하고, 에셋은 참조 카운트 스마트 포인터 `Ptr<T>`로 관리합니다.
- 컴포넌트 종류가 늘어날 때마다 직접 고쳐야 하던 `GetComponent` 매크로와 헤더 목록을 빌드 전 단계에서 자동으로 맞춰 주는 도구(ComponentAuto)를 만들어 사용했습니다.

**관련 코드** [GameObject.cpp](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/GameObject.cpp) · [Component.h](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/Component.h) · [Entity.h](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/Entity.h) · [Ptr.h](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/Ptr.h) · [ComponentAuto](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/ComponentAuto/ComponentAuto/main.cpp)

---

## 렌더링과 카메라

메인 루프는 시간 · 입력 갱신 → 레벨 갱신 → 렌더링 → 화면 출력 → TaskMgr 처리 순으로 진행됩니다.

- 레벨 상태에 따라 사용하는 카메라가 달라집니다. 플레이 중에는 메인 카메라와 UI 카메라, 에디터(Stop) 상태에서는 에디터 카메라, 인트로 연출(Cinematic) 중에는 메인 카메라를 사용합니다.
- 각 카메라는 레이어 마스크로 그릴 레이어를 고르고, 오브젝트를 머티리얼의 렌더 도메인(Opaque · Masked · Transparent · PostProcess)별로 분류한 뒤 순서대로 렌더링합니다.
- 2D 광원 정보는 StructuredBuffer로 셰이더에 전달하고, 타일맵은 타일 정보를 StructuredBuffer에 담아 한 번의 Draw로 그립니다.

**관련 코드** [Engine.cpp](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/Engine.cpp) · [RenderMgr.cpp](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/RenderMgr.cpp) · [CCamera.cpp](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/CCamera.cpp) · [StructuredBuffer.cpp](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/StructuredBuffer.cpp) · [CTileRender.cpp](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/CTileRender.cpp)

---

## 레이어 충돌 처리

레벨마다 레이어 간 충돌 여부를 비트 행렬로 저장하고(에디터에서 편집 가능), 충돌하도록 설정된 레이어 쌍만 검사합니다.

- 두 콜라이더의 ID를 64비트 키 하나로 묶어 이전 프레임의 충돌 여부를 기록하고, 이를 기준으로 `BeginOverlap` / `Overlap` / `EndOverlap`을 호출합니다.
- 충돌 판정은 콜라이더 모양에 따라 나뉩니다.
  - OBB vs OBB: 분리축 검사
  - 원 vs 원, OBB vs 원: 원 중심을 OBB 로컬 공간으로 옮겨 가장 가까운 점과의 거리 비교
  - 부채꼴(시야 등) vs 다른 도형: 대상 도형 위의 여러 점이 부채꼴 안에 있는지 검사
- 충돌 이벤트는 스크립트의 멤버 함수로 전달되어, 게임 콘텐츠에서 히트박스와 시야 판정에 사용합니다.

**관련 코드** [CollisionMgr.cpp](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/CollisionMgr.cpp) · [CCollider2D.cpp](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/CCollider2D.cpp) · [ALevel.cpp](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/ALevel.cpp)

---

## 프레임 끝 일괄 처리와 Play / Stop

- 오브젝트 생성 · 삭제, 레벨 전환, 레벨 상태 변경은 즉시 처리하지 않고 TaskMgr에 요청으로 쌓아 두었다가 프레임 마지막에 한 번에 반영합니다. 갱신 도중 오브젝트 목록이 바뀌어 생기는 문제를 피하기 위한 구조입니다.
- 삭제 요청된 오브젝트는 먼저 Dead 표시만 하고, 다음 프레임에 레이어와 부모의 목록에서 제거한 뒤 메모리를 해제합니다.
- 에디터에서 Play를 누르면 편집 중인 레벨을 복제해서 실행하고, Stop을 누르면 원본 레벨로 되돌아갑니다.

**관련 코드** [TaskMgr.cpp](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/TaskMgr.cpp) · [LevelMgr.cpp](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/LevelMgr.cpp) · [Layer.cpp](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/Layer.cpp)

---

## ImGui 에디터와 에셋 관리

- Inspector · Outliner · Content 창으로 오브젝트와 컴포넌트를 확인하고 수정합니다.
- 스크립트가 노출할 변수를 등록하면 Inspector에 자동으로 편집 UI가 생성됩니다.
- 제작 창
  - Sprite: 아틀라스를 지정한 크기로 잘라 스프라이트 생성
  - Flipbook: 스프라이트 범위를 지정해 애니메이션 생성
  - TileMap: 스프라이트를 칸에 끌어다 놓아 타일맵 구성
  - Material: 셰이더 · 텍스처 · 렌더 도메인 지정
  - Prefab / Level: 오브젝트와 레벨을 파일로 저장
- `AssetMgr`는 에셋을 타입별로 나눠 경로 Key로 관리하고, 레벨 · 오브젝트 · 컴포넌트는 각자 저장/불러오기 함수를 가진 바이너리 형식으로 저장됩니다.

**관련 코드** [EditorMgr.cpp](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/EditorMgr.cpp) · [EScriptUI.cpp](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/EScriptUI.cpp) · [AssetMgr.h](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/AssetMgr.h) · [TileMapMaker.cpp](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/TileMapMaker.cpp)

---

## 상태 기반 플레이어 · 적 행동

플레이어와 적의 행동을 상태 클래스로 나누고, StateManager가 상태 전환과 애니메이션 재생을 함께 처리합니다.

- **플레이어**: Idle · Walk · Jump · Punch · Kick · EnergyBlast · Death
  - 입력(Controller), 수치와 접촉 정보(Data), 상태 전환(StateManager), 애니메이션(Animator)을 별도 스크립트로 분리했습니다.
  - 근접 공격은 자식 오브젝트의 콜라이더를 공격 상태에서만 켜는 방식으로 판정합니다.
- **적**: Idle · Patrol · Chase · Attack · RangedAttack · Hit · Dead
  - 부모 `EnemyState`의 `Begin` / `Tick`이 중력 적용과 상태 경과 시간 계산을 공통으로 처리하고, 각 상태는 `OnBegin` / `OnTick`만 구현합니다.
  - 공격 상태에 들어갈 때 피격 이벤트를 구독해 맞으면 공격 처리를 즉시 멈추고, 상태를 나갈 때 구독을 해제해 다른 상태에 영향을 주지 않도록 했습니다.
  - 적 종류마다 같은 상태에 다른 애니메이션을 연결할 수 있도록, 상태와 플립북 인덱스를 함께 등록합니다.

**관련 코드** [CPlayerStateManager.cpp](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/Source/Scripts/CPlayerStateManager.cpp) · [CPlayerData.cpp](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/Source/Scripts/CPlayerData.cpp) · [EnemyState.h](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/Source/Content/EnemyState.h) · [EnemyState.cpp](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/Source/Content/EnemyState.cpp) · [EnemyAttackState.cpp](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/Source/Content/EnemyAttackState.cpp)

---

## 무작위 공격 키와 이동 처리

- 첫 생명은 Z · X · C로 시작하고, 리스폰할 때마다 키보드 한 줄에 붙은 3키 조합 20가지(QWE · ASD · ZXC 계열) 중 하나를 무작위로 골라 펀치 · 킥 · 에너지 블래스트에 배정합니다.
- 리스폰 후 펀치 · 에너지 블래스트 키는 UI에 `?`로 가려 두고, 플레이어가 처음 누르는 순간 공개합니다.
- 리스폰 시 투사체를 제거하고, static 목록으로 비활성화된 적까지 한 번에 초기화합니다.
- 바닥과 닿은 콜라이더의 법선으로 바닥 · 천장을 구분하고, 경사면에서는 바닥 법선의 접선 방향으로 이동합니다.
- 바닥 · 벽과 겹친 만큼 밀어내 콜라이더가 지형에 파고들지 않도록 처리했습니다.
- 적은 자식 오브젝트의 시야 콜라이더로 플레이어를 감지해 추격하거나 원거리 공격을 시작하고, 시야를 벗어나면 짧은 유예 시간 뒤 Idle로 돌아갑니다.

**관련 코드** [RandomMgr.cpp](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/RandomMgr.cpp) · [CPlayerController.cpp](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/Source/Scripts/CPlayerController.cpp) · [PlayerMoveState.cpp](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/Source/Content/PlayerMoveState.cpp) · [CEnemyEyes.cpp](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/Source/Scripts/CEnemyEyes.cpp)

---

## 사용 기술

| 기술 | 활용 |
| --- | --- |
| C++ | 엔진 · 에디터 · 게임 로직 |
| DirectX 11 | 렌더링, 상수 버퍼 · StructuredBuffer, 렌더 상태 관리 |
| WinAPI | 창 생성, 메시지 처리 |
| Dear ImGui | 에디터 UI (Docking) |
| FMOD | BGM · 효과음 재생, 볼륨 · 음소거 |
| DirectXTex | 텍스처 로드 |
| FW1FontWrapper | 텍스트 렌더링 |

---

> 이 프로젝트는 DirectX 11 기반 게임 엔진의 구조와 동작 방식을 학습하며 구현한 개인 프로젝트입니다.
