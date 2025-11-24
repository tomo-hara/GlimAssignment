// GLIMDlg.h: 헤더 파일
#pragma once
#define MAX_CLICK_COUNT 3
#define IMAGE_WIDTH		640
#define IMAGE_HEIGHT	480
#define IMAGE_BPP		8
#define COLOR_BLACK		0x03
#define MARGIN			200
#define DUMMY_X_SIZE	(IMAGE_WIDTH * 2)
#define DUMMY_Y_SIZE	(IMAGE_HEIGHT * 2)

// CGLIMDlg 대화 상자
class CGLIMDlg : public CDialogEx
{
private:
	CImage m_image;
	int m_nClickCount = 0;
	POINT m_posList[MAX_CLICK_COUNT];
	CRect m_rect; 

// 생성입니다.
public:
	CGLIMDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

	// point 좌표가 이미지 영역에 해당하는지 판단한다.
	bool isValidPos(CPoint point);
	// (x, y) 가 중심점 (n_CenterX, nCenterY) 반지름 nRadius 로 그려진 원 위의 좌표인지 확인한다.
	bool isInCircle(int x, int y, int nCenterX, int nCenterY, int nRadius);
	// 세 클릭 지점을 지나가는 정원을 그린다.
	void DrawGarden();
	// (x, y) 가 정원 가장자리에 해당하는 좌표인지 확인한다.
	bool isThickPos(int x, int y, int centerX, int centerY, double dRadius);
	// 중심점을 통해 원의 반지름을 구한다.
	double getRadius(POINT centerPos);
	// 두 점의 y 값이 일치할 때 그 점의 좌표를 반환한다.
	POINT findToIntersectionPoint();
	// y 좌표가 이미지 영역 위의 좌표인지 확인한다.
	bool isValidPos(int y);
	// point 좌표가 드러난 이미지 영역에 해당하는지 판단한다.
	bool isValidViewPos(CPoint point);

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
	afx_msg void OnBnClickedSetThickBtn();
	int m_nThick;
};
