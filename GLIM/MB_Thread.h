#ifndef _MB_THREAD_H_
#define _MB_THREAD_H_

// 이 클래스는 (주)그림, SW 개발직군 신입 과제를 진행하는 중 스레드 관련 코드를 공개하지 않기 위해 만들었습니다.
// 처음 만든 날짜 : 2025년 12월 02일
// 마지막 업데이트 날짜 : 2025년 12월 06일

#define WM_EXIT_MB_THREAD 26001

#include "MB_ListBox.h"

typedef struct WorkThreadData
{
	HANDLE	hThread;
	HANDLE	hKillEvent;
	HWND	hNotifyWnd;
	CWnd	*pParent;
	int	nIterCount;
	DWORD dwThreadId;
	MB_ListBox *pEventList;
} MB_WTD;

class MB_Thread
{
private:
	MB_WTD	m_wtd;

public:
	MB_Thread();
	MB_Thread(LPTHREAD_START_ROUTINE pThreadFunc, HWND hWnd, CWnd *pParent);
	~MB_Thread();

	void CopyThreadData(const MB_Thread *pThread);
	MB_Thread *operator=(const MB_Thread &pThread);
	MB_Thread *operator=(const void *pThread);

	// 작업 스레드가 존재하는지 확인하는 함수
	HANDLE isExistWorkThread();
	// 작업 스레드가 종료되었는지 확인하는 함수
	DWORD isCloseThread();
	// 작업 스레드를 생성하는 함수
	void CreateThread(LPTHREAD_START_ROUTINE pThreadFunc, HWND hWnd, void *pParent, int nIterCount);
	// 작업 스레드를 종료하는 함수
	void DestroyThread();
	// 스레드 정리 함수
	void CleanUpThread();
	
	// getter / setter
	CWnd *getWindow();
	int getIterCount();
	void setEventList(MB_ListBox *pList);
};

#endif

