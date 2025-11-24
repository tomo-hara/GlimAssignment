// GLIMDlg.h: 헤더 파일
#pragma once
#define MAX_CLICK_COUNT 3
#define IMAGE_WIDTH		640
#define IMAGE_HEIGHT	480
#define IMAGE_BPP		8
#define COLOR_BLACK		0x03
#define MARGIN			200

// CGLIMDlg 대화 상자
class CGLIMDlg : public CDialogEx
{
private:
	CImage m_image;
	int m_nClickCount;

// 생성입니다.
public:
	CGLIMDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

	// point 좌표가 이미지 영역에 해당하는지 판단한다.
	bool isValidPos(CPoint point);
	// (x, y) 가 중심점 (n_CenterX, nCenterY) 반지름 nRadius 로 그려진 원 위의 좌표인지 확인한다.
	bool isInCircle(int x, int y, int nCenterX, int nCenterY, int nRadius);

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GLIM_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	int m_nRadius;
	afx_msg void OnBnClickedSetRadiusBtn();
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};
