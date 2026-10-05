# Fresh Man

<p align="center">
  <img src="ScreenShot/s1.jpg" width="85%" alt="Fresh Man 메인 화면">
</p>

DirectX 11 · C++ 엔진 위에 ImGui 에디터 제작 도구를 더하고, 이를 이용해 만든 2D 액션 플랫포머입니다.

게임잼 테마 'Fresh'에 맞춰, 죽을 때마다 공격 키 배치가 바뀌고 직접 눌러 보기 전까지 일부 키를 알 수 없는 것이 이 게임의 핵심 규칙입니다.

> 엔진의 기반 구조(GameObject · Component, 렌더링, 충돌 처리, TaskMgr 등)는 강의 코드를 따라 구현하며 학습한 부분입니다.
> 아래 **주요 구현 내용**은 그 위에서 직접 설계 · 구현한 기능입니다.

---

## 프로젝트 소개

| 항목 | 내용 |
| --- | --- |
| 플랫폼 | Windows |
| 개발 언어 | C++ |
| 그래픽 API | DirectX 11 |
| 사용 라이브러리 | Dear ImGui, FMOD, DirectXTex, FW1FontWrapper |
| 개발 인원 | 1명 |

---

## Gameplay

| 스크린샷 |
| :---: |
| <img src="ScreenShot/s2.jpg" width="49%" alt="게임 플레이 화면 1"> <img src="ScreenShot/s3.jpg" width="49%" alt="게임 플레이 화면 2"> |

[Fresh Man Gameplay Video](https://youtu.be/giiFv2sXMus?si=xqEO6lNW48JX_c4-)

---

## Download

- [Download for Windows (itch.io)](https://thispring.itch.io/fresh-man)

---

# 주요 구현 내용

## 지연 처리 Task 활용

**문제** 에디터를 실행 중인 런타임에도 GameObject를 만들어 레벨에 등록해야 했습니다. 생성 요청은 Task로 다음 프레임에 처리되는데, 요청한 프레임 안에서 제작 창 설정을 초기화하면 다음 프레임의 Task가 이미 해제된 객체를 참조해 크래시가 발생했습니다.

**해결** 요청 시점의 객체를 깊은 복사해 Task로 전달했습니다.

- Task에는 오브젝트의 원시 포인터만 담기므로, 제작 창이 원본 `Ptr`을 초기화하면 참조 카운트가 0이 되어 다음 프레임 전에 해제된 것이 원인이었습니다.
- GameObject 제작 창에서 OK를 누르면 설정 중인 오브젝트를 복사 생성자로 깊은 복사하고, 그 복사본을 Task로 넘긴 뒤 설정을 초기화합니다.
- 복사본은 제작 창의 멤버 `Ptr`(`m_CloneObject`)이 다음 생성 요청 전까지 보유해, Task가 처리될 때까지 수명을 보장합니다.
- 오브젝트 활성 · 비활성 변경도 `SET_ACTIVE_OBJECT` Task를 추가해 다음 프레임에 반영하도록 했습니다. (세이브 포인트가 Tick 도중 자기 자신을 끌 때 사용)

**관련 코드** [GameObjectMaker.cpp](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/GameObjectMaker.cpp) · [LevelMgr.cpp](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/LevelMgr.cpp) · [TaskMgr.cpp](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/TaskMgr.cpp)

---

## 상태 기반 플레이어 · 적 FSM

**문제** 플레이어와 적의 행동이 Flipbook 애니메이션과 어긋나지 않아야 했고, 적의 종류와 패턴이 늘어나도 콘텐츠를 빠르게 추가 · 수정할 수 있어야 했습니다.

**해결** 플레이어와 적이 한 번에 하나의 상태만 갖는 FSM을 Begin · Tick · FinalTick 기반으로 구현했습니다.

- Begin에서 진입 시 처리(공격 콜라이더 켜기 등), Tick에서 매 프레임 동작, FinalTick에서 상태를 나가기 전 정리(콜라이더 끄기 등)를 수행합니다.
- StateManager가 상태와 Flipbook 번호를 함께 등록하고, 전환 시 이전 상태 FinalTick → 새 상태 Begin → 해당 애니메이션 재생 순으로 처리합니다.
- 플레이어: Idle · Walk · Jump · Punch · Kick · EnergyBlast · Death
- 적: Idle · Patrol · Chase · Attack · RangedAttack · Hit · Dead
- Animator가 공격 애니메이션이 끝난 시점을 확인해, 키를 누르고 있으면 같은 공격을 다시 시작하고 아니면 Idle로 전환합니다.
- `EnemyState`는 중력 · 상태 경과 시간 계산을 부모의 Begin · Tick에서 공통 처리하고, 각 상태는 `OnBegin` · `OnTick`만 구현합니다.
- 적 종류마다 같은 상태에 다른 Flipbook 번호를 연결해, 상태 코드는 공유하고 애니메이션만 다르게 사용합니다.

**관련 코드** [CPlayerStateManager.cpp](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/Source/Scripts/CPlayerStateManager.cpp) · [CPlayerAnimator.cpp](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/Source/Scripts/CPlayerAnimator.cpp) · [EnemyState.h](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/Source/Content/EnemyState.h) · [contentEnum.h](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/contentEnum.h)

---

## 무작위 공격 키 배치

**문제** 게임잼 테마 'Fresh'에 맞춰, 죽을 때마다 기본 공격 키 3개가 바뀌어 매번 새로운 키를 찾아야 하는 기믹을 기획했습니다. 다만 세 키가 서로 떨어져 있으면 다시 손에 익히기 어렵고, 키가 바뀐다는 사실을 플레이어가 스스로 알아챌 수 있어야 했습니다.

**해결** 연속된 키 조합에서 무작위로 고르고, 일부 키만 공개했습니다.

- 키보드 한 줄에서 붙어 있는 3키 조합 20가지(QWE · ASD · ZXC 계열)를 미리 정의하고, 리스폰할 때마다 그중 하나를 무작위로 골라 펀치 · 킥 · 에너지 블래스트에 배정합니다. (첫 생명은 Z · X · C)
- 3개 중 2번째(킥) 키만 처음부터 UI에 보여 주고, 나머지는 `?`로 가려 두었다가 맞는 키를 누르면 이후 계속 표시합니다.
- 키 아이콘은 KEY 값으로 아틀라스 UV를 계산해 표시하고, 누르는 동안 눌린 모양으로 바꿉니다.
- 메인 메뉴도 같은 함수로 5초마다 메뉴 선택 키를 바꿔, 게임 시작 전부터 기믹을 경험하도록 구성했습니다.

**관련 코드** [RandomMgr.cpp](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/RandomMgr.cpp) · [contentFunc.cpp](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/contentFunc.cpp) · [CPlayerStateManager.cpp](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/Source/Scripts/CPlayerStateManager.cpp) · [CUIOverlayController.cpp](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/Source/Scripts/CUIOverlayController.cpp)

---

## 에디터 콘텐츠 제작 도구

**문제** 자체 엔진으로 콘텐츠를 만들면서 모든 요소를 코드로 작성하면, 추가할 요소가 늘어날수록 작업량과 유지보수 부담이 커질 것이라 판단했습니다. 자주 쓰는 에셋을 런타임에서 바로 만들고 편집할 수 있어야 했습니다.

**해결** AssetMgr가 에셋을 만들 때 필요한 값을 ImGui 제작 창에서 입력하도록 구현 · 디자인했습니다.

- Sprite: 아틀라스를 지정한 크기로 잘라 생성 / Flipbook: 스프라이트 범위로 애니메이션 생성
- TileMap: Content 창의 스프라이트를 칸에 끌어다 놓아 구성
- GameObject: 컴포넌트와 스크립트를 골라 조합한 뒤, 레벨 · 레이어를 지정해 생성
- Content 창의 에셋을 입력 칸에 끌어다 놓으면 경로를 자동으로 채우고, 저장 전 확인 팝업으로 입력 값을 한 번 더 확인
- 첫 디자인의 가독성이 낮다는 피드백을 받아 버튼 · 제목의 색상과 배치를 바꿔 개선

**관련 코드** [SpriteMaker.cpp](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/SpriteMaker.cpp) · [TileMapMaker.cpp](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/TileMapMaker.cpp) · [GameObjectMaker.cpp](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/GameObjectMaker.cpp) · [AssetMgr_Func.cpp](https://github.com/Thispring/Fresh_Man/blob/main/DirectX11_Engine/GameClient/AssetMgr_Func.cpp)

---

## 사용 기술

| 기술 | 활용 |
| --- | --- |
| C++ | 엔진 학습 · 에디터 도구 · 게임 로직 |
| DirectX 11 | 렌더링 |
| Dear ImGui | 에디터 UI |
| FMOD | BGM · 효과음 |
| DirectXTex | 텍스처 로드 |
| FW1FontWrapper | 텍스트 렌더링 |

---

> 이 프로젝트는 DirectX 11 기반 게임 엔진의 구조와 동작 방식을 학습하며 구현한 개인 프로젝트입니다.
