// GLIMDlg.cpp: 구현 파일
#include "pch.h"
#include "framework.h"
#include "GLIM.h"
#include "GLIMDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CGLIMDlg 대화 상자

CGLIMDlg::CGLIMDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_GLIM_DIALOG, pParent)
	, m_nRadius(0)
	, m_nThick(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGLIMDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_RADIUS_EDIT, m_nRadius);
	DDX_Text(pDX, IDC_THICK_EDIT, m_nThick);
}

BEGIN_MESSAGE_MAP(CGLIMDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CGLIMDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CGLIMDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_SET_RADIUS_BTN, &CGLIMDlg::OnBnClickedSetRadiusBtn)
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_SET_THICK_BTN, &CGLIMDlg::OnBnClickedSetThickBtn)
END_MESSAGE_MAP()

// CGLIMDlg 메시지 처리기

BOOL CGLIMDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	int nWidth = IMAGE_WIDTH + DUMMY_X_SIZE * 2;
	int nHeight = IMAGE_HEIGHT + DUMMY_Y_SIZE * 2;
	int nBpp = IMAGE_BPP;
	m_image.Create(nWidth, -nHeight, nBpp);
	if (nBpp == IMAGE_BPP) {
		static RGBQUAD rgb[256];
		for (int i = 0; i < 256; i++) {
			rgb[i].rgbRed = rgb[i].rgbGreen = rgb[i].rgbBlue = i;
		}
		m_image.SetColorTable(0, 256, rgb);
	}

	unsigned char *fm = (unsigned char *)m_image.GetBits();
	memset(fm, 0xff, sizeof(unsigned char) * nWidth * nHeight);

	// 이미지 영역 확인에 문제가 없도록 윈도우 크기를 조절한다.
	MoveWindow(0, 0, IMAGE_WIDTH + MARGIN, IMAGE_HEIGHT + MARGIN);
	// 이미지와 대화상자 영역이 구분되도록 색상을 달리한다.
	SetBackgroundColor(RGB(0xf0, 0xf0, 0xf0));
	// 다른 컨트롤이 이미지 영역을 가리지 않도록 조정한다.
	GetDlgItem(IDC_SET_RADIUS_BTN)->MoveWindow(IMAGE_WIDTH + MARGIN - 136, 0, 116, 28);
	GetDlgItem(IDC_RADIUS_EDIT)->MoveWindow(IMAGE_WIDTH + MARGIN - 176, 0, 28, 28);
	GetDlgItem(IDC_SET_THICK_BTN)->MoveWindow(IMAGE_WIDTH + MARGIN - 136, 36, 116, 28);
	GetDlgItem(IDC_THICK_EDIT)->MoveWindow(IMAGE_WIDTH + MARGIN - 176, 36, 28, 28);
	// 대화상자 영역 갱신을 위해 정보를 멤버변수에 저장한다.
	GetClientRect(m_rect);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CGLIMDlg::OnPaint()
{
	CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.
	if (IsIconic())
	{
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		// CImage 객체에 수정한 내용을 dc에 반영한다.
		m_image.Draw(dc, 0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, DUMMY_X_SIZE, DUMMY_Y_SIZE, IMAGE_WIDTH, IMAGE_HEIGHT);
		
		//CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CGLIMDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CGLIMDlg::OnBnClickedOk() { /* CDialogEx::OnOK(); */ }
void CGLIMDlg::OnBnClickedCancel() { CDialogEx::OnCancel(); }

void CGLIMDlg::OnBnClickedSetRadiusBtn()
{
	// 사용자 입력 값을 변수에 대입한다.
	UpdateData(TRUE);
	// 유효하지 않은 값을 입력한 경우 예외처리될 수 있도록 값을 초기화한다.
	if (m_nRadius < 0) m_nRadius = 0;
	// 실제 사용될 int 값을 확인할 수 있도록 재출력한다.
	UpdateData(FALSE);
}

void CGLIMDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	if (!m_image.IsNull()) {
		m_image.Destroy();
	}
}

bool CGLIMDlg::isValidViewPos(CPoint point)
{
	CRect rect(DUMMY_X_SIZE, DUMMY_Y_SIZE, DUMMY_X_SIZE + IMAGE_WIDTH, DUMMY_Y_SIZE + IMAGE_HEIGHT);

	return rect.PtInRect(point);
}

bool CGLIMDlg::isValidPos(int y)
{
	return 0 <= y && y < m_image.GetHeight();
}

bool CGLIMDlg::isValidPos(CPoint point)
{
	CRect rect(0, 0, m_image.GetWidth(), m_image.GetHeight());

	return rect.PtInRect(point);
}

bool CGLIMDlg::isInCircle(int x, int y, int nCenterX, int nCenterY, int nRadius)
{
	bool bRet = false;

	double dX = x - nCenterX;
	double dY = y - nCenterY;
	double dDist = dX * dX + dY * dY;

	if (dDist < nRadius * nRadius) {
		bRet = true;
	}

	return bRet;
}

void CGLIMDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// 사용자 입력값이 올바르지 않은 경우 오류 메시지를 출력한다.
	if (!m_nRadius || !m_nThick) {
		AfxMessageBox(_T("먼저 유효한 입력값을 설정하세요."));
	} else {
		// 보이지 않는 대화상자 영역을 건너뛴다.
		point.x += DUMMY_X_SIZE;
		point.y += DUMMY_Y_SIZE;
		// 세 번째 클릭까지는 클릭 지점 그리기를 수행한다.
		if (isValidViewPos(point) && m_nClickCount < 3) {
			int nSttx = point.x - m_nRadius;
			int nStty = point.y - m_nRadius;
			int nEndx = point.x + m_nRadius;
			int nEndy = point.y + m_nRadius;

			unsigned char *fm = (unsigned char *)m_image.GetBits();
			int nPitch = m_image.GetPitch();

			for (int y = nStty; y < nEndy; y++) {
				for (int x = nSttx; x < nEndx; x++) {
					if (isInCircle(x, y, point.x, point.y, m_nRadius)) {
						fm[y * nPitch + x] = COLOR_BLACK;
					}
				}
			}

			// 클릭시 그려지는 원이 즉시 보이도록 대화상자 영역을 무효화 처리한다.
			InvalidateRect(m_rect);

			// 클릭시 좌표를 기억할 수 있도록 멤버변수에 저장한다.
			m_posList[m_nClickCount].x = point.x;
			m_posList[m_nClickCount].y = point.y;
			m_nClickCount++;
		} else {
			DrawGarden();
		}

	}

	CDialogEx::OnLButtonDown(nFlags, point);
}

void CGLIMDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialogEx::OnMouseMove(nFlags, point);
}

void CGLIMDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialogEx::OnLButtonUp(nFlags, point);
}

void CGLIMDlg::OnBnClickedSetThickBtn()
{
	// 사용자 입력 값을 변수에 대입한다.
	UpdateData(TRUE);
	// 유효하지 않은 값을 입력한 경우 예외처리될 수 있도록 값을 초기화한다.
	if (m_nThick < 0) m_nThick = 0;
	// 실제 사용될 int 값을 확인할 수 있도록 재출력한다.
	UpdateData(FALSE);
}

// 매개 변수 x 값에서 두 점 (x1, y1), (x2, y2) 를 지나는 수직이등분선 위 y 값을 반환한다. 
static double getYposOnPerpendicularBisector(double x1, double y1, double x2, double y2, double x)
{
	double dYpos;
	dYpos = -((x2 - x1) / (y2 - y1)) * (x - ((x1 + x2) / 2.0)) + ((y1 + y2) / 2.0);
	return dYpos;
}

POINT CGLIMDlg::findToIntersectionPoint()
{
	int x = 0, y;
	// 전체 이미지 영역을 x 좌표 기준으로 완전탐색한다.
	while (x < m_image.GetWidth()) {
		// 첫 번째, 두 번째 클릭 지점을 기준으로 그려지는 수직이등분선의 y 좌표를 구한다.
		y = (int)getYposOnPerpendicularBisector(m_posList[0].x, m_posList[0].y, m_posList[1].x, m_posList[1].y, x);
		// 두 번째, 세 번째 클릭 지점을 기준으로 그려지는 수직이등분선의 y 좌표를 구하고, 둘의 차이가 유효범위를 만족하면 탐색을 종료한다.
		if (isValidPos(y) && 1.0 > fabs(y - getYposOnPerpendicularBisector(m_posList[1].x, m_posList[1].y, m_posList[2].x, m_posList[2].y, x))) {
			break;
		}
		x++;
	}
	POINT pos;
	pos.x = x;
	pos.y = y;
	return pos;
}

double CGLIMDlg::getRadius(POINT centerPos)
{
	return sqrt((centerPos.x - m_posList[0].x) * (centerPos.x - m_posList[0].x) \
		+ (centerPos.y - m_posList[0].y) * (centerPos.y - m_posList[0].y));
}

bool CGLIMDlg::isThickPos(int x, int y, int centerX, int centerY, double dRadius)
{
	bool bRet = false;
	double dDist = sqrt((centerX - x) * (centerX - x) + (centerY - y) * (centerY - y));
	if (dRadius - m_nThick < dDist && dDist < dRadius) {
		bRet = true;
	}
	return bRet;
}

void CGLIMDlg::DrawGarden()
{
	unsigned char *fm = (unsigned char *)m_image.GetBits();
	int nPitch = m_image.GetPitch();

	POINT centerPos = findToIntersectionPoint();
	double dRadius = getRadius(centerPos);
	// 그리기 영역에 해당하는 부분만 정원인지 판단한다.
	for (int j = DUMMY_Y_SIZE; j < DUMMY_Y_SIZE + IMAGE_HEIGHT; j++) {
		for (int i = DUMMY_X_SIZE; i < DUMMY_X_SIZE + IMAGE_WIDTH; i++) {
			if (isThickPos(i, j, centerPos.x, centerPos.y, dRadius)) {
				fm[j * nPitch + i] = COLOR_BLACK;
			}
		}
	}
	// 정원이 즉시 보이도록 대화상자 영역을 무효화 처리한다.
	InvalidateRect(m_rect);
}