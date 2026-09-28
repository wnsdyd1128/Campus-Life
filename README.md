# Campus Life

`Campus Life`는 객체지향 프로그래밍 개념을 단계적으로 적용하며 완성하는 C++ 콘솔 프로젝트입니다.
학생의 돈과 체력, 수강 과목의 성적과 학습 진척도, 평일 행동 계획을 객체로 관리합니다.

## 현재 단계

현재 `week5` 브랜치는 로드맵의 **4단계 - 연산자 오버로딩과 주간 계획표**를 구현합니다.
강의자료 2–3, 12, 23쪽의 내용은 이번 단계에서 제외했습니다.

- `WeeklyPlan`에 월요일부터 금요일까지 5개의 행동 저장
- `WeeklyPlan::operator[]`의 읽기/쓰기 오버로드
- `Course::operator+=`를 통한 학습 점수 누적과 체이닝
- `Course`와 `Student`의 스트림 출력 연산자 `operator<<`
- `WeeklyPlan::clear()`를 통한 계획 초기화
- GoogleTest를 이용한 기존 소유권과 새 연산자 동작 검증

행동 클래스 계층, 계획의 자동 실행, 입력 오류 복구, 파일 저장은 이후 단계에서 다룹니다.

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
│   ├── Course.hpp
│   ├── Student.hpp
│   └── WeeklyPlan.hpp
├── src/
│   ├── Course.cpp
│   ├── Student.cpp
│   ├── WeeklyPlan.cpp
│   └── main.cpp
└── tests/
    ├── OperatorOverloadingTests.cpp
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

프로그램은 먼저 5일치 계획을 저장하고 `operator[]`로 다시 읽은 다음, 기존 예시 행동을 실행해
학생과 과목 상태를 `operator<<`로 출력합니다.

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
```

현재 등록된 테스트 10개는 다음 동작을 검증합니다.

1. `Student` 복사 생성자와 복사 대입 연산자의 깊은 복사 및 자기 대입
2. 사용자 지정 과목 배열 생성
3. `Course::operator+=`의 반환값과 성적 상승
4. `Course`와 `Student`의 스트림 출력
5. `WeeklyPlan::operator[]`의 쓰기 및 `const` 읽기
6. `WeeklyPlan::clear()`의 5일 계획 초기화

## 브랜치

| 브랜치 | 단계 | 주요 학습 내용 |
| --- | --- | --- |
| `week3` | 2단계 | `Student`와 `Course`의 구성 관계 |
| `week4` | 3단계 | 동적 메모리와 깊은 복사 (이동 의미론 제외) |
| `week5` | 4단계 | 연산자 오버로딩과 5일 주간 계획표 |

## 라이선스

이 프로젝트는 [MIT License](LICENSE)를 따릅니다.
