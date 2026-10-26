# Campus Life

`Campus Life`는 객체지향 프로그래밍 개념을 단계적으로 적용하며 완성하는 C++ 콘솔 프로젝트입니다.
학생의 돈과 체력, 수강 과목의 성적과 학습 진척도, 평일 행동 계획을 객체로 관리합니다.

## 현재 단계

현재 `week09` 브랜치는 로드맵의 **06·07단계 - 다형성 기반 주간 실행과 행동 시스템 완성**을 통합합니다.
7주차 `7. chap12_polymorphism1.pdf`와 9주차 `9. chap12_polymorphism2.pdf`를 기준으로 하며,
두 번째 강의자료의 15–17쪽 내용은 제외했습니다.

- `Activity::getName()`과 `execute()`를 순수 가상 함수로 선언하고 가상 소멸자 제공
- 네 구체 행동 클래스가 공통 인터페이스를 `override`
- `WeeklyPlan`이 `const Activity*`와 공부 대상 과목 인덱스를 저장
- `CampusLifeGame::simulateWeek()`가 공통 인터페이스로 월~금 계획을 순서대로 실행
- 실패한 행동은 상태를 바꾸지 않고 그날을 소비하며 다음 날 실행을 계속 진행
- 주말 체력 +35와 체력 상한 100 적용
- 성공한 행동으로 체력이 0이 되면 다음 주 Burnout 예약, 해당 주 공부 점수는 절반 적용

`StudyActivity` **is-a** `Activity`이고, `Student` **has-a** `Course`라는 두 객체 관계를 비교할 수 있습니다.
`Activity` 자체는 객체를 생성할 수 없는 추상 클래스입니다. `StudyActivity` 등의 객체 주소를
기본 클래스 포인터로 저장하는 upcasting과 가상 함수 호출로 실제 객체의 동작을 선택합니다.
16주 전체 게임 루프, 콘솔 입력 UI, 파일 저장은 이후 단계에서 다룹니다.

## 행동 객체와 계획의 수명

`CampusLifeGame`은 학생, 네 행동 객체와 계획표를 값 멤버로 보유합니다. `planStudy()` 등의 함수는
게임이 보유한 행동의 주소를 계획표에 저장하며, 이때 학생 상태는 바뀌지 않습니다.
계획표의 포인터는 **비소유 참조**이므로 `WeeklyPlan`은 행동을 삭제하지 않습니다.
게임 내부 행동을 참조하는 포인터의 수명을 보장하기 위해 게임 객체의 복사·이동은 허용하지 않습니다.

공부 대상은 행동 객체가 아닌 `PlannedAction::courseIndex`에 저장합니다. 따라서 하나의
`StudyActivity`를 월요일과 화요일에 공유하면서 서로 다른 과목을 선택할 수 있습니다.
`execute(Student&, bool burnout, int courseIndex)`의 `const`는 행동 객체를 변경하지 않는다는 뜻이며,
전달받은 학생의 자원과 과목은 변경할 수 있습니다.

`simulateWeek()`는 요일별 성공 여부를 `WeekResult`로 반환합니다. 미선택 요일(`nullptr`)은 건너뛰며
반환값은 `false`입니다. 계획은 실행 후에도 유지되어 다음 주에 재사용하거나 요일별로 덮어쓸 수 있습니다.
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
│   ├── ChickenActivity.hpp
│   ├── Course.hpp
│   ├── PartTimeJobActivity.hpp
│   ├── SleepActivity.hpp
│   ├── Student.hpp
│   ├── StudyActivity.hpp
│   └── WeeklyPlan.hpp
├── src/
│   ├── CampusLifeGame.cpp
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
    ├── OperatorOverloadingTests.cpp
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

프로그램은 공부(0번 과목) → 공부(1번 과목) → 치킨 → 아르바이트 → 잠자기의 5일 계획을 먼저 저장한 뒤
주간 실행기를 호출합니다. 주말 회복까지 적용한 예시 결과는 Money 65,000, Energy 85,
두 과목의 학습 진척도 각각 40입니다. 요일별 성공 여부와 이번 주·다음 주 Burnout 상태도 출력합니다.

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
```

현재 등록된 테스트 31개는 다음 동작을 검증합니다.

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

## 브랜치

| 브랜치 | 단계 | 주요 학습 내용 |
| --- | --- | --- |
| `week3` | 2단계 | `Student`와 `Course`의 구성 관계 |
| `week4` | 3단계 | 동적 메모리와 깊은 복사 (이동 의미론 제외) |
| `week5` | 4단계 | 연산자 오버로딩과 5일 주간 계획표 |
| `week6` | 5단계 | `Activity` 상속 계층과 네 행동 클래스 |
| `week09` | 06·07단계 통합 | 추상 클래스, 가상 함수와 다형적 주간 실행 |

## 라이선스

이 프로젝트는 [MIT License](LICENSE)를 따릅니다.
