## 📝 README: 세 점을 지나는 정원 그리기 프로그램

### 🌟 개요

| 카테고리 | 설명 |
| ----------- | ----------- |
| 개발 언어 | C, C++ |
| IDE | Visual Studio 2022, GitHub Desktop, VS Code, Git Bash |
| 프레임워크 | MFC, Win32 |
| 버전 관리 | GitHub |
| 운영체제 | Windows 10 |

이 프로젝트는 MFC Dialog 기반으로 작성되었으며, 사용자가 세 번의 클릭한 세 지점을 모두 지나는 **정원**을 그리는 프로그램입니다. 

제시된 부가 기능은 다음과 같습니다.
1. 클릭 지점을 클릭하여 드래그했을 때 정원을 다시 그립니다.
2. 초기화 기능을 통해 모든 내용을 삭제하고 처음 상태로 돌아갑니다.
3. 랜덤 이동 기능을 통해 3개의 클릭 지점 모두를 랜덤한 위치로 이동합니다.
4. 랜덤 이동 기능을 자동으로 반복합니다.

❌`그림을 그리는 방식은 특정 GDI/GDI+ API를 사용하지 않고 구현합니다.`

💡 [구현에 참고할 영상 목록](https://youtube.com/playlist?list=PLlIX4lkC1JdMx-vfK8I-J3-L-GL7TbMf9&si=VqwjifaHOXGIT1dm)

💡 [과제 안내 링크](https://doc.clickup.com/25596452/p/h/rd4h4-262778/9faf6d2e66ff9e7)

-----

✔ 클릭 지점 정보로 원을 구성하는 방법

1. 원의 방정식을 이용하는 방법
2. 수학적으로 정리된 방법 (헤론의 공식)
3. 원의 작도법을 이용하는 방법

✔ CImage 객체로 비트맵을 구성할 때 DIB 특징에 의해 y 좌표 계산에 보정 필요
<a href="https://ibb.co/qLddT1ss"><img src="https://i.ibb.co/wZRR56WW/CImage-Create.png" alt="CImage-Create" border="0"></a> <sup>[1](#footnote_1)</sup>
1. positive nHeight 경우 bottom-up DIB : origins 포인터 이동하여 상향식 혹은 하향식 처리
2. negative nHeight 경우 top-down DIB : origins 포인터 하향식 처리


포인터 이동 연산이 들어가면 자칫 복잡하게 보일 수 있을 것으로 판단되어 두 번째 방법을 선택하였습니다.
```C++
m_image.Create(nWidth, -nHeight, nBpp);
```

✔ 랜덤 이동 작업

❎ 일정 시간 간격으로 반복된 작업을 위한 타이머

타이머의 경우 이미지의 크기가 일정 기준 이상으로 커지는 경우 사용자가 불편을 느낄 정도의 지연이 발생합니다. 테스트 결과 단일 스레드에서 대략 nMB 부터 500ms 가 넘어갑니다. 따라서 요구된 조건처럼 멀티 스레딩이 필요합니다.

✅ 메인 UI 프리징을 막기 위한 멀티 스레드

스레드를 사용하며 고려할 사항은 다음과 같습니다.

1. 수행해야할 작업
2. 각 작업의 시작시점
3. 각 작업의 종료시점
4. 각 작업의 상호연관성

수행하는 작업은 크게 난수 생성, 원 계산, 정원 마킹 및 그리기가 있습니다. 메인 스레드가 담당하는 그리기를 제외한 모든 작업들에서 ms 단위의 지연이 발생하기 때문에 프리징을 일으킬 수 있는 요소로 판단하고 작업스레드가 수행하도록 설계했습니다.

아직 이 프로그램이 여러 개가 실행되거나 다른 프로그램과의 연동을 염두하지는 않기 때문에 프로세스 수준의 동기화 기술인 뮤텍스가 아닌 스레드 수준에서의 처리를 명확히 합니다.

스레드를 사용하며 UI 설계에 대한 이해 (예. 메시지 시스템<sup>[1](#footnote_2)</sup>) 및 몇몇 함수의 특징에 대한 학습도 개인적으로 유익했습니다.

가령, 난수 생성에서 유효한 값을 얻기 위해 사용하는 srand() 함수의 경우 스레드 단위로 작업이 이루어집니다. 따라서 여러 작업 스레드가 난수를 생성한다고 하더라도 시드를 바꾸는 작업은 1회면 충분합니다.

초당 2회 반복하는 작업에서 필요한 지연은 해당 스레드에서 sleep 관련 함수를 통해 구현합니다.

총 10회 작업을 위해 스레드를 10번 생성할 수도 있겠지만, 프로그래밍에서 동일한 작업을 진행할 객체를 중복해서 할당해제할 필요는 없습니다. 따라서 할당해제 코드를 반복문 밖으로 정리합니다.

- - - 

▶ 제출 외 개인적인 진행
- No Commit : 타이머 + 콜백함수 구조로 자동 랜덤 기능 구현
- Nov 28, 2025 (72c3332) : 640x480 크기 이미지 소요 시간 측정. 작업 스레드 5개 (정원 4개, 나머지 계산 1개)
- Dec 03, 2025 (549a68b) : 640x480 크기 이미지 소요 시간 측정. 작업 스레드 1개 (대화상자 메인 UI 작업 외 모든 기능), Mb_Thread 라이브러리 작업
- Dec 06, 2025 (xxxxxxx) : 추가 기능 7. ListBox 추가 및 스레드 예외처리
- - -
추가해볼만한 기능

1. 클릭 및 드래그 지점을 원이 그려질 곳으로 보정
2. 클릭 및 드래그 지점에 대한 가이드 UI를 제공
3. 클릭 지점에 커서 위치할 때 원 색상 변경
<br></br>

4. 진행 정도를 나타낼 Progress 제공
5. 영역 크기를 동적으로 변경
6. 깜박임을 잡기 위한 Memory DC 처리
<br></br>

7. 작업 상황에 대한 메시지를 확인할 ListBox
8. 그리기에 사용되는 색상을 동적으로 변경
9. 일부 소스코드를 공개하지 않기 위한 라이브러리 작업 ✅

~~서버 - 클라이언트 모델로 클릭 지점을 서로 다른 프로그램에서 그리기~~
- - -

***특정 소스코드는 보호 목적으로 공개하지 않습니다. (출처 : [MyThread 네이버 카페](https://cafe.naver.com/MyThread))***

<a name="footnote_1">[1] [CImage::Create 공식 문서](https://learn.microsoft.com/en-us/cpp/atl-mfc-shared/reference/cimage-class?view=msvc-170&devlangs=cpp&f1url=%3FappId%3DDev17IDEF1%26l%3DKO-KR%26k%3Dk(ATLIMAGE%2FATL%3A%3ACImage%3A%3ACreate)%3Bk(ATL%3A%3ACImage%3A%3ACreate)%3Bk(Create)%3Bk(DevLang-C%2B%2B)%3Bk(TargetOS-Windows)%26rd%3Dtrue#create)

<a name="footnote_2">[2]</a> [Application-Defined Messages](https://learn.microsoft.com/en-us/windows/win32/winmsg/about-messages-and-message-queues#application-defined-messages) 도움말 페이지를 통해 데스크톱 앱에서 개발자가 구성할 메시지 값의 유효 범위에 대해 알 수 있습니다.

<a href="https://ibb.co/BV0CnHv2"><img src="https://i.ibb.co/bgcKbjY5/Messages-WM-USER.png" alt="Messages-WM-USER" border="0"></a>

