#ifndef _MB_LISTBOX_H_
#define _MB_LISTBOX_H_

// 이 클래스는 (주)그림, SW 개발직군 신입 과제를 진행하는 중 스레드 관련 코드를 공개하지 않기 위해 만들었습니다.
// 처음 만든 날짜 : 2025년 12월 05일
// 마지막 업데이트 날짜 : 2025년 12월 06일

typedef struct ThreadDataFormat
{
	DWORD dwThreadId;
	LPWSTR pStr;
	UINT nStrLength;
} TDF;

class MB_ListBox : public CListBox
{
	DECLARE_DYNAMIC(MB_ListBox)
private:
	// Owner-Draw 펜 색상
	COLORREF m_pen_color;
	// Owner-Draw 상황별 브러쉬 색상
	COLORREF m_focus_brush_color, m_select_brush_color, m_bk_brush_color;
	// Owner-Draw 상황별 브러쉬 객체
	HBRUSH mh_bk_brush;
	// Owner-Draw 폰트 객체
	HFONT mh_font;
	// Owner-Draw 문자열 구성을 위한 버퍼
	WCHAR m_StrBuffer[512];

public:
	MB_ListBox();
	virtual ~MB_ListBox();

	// 대화상자의 메인 스레드 혹은 작업 스레드에서 item 을 추가하는 함수
	void AddEventString(const wchar_t *pStr);
	void AddEventString(const wchar_t *pStr, DWORD dwThreadId);

	void CleanUp();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH CtlColor(CDC * /*pDC*/, UINT /*nCtlColor*/);
	afx_msg void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual void PreSubclassWindow();
};

#endif _MB_LISTBOX_H_
