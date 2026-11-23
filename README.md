# Campus Life

`Campus Life`는 객체지향 프로그래밍 개념을 단계적으로 적용하며 완성하는 C++ 콘솔 프로젝트입니다.
학생의 돈과 체력, 수강 과목의 성적과 학습 진척도, 평일 행동 계획을 객체로 관리합니다.

## 현재 단계

현재 `week13` 브랜치는 로드맵의 **11단계 - 게임이 오류로 종료되지 않게 하기**를 구현합니다.
Exception Handling 강의의 `try`·`catch`·`throw`와 사용자 정의 예외 클래스를 파일 처리에 적용합니다.

- 매주 완료 후 `data/play.log`에 결과를 텍스트로 추가
- `data/save.dat`의 고정 길이 바이너리 레코드 3개에 각각 저장하고 불러오기
- `seekg`와 `seekp`로 선택한 슬롯만 접근
- Week, Money, Energy, 학생 이름, 세 과목의 이름·등급·진척도와 Burnout 상태 복원
- 파일 오류, 잘못된 슬롯 번호, 손상된 저장 데이터를 `GameException` 계층으로 구분

## 오류 처리

- 에너지나 돈이 부족하면 해당 행동이 실패하고, 다음 행동을 계속 실행합니다.
- 빈 슬롯이나 저장 파일이 없는 경우 불러오기는 `false`를 반환합니다. 메뉴의 잘못된 입력은 다시 받습니다.
- 저장 API에 1~3 범위 밖의 번호를 전달하면 `InvalidSaveSlotException`이 발생합니다.
  `CorruptedSaveDataException`은 잘못된 파일 크기나 레코드를, `FileOpenException`은 파일 접근 오류를 알립니다.
- 불러오기 중 오류가 나면 원인을 출력하고 메인 메뉴로 돌아갑니다. 저장 오류가 나면 계획 입력을 이어가고,
  로그를 쓸 수 없으면 경고를 출력한 뒤 주간 결과를 표시합니다.

`StudyActivity` **is-a** `Activity`이고, `Student` **has-a** `Course`라는 두 객체 관계를 비교할 수 있습니다.
`Activity` 자체는 객체를 생성할 수 없는 추상 클래스입니다. `StudyActivity` 등의 객체 주소를
기본 클래스 포인터로 저장하는 upcasting과 가상 함수 호출로 실제 객체의 동작을 선택합니다.
새 게임은 총 16주를 진행합니다.

## 저장과 로그

- 주간 행동을 선택하는 화면에서 `5`를 입력하면 현재 주가 시작된 상태를 저장합니다. 입력 중인 미확정
  요일 계획은 저장하지 않으며, 저장 후 같은 요일의 행동 선택을 계속합니다.
- 저장 화면과 메인 메뉴의 `2`에서 슬롯 1~3을 고릅니다. `0`은 취소합니다. 사용 중인 슬롯에 저장하면
  그 슬롯을 덮어쓰며, 복원 후에는 해당 주의 계획을 다시 입력합니다.
- `save.dat`는 빈 레코드까지 포함해 동일한 크기의 레코드 3개로 구성됩니다. 슬롯 번호로 오프셋을 계산해
  해당 레코드만 읽거나 씁니다. 손상된 슬롯은 불러오지 않고 현재 게임 상태를 유지합니다. 파일 크기가
  올바르지 않으면 기존 파일을 덮어쓰지 않습니다. 쓰기 실패 시 선택한 슬롯은 불완전해질 수 있습니다.
- 학생 이름은 최대 63바이트, 과목 이름은 각각 최대 31바이트입니다(종료 문자 제외). 길이가 넘는 이름은
  잘라내지 않고 저장을 거부합니다. 바이너리 파일은 현재 빌드의 정수 표현과 레코드 배치를 사용하므로
  서로 다른 플랫폼이나 컴파일러 사이의 호환성은 보장하지 않습니다.
- 이전 단계의 `data/save.txt`는 자동 변환하거나 삭제하지 않습니다. 새 버전은 `save.dat`만 읽습니다.
- 완료한 주는 주말 회복 이후 `data/play.log`에 한 줄로 기록됩니다. 로그 쓰기에 실패해도 경고 후 게임은 계속됩니다.
- `data/`는 프로그램을 실행한 현재 작업 디렉터리 기준이며 Git에서 제외됩니다. 저장은 수동이므로
  게임을 종료하기 전에 필요한 상태를 저장하세요.

## 행동 객체와 계획의 수명

`CampusLifeGame`은 학생, 네 행동 객체와 계획표를 값 멤버로 보유합니다. `planStudy()` 등의 함수는
게임이 보유한 행동의 주소를 계획표에 저장하며, 이때 학생 상태는 바뀌지 않습니다.
계획표의 포인터는 **비소유 참조**이므로 `WeeklyPlan`은 행동을 삭제하지 않습니다.
게임 내부 행동을 참조하는 포인터의 수명을 보장하기 위해 게임 객체의 복사·이동은 허용하지 않습니다.

공부 대상은 행동 객체가 아닌 `PlannedAction::courseIndex`에 저장합니다. 따라서 하나의
`StudyActivity`를 월요일과 화요일에 공유하면서 서로 다른 과목을 선택할 수 있습니다.
`execute(Student&, bool burnout, int courseIndex)`의 `const`는 행동 객체를 변경하지 않는다는 뜻이며,
전달받은 학생의 자원과 과목은 변경할 수 있습니다.

`simulateWeek()`는 요일별 성공 여부와 자원·과목의 실행 전후 상태, 주말 회복량을 `WeekResult`로 반환합니다.
미선택 요일(`nullptr`)은 건너뛰며 반환값은 `false`입니다. 콘솔 UI는 임시로 선택한 계획을 저장해 두었다가
확인된 이후에만 게임 계획을 갱신합니다. 실행된 계획은 다음 주에 새로운 선택으로 덮어씁니다.
Burnout은 다음 실행 주에만 적용되고, 그 주에도 체력이 0이 되면 다시 예약됩니다. 회복으로 예약이 취소되거나
공부 배율이 1/4로 중첩되지 않습니다. C/B/A의 공부 점수는 Burnout 동안 각각 20/12/10입니다.

## 요구 사항

- CMake 3.20 이상
- C++20을 지원하는 컴파일러
  - Windows: Visual Studio 2022 이상 권장
  - Linux/macOS: 최신 GCC 또는 Clang
- GoogleTest를 처음 내려받을 때 사용할 인터넷 연결

GoogleTest v1.18.0은 CMake `FetchContent`로 자동으로 내려받으므로 별도로 설치할 필요가 없습니다.

## 프로젝트 구조

```text
Campus-Life/
├── CMakeLists.txt
├── include/
│   ├── Activity.hpp
│   ├── CampusLifeGame.hpp
│   ├── ConsoleUI.hpp
│   ├── LogManager.hpp
│   ├── PersistenceResult.hpp
│   ├── GameException.hpp
│   ├── SaveManager.hpp
│   ├── ChickenActivity.hpp
│   ├── Course.hpp
│   ├── PartTimeJobActivity.hpp
│   ├── SleepActivity.hpp
│   ├── Student.hpp
│   ├── StudyActivity.hpp
│   └── WeeklyPlan.hpp
├── src/
│   ├── CampusLifeGame.cpp
│   ├── ConsoleUI.cpp
│   ├── LogManager.cpp
│   ├── SaveManager.cpp
│   ├── ChickenActivity.cpp
│   ├── Course.cpp
│   ├── PartTimeJobActivity.cpp
│   ├── SleepActivity.cpp
│   ├── Student.cpp
│   ├── StudyActivity.cpp
│   ├── WeeklyPlan.cpp
│   └── main.cpp
└── tests/
    ├── ActivityInheritanceTests.cpp
    ├── ConsoleUITests.cpp
    ├── OperatorOverloadingTests.cpp
    ├── PersistenceTests.cpp
    ├── PolymorphismTests.cpp
    └── StudentOwnershipTests.cpp
```

## 빌드

프로젝트 루트에서 다음 명령을 실행합니다.

```powershell
cmake -S . -B out/cmake-build
cmake --build out/cmake-build --config Debug
```

Visual Studio의 CMake 생성기는 Debug와 Release를 함께 생성하는 다중 구성 방식이므로 빌드할 때
`--config Debug` 또는 `--config Release`를 지정해야 합니다.

기존 Visual Studio 프로젝트를 사용하려면 `Campus-Life.slnx`를 열어 빌드할 수도 있습니다. GoogleTest
테스트 프로젝트는 CMake로 생성되므로 테스트는 위 CMake 빌드 방식을 사용합니다.

## 실행

Windows Debug 빌드:

```powershell
.\out\cmake-build\Debug\CampusLife.exe
```

단일 구성 생성기인 Ninja, Makefiles 등을 사용했다면 실행 파일은 보통 다음 위치에 생성됩니다.

```text
out/cmake-build/CampusLife
```

메인 메뉴에서 `1`을 입력해 새 게임을 시작합니다. 각 요일에 `1` 공부, `2` 아르바이트,
`3` 잠자기, `4` 치킨 먹기를 선택하고, 공부에는 과목 번호를 추가로 입력합니다.
계획을 확인할 때 `Y`를 입력하면 실행하고 `N`을 입력하면 월요일부터 다시 선택합니다.
계획 입력 중 `5`를 입력하면 현재 상태를 저장하고, `0`을 입력하면 메인 메뉴로 돌아갑니다.
메인 메뉴에서 `2`를 입력하면 슬롯을 선택해 저장 상태를 불러옵니다. 메뉴와 결과 문구는 영어로 출력됩니다.

## 테스트

먼저 Debug 구성으로 모든 실행 파일을 빌드합니다.

```powershell
cmake --build out/cmake-build --config Debug
```

CTest로 전체 테스트를 실행합니다.

```powershell
ctest --test-dir out/cmake-build -C Debug --output-on-failure
```

`-C Debug`를 생략하면 Visual Studio 다중 구성 빌드에서는 테스트 실행 파일의 구성을 결정할 수 없어
`Test not available without configuration` 오류가 발생합니다.

GoogleTest 실행 파일을 직접 실행할 수도 있습니다.

```powershell
.\out\cmake-build\Debug\StudentOwnershipTests.exe
.\out\cmake-build\Debug\OperatorOverloadingTests.exe
.\out\cmake-build\Debug\ActivityInheritanceTests.exe
.\out\cmake-build\Debug\PolymorphismTests.exe
.\out\cmake-build\Debug\ConsoleUITests.exe
.\out\cmake-build\Debug\PersistenceTests.exe
```

기존 테스트와 콘솔 UI 테스트는 다음 동작을 검증합니다.

1. `Student` 복사 생성자와 복사 대입 연산자의 깊은 복사 및 자기 대입
2. 사용자 지정 과목 배열 생성
3. `Course::operator+=`의 반환값과 성적 상승
4. `Course`와 `Student`의 스트림 출력
5. `WeeklyPlan::operator[]`의 쓰기 및 `const` 읽기
6. `WeeklyPlan::clear()`의 5일 계획 초기화
7. 네 행동 클래스의 상속 관계, 공통 이름, 행동별 자원 변화와 실패 시 상태 유지
8. 추상/구체 클래스 조건, 기본 클래스 포인터·참조를 통한 동적 호출과 가상 소멸자
9. 같은 공부 객체를 공유하는 계획과 월~금 실행 순서, 실패 후 계속 실행
10. 주말 회복 상한, Burnout 예약·해제·재예약과 정수 나눗셈에 의한 학습량
11. 모든 과목이 A+가 되어도 남은 요일과 주말 처리를 끝까지 수행
12. 잘못된 입력·EOF·계획 재입력, 일별 출력과 16주 종료·장학금 화면
13. 세 저장 슬롯 독립성, 상태 및 Burnout 복원, 손상 파일 거부 및 실패 시 상태 보존
14. 주간 로그 추가, 예외 종류별 처리와 오류 후 콘솔 메뉴 복귀

## 브랜치

| 브랜치 | 단계 | 주요 학습 내용 |
| --- | --- | --- |
| `week3` | 2단계 | `Student`와 `Course`의 구성 관계 |
| `week4` | 3단계 | 동적 메모리와 깊은 복사 (이동 의미론 제외) |
| `week5` | 4단계 | 연산자 오버로딩과 5일 주간 계획표 |
| `week6` | 5단계 | `Activity` 상속 계층과 네 행동 클래스 |
| `week09` | 06·07단계 통합 | 추상 클래스, 가상 함수와 다형적 주간 실행 |
| `week10` | 08단계 | 콘솔 입력 복구와 주간·학기 게임 흐름 |
| `week11` | 09단계 | 순차 파일 저장·불러오기와 주간 로그 |
| `week12` | 10단계 | 바이너리 랜덤 접근과 세 저장 슬롯 |
| `week13` | 11단계 | 파일 및 저장 데이터 오류의 예외 처리 |

## 라이선스

이 프로젝트는 [MIT License](LICENSE)를 따릅니다.
