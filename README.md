# ⚔️ Text RPG

C++로 만든 콘솔 텍스트 RPG 게임입니다.

## 바로 실행하기

> **rpg.exe** 와 **bgm.mp3** 가 같은 폴더에 있어야 합니다.

1. 이 저장소를 ZIP으로 다운로드 후 압축 해제
2. `rpg.exe` 실행

※ Windows 전용

## 게임 내용

- **직업 선택**: 전사 / 마법사 / 힐러
- **4개 스테이지**: 고블린 → 늑대 → 악어 → 드래곤
- **스킬 시스템**: 직업별 고유 스킬
- **마을 시스템**: 스테이지 사이 회복 / 저장
- **배경음악 설정**: 메인메뉴 → 설정에서 볼륨 조절

## 직접 빌드하기

MinGW(g++) 필요:

```bash
g++ -std=c++17 -o rpg main.cpp Entity.cpp Character.cpp Monster.cpp BattleManager.cpp Item.cpp Jobs.cpp SaveManager.cpp -lwinmm
```

## 파일 구조

```
rpg.exe          실행파일
bgm.mp3          배경음악 (필수)
main.cpp         메인 / 메뉴 / 오디오
Jobs.cpp/h       전사, 마법사, 힐러 클래스
Monster.cpp/h    몬스터 클래스
BattleManager    전투 시스템
SaveManager      저장/불러오기
Entity, Character, Item  기반 클래스
```
