// GLIMDlg.cpp: 구현 파일
#include "pch.h"
#include "framework.h"
#include "GLIM.h"
#include "GLIMDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
	#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
	#pragma comment(lib, "DSH32_Mb_MFC.lib")
#else 
	#pragma comment(lib, "RST32_Mb_MFC.lib")
#endif

// CGLIMDlg 대화 상자

CGLIMDlg::CGLIMDlg(CWnd *pParent /*=nullptr*/)
	: CDialogEx(IDD_GLIM_DIALOG, pParent)
	, m_nRadius(0)
	, m_nThick(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	memset(&m_centerPos, 0, sizeof(POINT));
}

void CGLIMDlg::DoDataExchange(CDataExchange *pDX)
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
	ON_BN_CLICKED(IDC_CLEANUP_BTN, &CGLIMDlg::OnBnClickedCleanupBtn)
	ON_BN_CLICKED(IDC_RAND_BTN, &CGLIMDlg::OnBnClickedRandBtn)
	ON_BN_CLICKED(IDC_AUTO_RAND_BTN, &CGLIMDlg::OnBnClickedAutoRandBtn)
END_MESSAGE_MAP()

// CGLIMDlg 메시지 처리기

void CGLIMDlg::Clear()
{
	memset(mp_fm, 0xff, sizeof(unsigned char) * (DUMMY_X_SIZE * 2 + IMAGE_WIDTH) * (DUMMY_Y_SIZE * 2 + IMAGE_HEIGHT));
}

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

	mp_fm = (unsigned char *)m_image.GetBits();
	m_nPitch = m_image.GetPitch();
	Clear();

	// 이미지 영역 확인에 문제가 없도록 윈도우 크기를 조절한다.
	MoveWindow(0, 0, IMAGE_WIDTH + MARGIN, IMAGE_HEIGHT + MARGIN);
	// 이미지와 대화상자 영역이 구분되도록 색상을 달리한다.
	SetBackgroundColor(RGB(0xf0, 0xf0, 0xf0));
	// 다른 컨트롤이 이미지 영역을 가리지 않도록 조정한다.
	GetDlgItem(IDC_SET_RADIUS_BTN)->MoveWindow(IMAGE_WIDTH + MARGIN - 136, 0, 116, 28);
	GetDlgItem(IDC_RADIUS_EDIT)->MoveWindow(IMAGE_WIDTH + MARGIN - 176, 0, 28, 28);
	GetDlgItem(IDC_SET_THICK_BTN)->MoveWindow(IMAGE_WIDTH + MARGIN - 136, 36, 116, 28);
	GetDlgItem(IDC_THICK_EDIT)->MoveWindow(IMAGE_WIDTH + MARGIN - 176, 36, 28, 28);
	GetDlgItem(IDC_CLEANUP_BTN)->MoveWindow(IMAGE_WIDTH + MARGIN - 136, 36 * 2, 116, 28);
	GetDlgItem(IDC_RAND_BTN)->MoveWindow(IMAGE_WIDTH + MARGIN - 136, 36 * 4, 116, 28);
	GetDlgItem(IDC_AUTO_RAND_BTN)->MoveWindow(IMAGE_WIDTH + MARGIN - 136, 36 * 5, 116, 28);


	// 대화상자 영역 갱신을 위해 정보를 멤버변수에 저장한다.
	m_rect.SetRect(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT);

	// IDC_EVENT_LIST컨트롤을 MB_ListBox 객체로 서브클래싱한다.
	m_event_list.SubclassDlgItem(IDC_EVENT_LIST, this);
	GetDlgItem(IDC_EVENT_LIST)->MoveWindow(0, IMAGE_HEIGHT + 16, IMAGE_WIDTH, 136);
	m_event_list.AddEventString(L"프로그램이 시작되었습니다.");
	m_thread.setEventList(&m_event_list);

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

	reDraw();
}

void CGLIMDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	if (!m_image.IsNull()) {
		m_image.Destroy();
	}

	m_thread.DestroyThread();
}

bool CGLIMDlg::isValidViewPos(CPoint point)
{
	CRect rect(DUMMY_X_SIZE, DUMMY_Y_SIZE, DUMMY_X_SIZE + IMAGE_WIDTH, DUMMY_Y_SIZE + IMAGE_HEIGHT);

	return rect.PtInRect(point);
}

__inline bool CGLIMDlg::isValidPos(int y)
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
	}
	else {
		// 보이지 않는 대화상자 영역을 건너뛴다.
		point.Offset(DUMMY_X_SIZE, DUMMY_Y_SIZE);
		// 세 번째 클릭까지는 클릭 지점 그리기를 수행한다.
		if (isValidViewPos(point) && m_nClickCount < 3) {
			// 클릭시 좌표를 기억할 수 있도록 멤버변수에 저장한다.
			m_posList[m_nClickCount].x = point.x;
			m_posList[m_nClickCount].y = point.y;
			m_nClickCount++;
			// 기억한 좌표를 바탕으로 원의 중심 UI를 표시한다.
			drawPoints();
		}
		else {
			for (int i = 0; i < MAX_CLICK_COUNT; i++) {
				if (isInCircle(m_posList[i].x, m_posList[i].y, point.x, point.y, m_nRadius)) {
					// 클릭 지점을 구분할 수 있도록 색인을 초기화한다.
					m_clickIndex = i;
					// 클릭 지점을 이동할 수 있도록 클릭한 지점 좌표정보를 저장해둔다.
					m_prevPos = point;
					break;
				}
			}
		}
		drawGarden();
		// 클릭시 그려지는 것들이 즉시 보이도록 대화상자 영역을 무효화 처리한다.
		InvalidateRect(m_rect);
	}

	CDialogEx::OnLButtonDown(nFlags, point);
}

void CGLIMDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	point.Offset(DUMMY_X_SIZE, DUMMY_Y_SIZE);
	if (m_clickIndex != NONCLICK_STATE) {
		m_posList[m_clickIndex] += point - m_prevPos;
		m_prevPos = point;

		reDraw();
		UpdateWindow();
	}
	else CDialogEx::OnMouseMove(nFlags, point);
}

void CGLIMDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_clickIndex != NONCLICK_STATE) {
		m_clickIndex = NONCLICK_STATE;
	}
	else CDialogEx::OnLButtonUp(nFlags, point);
}

void CGLIMDlg::OnBnClickedSetThickBtn()
{
	// 사용자 입력 값을 변수에 대입한다.
	UpdateData(TRUE);
	// 유효하지 않은 값을 입력한 경우 예외처리될 수 있도록 값을 초기화한다.
	if (m_nThick < 0) m_nThick = 0;
	// 실제 사용될 int 값을 확인할 수 있도록 재출력한다.
	UpdateData(FALSE);

	reDraw();
}

// 매개 변수 x 값에서 두 점 (x1, y1), (x2, y2) 를 지나는 수직이등분선 위 y 값을 반환한다. 
__inline static double getYposOnPerpendicularBisector(double x1, double y1, double x2, double y2, double x)
{
	double dYpos;
	dYpos = -((x2 - x1) / (y2 - y1)) * (x - ((x1 + x2) / 2.0)) + ((y1 + y2) / 2.0);
	return dYpos;
}

POINT CGLIMDlg::findToIntersectionPoint()
{
	CPoint pos(0, 0);
	// 전체 이미지 영역을 x 좌표 기준으로 완전탐색한다.
	while (pos.x < m_image.GetWidth()) {
		// 첫 번째, 두 번째 클릭 지점을 기준으로 그려지는 수직이등분선의 y 좌표를 구한다.
		pos.y = (int)getYposOnPerpendicularBisector(m_posList[0].x, m_posList[0].y, m_posList[1].x, m_posList[1].y, pos.x);
		// 두 번째, 세 번째 클릭 지점을 기준으로 그려지는 수직이등분선의 y 좌표를 구하고, 둘의 차이가 유효범위를 만족하면 탐색을 종료한다.
		if (isValidPos(pos.y) && 1.0 > fabs(pos.y - getYposOnPerpendicularBisector(m_posList[1].x, m_posList[1].y, m_posList[2].x, m_posList[2].y, pos.x))) {
			break;
		}
		pos.x++;
	}
	return pos;
}

double CGLIMDlg::getRadius(POINT centerPos)
{
	return sqrt((centerPos.x - m_posList[0].x) * (centerPos.x - m_posList[0].x) \
		+ (centerPos.y - m_posList[0].y) * (centerPos.y - m_posList[0].y));
}

// (x, y) 가 정원 가장자리에 해당하는 좌표인지 확인한다.
__inline static bool isThickPos(int x, int y, int centerX, int centerY, double dRadius, int nThick)
{
	bool bRet = false;
	double dDist = sqrt((centerX - x) * (centerX - x) + (centerY - y) * (centerY - y));
	if (dRadius - nThick < dDist && dDist < dRadius) {
		bRet = true;
	}
	return bRet;
}

void CGLIMDlg::drawGarden()
{
	if (m_nClickCount == 3) {
		POINT centerPos = findToIntersectionPoint();
		double dRadius = getRadius(centerPos);
		int nThick = getThick();
		// 그리기 영역에 해당하는 부분만 정원인지 판단한다.
		for (int j = DUMMY_Y_SIZE; j < DUMMY_Y_SIZE + IMAGE_HEIGHT; j++) {
			for (int i = DUMMY_X_SIZE; i < DUMMY_X_SIZE + IMAGE_WIDTH; i++) {
				if (isThickPos(i, j, centerPos.x, centerPos.y, dRadius, nThick)) {
					mp_fm[j * m_nPitch + i] = COLOR_BLACK;
				}
			}
		}
	}
}

void CGLIMDlg::drawPoints()
{
	for (int i = 0; i < m_nClickCount; i++) {
		int nSttx = m_posList[i].x - m_nRadius;
		int nStty = m_posList[i].y - m_nRadius;
		int nEndx = m_posList[i].x + m_nRadius;
		int nEndy = m_posList[i].y + m_nRadius;

		for (int y = nStty; y < nEndy; y++) {
			for (int x = nSttx; x < nEndx; x++) {
				if (isInCircle(x, y, m_posList[i].x, m_posList[i].y, m_nRadius)) {
					mp_fm[y * m_nPitch + x] = COLOR_BLACK;
				}
			}
		}
	}
}

void CGLIMDlg::cleanUp()
{
	m_nClickCount = 0;
	m_nThick = 0;
	m_nRadius = 0;
	Clear();
	InvalidateRect(m_rect);
	UpdateData(FALSE);
	m_event_list.CleanUp();
}

void CGLIMDlg::OnBnClickedCleanupBtn()
{
	cleanUp();
}

void CGLIMDlg::reDraw()
{
	Clear();
	drawPoints();
	drawGarden();
	InvalidateRect(m_rect);
}

bool CGLIMDlg::isOnCircle(int nCenterX, int nCenterY)
{
	bool bRet = true;
	int nDistSquare;
	int nDx = m_posList[MAX_CLICK_COUNT - 1].x - nCenterX;
	int nDy = m_posList[MAX_CLICK_COUNT - 1].y - nCenterY;
	// 마지막 클릭 지점을 기준으로 반지름의 제곱값을 계산한다.
	int nRadiusSquare = nDx * nDx + nDy * nDy;
	// 나머지 클릭 지점이 유효 범위를 벗어나면 false 값을 설정한다.
	for (int i = 0; i < MAX_CLICK_COUNT - 1; i++) {
		nDx = m_posList[i].x - nCenterX;
		nDy = m_posList[i].y - nCenterY;
		nDistSquare = nDx * nDx + nDy * nDy;

		if (1000 < abs(nDistSquare - nRadiusSquare)) {
			bRet = false;
			break;
		}
	}

	return bRet;
}

DWORD WINAPI threadFn(void *pData)
{
	MB_Thread *pThread = (MB_Thread *)pData;
	CGLIMDlg *pDlg = (CGLIMDlg *)pThread->getWindow();

	// 시드 설정은 스레드 단위로 이뤄짐으로 여기서 처리한다.
	srand((unsigned int)time(NULL));

	for (int i = 1; i <= pThread->getIterCount(); i++) {
		if (pThread->isCloseThread()) break;

		auto start = chrono::system_clock::now();
		CPoint *pPos = pDlg->getPosList(), centerPos;
		// 입출력 단위연산의 원자성을 보존하기 위해 임계영역을 설정한다.
		//g_cs.Lock();
		// 클릭 지점 두 개를 랜덤한 위치에 생성한다.
		for (int sub_i = 1; sub_i < MAX_CLICK_COUNT; sub_i++) {
			pPos[sub_i].x = DUMMY_X_SIZE + (rand() % IMAGE_WIDTH);
			pPos[sub_i].y = DUMMY_Y_SIZE + (rand() % IMAGE_HEIGHT);
		}
		// 나머지 클릭 지점이 원 둘레에 위치할 수 있도록 유효한 랜덤값을 설정한다.
		do {
			pPos[0].x = DUMMY_X_SIZE + (rand() % IMAGE_WIDTH);
			pPos[0].y = DUMMY_Y_SIZE + (rand() % IMAGE_HEIGHT);
			centerPos = pDlg->findToIntersectionPoint();
		} while (!pDlg->isOnCircle(centerPos.x, centerPos.y));

		pDlg->Clear();
		pDlg->drawPoints();
		// 반복문에서 중복되는 부분을 미리 처리한다.
		unsigned char *fm = pDlg->getFieldImage();
		int nPitch = pDlg->getPitch();
		double dRadius = pDlg->getRadius(centerPos);
		int nThick = pDlg->getThick();
		// 그리기 영역에 해당하는 부분만 정원인지 판단한다.
		for (int j = DUMMY_Y_SIZE; j < DUMMY_Y_SIZE + IMAGE_HEIGHT; j++) {
			for (int i = DUMMY_X_SIZE; i < DUMMY_X_SIZE + IMAGE_WIDTH; i++) {
				if (isThickPos(i, j, centerPos.x, centerPos.y, dRadius, nThick)) {
					fm[j * nPitch + i] = COLOR_BLACK;
				}
			}
		}
		// 입출력 단위연산이 종료되었음으로 임계영역을 해제한다.
		//g_cs.Unlock();
		// 메인 스레드가 이미지 객체 정보를 출력할 수 있도록 메시지를 전송한다.
		pDlg->PostMessageW(WM_REFRESH, (WPARAM)i, (LPARAM)0);
		auto end = chrono::system_clock::now();
		auto msec = chrono::duration_cast<chrono::milliseconds>(end - start);
		cout << ">>> " << msec.count() << " msec" << endl;
		// 0.5초 간격으로 작업하기 위한 지연함수
		Sleep(480);
	}

	pThread->DestroyThread();

	return 0;
}

void CGLIMDlg::OnBnClickedRandBtn()
{
	m_thread.CreateThread(threadFn, m_hWnd, this, 1);
}

void CGLIMDlg::OnBnClickedAutoRandBtn()
{
	m_thread.CreateThread(threadFn, m_hWnd, this, MAX_ITER_COUNT);
}

void CGLIMDlg::refresh()
{
	InvalidateRect(m_rect);
	UpdateWindow();
}

LRESULT CGLIMDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_REFRESH) {
		refresh();
	}
	else if (message == WM_EXIT_MB_THREAD) {
		/* 스레드 정상 종료 */
		//m_event_list.AddEventString(L"스레드 작업이 정상 종료되었습니다.");
	}

	return CDialogEx::WindowProc(message, wParam, lParam);
}
