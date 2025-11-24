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
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGLIMDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_RADIUS_EDIT, m_nRadius);
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
	int nWidth = IMAGE_WIDTH;
	int nHeight = IMAGE_HEIGHT;
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
	SetBackgroundColor(RGB(0xF0, 0xF0, 0xF0));
	// 다른 컨트롤이 이미지 영역을 가리지 않도록 조정한다.
	GetDlgItem(IDC_SET_RADIUS_BTN)->MoveWindow(IMAGE_WIDTH + MARGIN - 136, 0, 116, 28);
	GetDlgItem(IDC_RADIUS_EDIT)->MoveWindow(IMAGE_WIDTH + MARGIN - 176, 0, 28, 28);

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
		m_image.Draw(dc, 0, 0);
		
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
	// 반지름 값이 올바르지 않은 경우 오류 메시지를 출력한다.
	if (!m_nRadius) {
		AfxMessageBox(_T("먼저 유효한 반지름 값을 설정하세요."));
	} else {
		// 세 번째 클릭까지는 클릭 지점 그리기를 수행한다.
		if (isValidPos(point) && m_nClickCount < 3) {
			int nSttx = point.x - m_nRadius;
			int nStty = point.y - m_nRadius;
			int nEndx = point.x + m_nRadius;
			int nEndy = point.y + m_nRadius;

			unsigned char *fm = (unsigned char *)m_image.GetBits();
			int nBlack = COLOR_BLACK;
			int nPitch = m_image.GetPitch();

			for (int y = nStty; y < nEndy; y++) {
				for (int x = nSttx; x < nEndx; x++) {
					if (isInCircle(x, y, point.x, point.y, m_nRadius)) {
						fm[y * nPitch + x] = nBlack;
					}
				}
			}

			// 클릭시 그려지는 원이 즉시 보이도록 해당 영역을 무효화 처리한다.
			CRect redrawRect(nSttx, nStty, nEndx, nEndy);
			InvalidateRect(redrawRect);

			m_nClickCount++;
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
