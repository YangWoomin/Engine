# Engine
* C/C++을 사용한 윈도우 서버 엔진
****
### 1. 작업 시작 날짜 : 2018-09-26 ~
****
### 2. IDE : Visual Studio 2015
****
### 3. 참고 서적
#### 3.1. 윈도우시스템 프로그램을 구현하는 기술 (한빛미디어, 저자 이호동)
> EchoServer, EchoClient 프로젝트는 위 서적의 예제 코드를 변형하여 구현 (저작권 등의 문제 시 제거하겠습니다.)
#### 3.2. 제프리 리처의 Windows VIA C/C++ (한빛미디어, 저자 제프리 리처, 크리스토프 나자르)
#### 3.3. 개발자가 반드시 정복해야 할 객체 지향과 디자인 패턴 (인투북스, 저자 최범균)
#### 3.4. 이펙티브 C++ 3rd edition (프로텍 미디어, 저자 스콧 마이어스)
****
### 4. 프로젝트 설명
#### 4.1. ThreadpoolEngine
*  윈도우 비스타부터 지원하는 윈도우 스레드풀을 사용하기 위한 간단한 엔진을 만들었습니다. 스레드풀을 사용한 이유는 비동기 입출력을 위한 IOCP의 한계로 스레드의 개수를 프로그래머가 직접 조절해야 하는 어려움이 있기 때문입니다. 일반적으로 게임 서버마다 IOCP의 스레드들이 하는 작업은 천차만별이고 실시간으로 최적의 스레드 개수만을 IOCP 풀에 풀어 놓기는 거의 불가능한 일입니다. 이 어려운 일을 운영체제가 스레드 개수를 조절하여 항상 최적의 스레드 개수를 유지시켜주는 풀을 제공하는데 그것이 스레드풀입니다.
 스레드풀을 사용하기 위한 스레드풀 객체는 4가지가 있습니다. 간단하게 비동기로 콜백 함수를 즉시 실행하는 TP_WORK, 동기화 객체가 시그널 상태가 되었을 때 콜백 함수를 실행하는 TP_WAIT, 타이머 기능과 조합되어 주기적으로 콜백 함수를 실행하는 TP_TIMER, 비동기 입출력 완료 통지가 발생되면 콜백 함수를 실행하는 TP_IO가 있습니다. 이것들을 통해서 프로그래머가 원하는 작업을 특정 스레드풀의 스레드로 콜백 함수 실행하게 합니다. 보다 효율적으로 스레드풀을 위 네가지 객체들로 사용하기 위해 ThreadpoolEngine을 만들었으며 논리적으로 그룹화해주는 기능을 제공합니다.
 스레드풀 객체들을 논리적인 그룹으로 관리하기 위해 CThreadpoolGroupManager 싱글톤 클래스가 있습니다. 스레드풀 그룹별 스레드풀 객체들을 관리합니다. 스레드풀 객체가 생성되고 초기화될 때 스레드풀 그룹에 추가하며 소멸되기 전에 마무리 작업으로 스레드풀 그룹에서 제거합니다. 생성된 스레드풀 그룹은 이 인스턴스의 생명 주기와 동일하기 때문에 프로그램 종료시에 모두 해제됩니다.
 다음으로 스레드풀 객체들을 추상화한 CThreadpoolCallbackObject 클래스 템플릿이 있습니다. 위 클래스를 통해 스레드풀 특정 그룹에 추가되고 제거됩니다. 프로그래머가 호출하려는 유저 콜백 함수와 관련 데이터(ICallbackData)를 래핑하고 있으며 이 데이터(인터페이스)를 통해 유저 콜백 함수가 실행되는 것을 도와 줍니다. 특히 유저 콜백 함수 내부에서 해당 콜백 객체의 콜백 함수 실행 완료를 대기하는 일은 데드락을 유발할 수 있으므로 스레드 별로 콜백 객체 인스턴스를 TLS에 임시 저장하여 데드락을 회피하도록 합니다.
 CThreadpoolCallbackObject 클래스 템플릿을 상속하여 구현한 4가지 클래스로 CThreadpoolCallbackXXX가 있습니다. 이 클래스들은 TP_XX 콜백 객체들을 구체적으로 사용하기 위한 각각의 전용 클래스들이며 CThreadpoolCallbackObjectWrapper 템플릿 클래스의 자식 클래스들인 CThreadpoolCallbackXXXWrapper로 래핑되고 이 래핑 클래스들은 dll로 export되어서 dll을 사용하는 프로그램에게 콜백 객체들의 필요한 기능을 제공합니다.
#### 4.2. EngineBase
*  게임 서버가 사용할 시스템 의존적인 기능을 지원하는 엔진 기본 클래스입니다. 현재 완성된 것이 아니고 계속 개발중에 있는 상황입니다. 
 객체풀을 사용하기 위한 CObjectPool 클래스 템플릿이 있습니다. 객체풀을 사용하는 이유는 많이 사용하는 객체의 new/delete 호출에 의한 메모리 작업 오버헤드를 줄이기 위함이며 객체들의 메모리 영역을 미리 할당하여 그 오버헤드를 줄일 수 있습니다. 이 클래스는 싱글톤 클래스 템플릿을 상속하여 구현하였습니다. 템플릿 인자로 객체풀에서 사용할 객체 타입을 넣어야 하며 이 객체들은 별도의 힙 영역에서 관리됩니다. 객체별 헤더가 추가되어 객체들을 관리하기 용이하도록 구현되었으며 AcquireAllocatedObject 멤버 함수와 ReturnAllocatedObject 멤버 함수를 통하여 객체의 획득/반환 작업이 가능합니다. 멀티 스레드 접근에도 기본 new/delete 못지 않은 성능을 내기 위해 유저 모드 동기화 수단인 Interlocked 시리즈 API를 활용하였습니다. 실제 간단한 테스트를 해본 결과 랜덤의 500,000,000 횟수의 new/delete보다 근소한 차이로 빠르다는 것을 확인했습니다. 실제 라이브 상황에서 fragmentation이 보다 심할 것으로 예상한다면 더 나은 퍼포먼스를 기대할 수 있겠습니다.
 
  현재 EngineBase 프로젝트 작업을 계속 진행 중에 있으며 STL의 allocator의 메모리 할당 부분을 위의 객체풀을 적용하여 확장된 컨테이너(map, set 등)를 제작할 예정입니다.
****
