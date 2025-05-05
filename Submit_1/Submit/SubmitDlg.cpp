
// SubmitDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "Submit.h"
#include "SubmitDlg.h"
#include "afxdialogex.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSubmitDlg 대화 상자



CSubmitDlg::CSubmitDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SUBMIT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_hThread = INVALID_HANDLE_VALUE;

	m_nDragPtIdx = -1;
	/*/
	m_hBR_Canvas = ::CreateSolidBrush(RGB(0, 0, 0));
	m_hBR_Points = ::CreateSolidBrush(RGB(255, 255, 255));
	/*/
	m_hBR_Canvas = ::CreateSolidBrush(RGB(255,255,255));
	m_hBR_Points = ::CreateSolidBrush(RGB(0,0,0));
	
	//*/
	m_hBR_CenterPT = ::CreateSolidBrush(RGB(255, 0, 0));

	_ResetPoints();
}

CSubmitDlg::~CSubmitDlg() {
	DeleteObject(m_hBR_Canvas);
	DeleteObject(m_hBR_Points);
	DeleteObject(m_hBR_CenterPT);
}

void CSubmitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSubmitDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_ERASEBKGND()
	ON_WM_SIZING()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BTN_RESET, &CSubmitDlg::OnBnClickedBtnReset)
	ON_WM_MOUSEMOVE()
	ON_WM_GETMINMAXINFO()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_BTN_RUN, &CSubmitDlg::OnBnClickedBtnRun)
END_MESSAGE_MAP()


// CSubmitDlg 메시지 처리기

BOOL CSubmitDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// 점의 기본 반지름
	((CEdit*)GetDlgItem(IDC_EDIT_PT_RADIUS))->SetWindowText(_T("10"));

	// 정원 선의 두께
	((CEdit*)GetDlgItem(IDC_EDIT_ELLIPSE_THICK))->SetWindowText(_T("3"));
	//::GetClientRect(GetSafeHwnd(), &_rtCanvas);
	//InvalidateRect(&_rtCanvas);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CSubmitDlg::OnPaint()
{
	if (IsIconic())
	{
		return;
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

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
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CSubmitDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



BOOL CSubmitDlg::PreTranslateMessage(MSG* pMsg)
{
	if ((WM_KEYDOWN == pMsg->message) && ( (VK_RETURN == pMsg->wParam) || (VK_ESCAPE== pMsg->wParam))) {
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL CSubmitDlg::OnEraseBkgnd(CDC* pDC)
{
	//*/
	CDialogEx::OnEraseBkgnd(pDC);
	HDC hDC = pDC->GetSafeHdc();

	this->_DrawCanvas(hDC);

	return TRUE;
	/*/

	return CDialogEx::OnEraseBkgnd(pDC);

	//*/

}

void CSubmitDlg::_ResetPoints() {
	for (int i = 0; i < 3; ++i) {
		m_arPoints[i].x = LONG_MIN;
		m_arPoints[i].y = LONG_MIN;

		SetRect(&(m_arRtPoints[i]), LONG_MIN, LONG_MIN, LONG_MIN, LONG_MIN);
	}
}

int CSubmitDlg::_SetPoint(POINT pt) {
	int nIdx = -1;

	int nRadius = GetDlgItemInt(IDC_EDIT_PT_RADIUS);

	if (PtInRect(&m_rtCanvas, pt)) {

		// 기존 점의 영역 내에있는지 판별
		for (int i = 0; i < 3; ++i) {
			if (_IsValidPoint(m_arPoints[i]) && PtInRect(&m_arRtPoints[i], pt)) {
				nIdx = i;
				break;
			}
		}

		if (nIdx == -1) {
			for (int i = 0; i < 3; ++i) {
				if (!_IsValidPoint(m_arPoints[i])) {
					nIdx = i;
					m_arPoints[i] = pt;
					SetRect(&(m_arRtPoints[i]), pt.x - nRadius, pt.y - nRadius, pt.x + nRadius, pt.y + nRadius);
					break;
				}
			}
		}
	}

	return nIdx;
}

void CSubmitDlg::_SetPoint(int nPtIdx, POINT pt) {

	int nRadius = GetDlgItemInt(IDC_EDIT_PT_RADIUS);
	m_arPoints[nPtIdx] = pt;
	SetRect(&(m_arRtPoints[nPtIdx]), pt.x - nRadius, pt.y - nRadius, pt.x + nRadius, pt.y + nRadius);

}

BOOL CSubmitDlg::_IsValidPoint(POINT pt) {
	BOOL bRet = TRUE;

	if ((pt.x == LONG_MIN) || (pt.y == LONG_MIN)) {
		bRet = FALSE;
	}

	return bRet;
}

void CSubmitDlg::_DrawCanvas(HDC hDC) {
	// 검은색 배경을 그린다.

	// 점들이 그려질 배경 영역 지정
	RECT rtTemp;
	this->GetClientRect(&rtTemp);
	rtTemp.left = 10;
	rtTemp.top = 40;
	rtTemp.right = rtTemp.right - 10;
	rtTemp.bottom = rtTemp.bottom - 10;

	m_rtCanvas = rtTemp;

	HDC hCompDC = ::CreateCompatibleDC(hDC);
	HBITMAP hCompBit = ::CreateCompatibleBitmap(hDC, m_rtCanvas.right, m_rtCanvas.bottom);

	HBITMAP hBit_Old = (HBITMAP)::SelectObject(hCompDC, hCompBit);

	this->GetClientRect(&rtTemp);
	::FillRect(hCompDC, &rtTemp, m_hBR_Canvas);

	// 세 점을 지나가는 원을 그린다.
	_DrawTotalCircle(hCompDC);

	// 세 점을 그린다.
	_DrawPointCircle(hCompDC);

	// 세 점의 중심 점을 그린다.
	//_DrawCenterPT(hCompDC);

	::BitBlt(hDC, m_rtCanvas.left, m_rtCanvas.top, m_rtCanvas.right - m_rtCanvas.left, m_rtCanvas.bottom - m_rtCanvas.top, hCompDC, 0, 0, SRCCOPY);

	::SelectObject(hCompDC, hBit_Old);
	::DeleteObject(hCompBit);
	::DeleteObject(hBit_Old);
	::DeleteDC(hCompDC);
}

void CSubmitDlg:: _DrawTotalCircle(HDC hDC) {
	POINT ptCenter = POINT{ LONG_MIN, LONG_MIN };
	LONG lnCenterEllipsRadius = 0L;
	HBRUSH hBR_Old = NULL;
	HPEN hPen = NULL, hPen_Old = NULL;
	int nThick = GetDlgItemInt(IDC_EDIT_ELLIPSE_THICK);

	if (!_GetCenterPoint(ptCenter, lnCenterEllipsRadius)) {
		return;
	}

	hPen = ::CreatePen(PS_SOLID, nThick, RGB(0, 0, 0));

	// 검은색의 원 테두리를 그린다.
	hBR_Old = (HBRUSH) SelectObject(hDC, m_hBR_Canvas);
	hPen_Old = (HPEN) SelectObject(hDC, hPen);
	_DrawEllipse(hDC, ptCenter, lnCenterEllipsRadius);

	SelectObject(hDC, hBR_Old);
	SelectObject(hDC, hPen_Old);
}

void CSubmitDlg::_DrawPointCircle(HDC hDC) {

	int nRadius = 0;

	nRadius = GetDlgItemInt(IDC_EDIT_PT_RADIUS);

	HBRUSH hBR_Old = (HBRUSH)::SelectObject(hDC, m_hBR_Points);

	for (int i = 0; i < 3; ++i) {
		if (_IsValidPoint(m_arPoints[i])) {
			_DrawEllipse(hDC, m_arPoints[i], nRadius);
		}
	}

	::SelectObject(hDC, hBR_Old);
}

void CSubmitDlg::_DrawEllipse(HDC hDC, POINT pt, int nRadius) {

	::Ellipse(hDC, pt.x - nRadius - 10, pt.y - nRadius - 40, pt.x + nRadius - 10, pt.y + nRadius - 40);

}

void CSubmitDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialogEx::OnSizing(fwSide, pRect);

	InvalidateRect(&m_rtCanvas);
}


void CSubmitDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	
	if (GetDlgItem(IDC_BTN_RESET)) {
		RECT rt;
		GetDlgItem(IDC_BTN_RESET)->GetWindowRect(&rt);
		ScreenToClient(&rt);
		GetDlgItem(IDC_BTN_RESET)->MoveWindow(cx - 10 - (rt.right - rt.left), rt.top, rt.right - rt.left, rt.bottom - rt.top);
	}

	InvalidateRect(&m_rtCanvas);
}


void CSubmitDlg::OnBnClickedBtnReset()
{
	_ResetPoints();

	InvalidateRect(&m_rtCanvas);
}

void CSubmitDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 640;
	lpMMI->ptMinTrackSize.y = 480;


	CDialogEx::OnGetMinMaxInfo(lpMMI);
}


BOOL CSubmitDlg::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.cx = 640;
	cs.cy = 480;

	return CDialogEx::PreCreateWindow(cs);
}

void CSubmitDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_nDragPtIdx = _SetPoint(point);

	InvalidateRect(&m_rtCanvas);

	CDialogEx::OnLButtonDown(nFlags, point);
}

void CSubmitDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_nDragPtIdx != -1) {
		_SetPoint(m_nDragPtIdx, point);
		InvalidateRect(&m_rtCanvas);
	}

	CDialogEx::OnMouseMove(nFlags, point);
}

void CSubmitDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_nDragPtIdx = -1;
	InvalidateRect(&m_rtCanvas);

	CDialogEx::OnLButtonUp(nFlags, point);
}

LONG distance(const POINT &a, const POINT &b) {
	return sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
}

BOOL CSubmitDlg::_GetCenterPoint(POINT &refPTCenter, LONG &lnRadius) {

	if ((LONG_MIN == m_arPoints[2].x) || (LONG_MIN == m_arPoints[2].y)) {
		return FALSE;
	}

	double D = 2 * (m_arPoints[0].x*(m_arPoints[1].y - m_arPoints[2].y) + m_arPoints[1].x*(m_arPoints[2].y - m_arPoints[0].y) + m_arPoints[2].x*(m_arPoints[0].y - m_arPoints[1].y));
	if (std::abs(D) < 1e-10) {
		return FALSE;
	}

	double A = m_arPoints[0].x * m_arPoints[0].x + m_arPoints[0].y * m_arPoints[0].y;
	double B = m_arPoints[1].x * m_arPoints[1].x + m_arPoints[1].y * m_arPoints[1].y;
	double C = m_arPoints[2].x * m_arPoints[2].x + m_arPoints[2].y * m_arPoints[2].y;
	
	refPTCenter.x = (A * (m_arPoints[1].y - m_arPoints[2].y) + B * (m_arPoints[2].y - m_arPoints[0].y) + C * (m_arPoints[0].y - m_arPoints[1].y)) / D;
	refPTCenter.y = (A * (m_arPoints[2].x - m_arPoints[1].x) + B * (m_arPoints[0].x - m_arPoints[2].x) + C * (m_arPoints[1].x - m_arPoints[0].x)) / D;

	lnRadius = distance(refPTCenter, m_arPoints[0]);

	return TRUE;
}

void CSubmitDlg:: _DrawCenterPT(HDC hDC) {

	POINT ptCenter = POINT{ LONG_MIN, LONG_MIN };
	LONG lnCenterEllipsRadius = 0L;

	int nRadius = 0;

	if (!_GetCenterPoint(ptCenter, lnCenterEllipsRadius)) {
		return;
	}

	nRadius = GetDlgItemInt(IDC_EDIT_PT_RADIUS);

	HBRUSH hBR_Old = (HBRUSH)::SelectObject(hDC, m_hBR_CenterPT);

	_DrawEllipse(hDC, ptCenter, nRadius);

	::SelectObject(hDC, hBR_Old);

}

unsigned int __stdcall WorkerThreadFunc(LPVOID pParam) {

	CSubmitDlg *pDlg = (CSubmitDlg*)pParam;
	
	pDlg->AutoRun();

	return 0;
}

void CSubmitDlg::OnBnClickedBtnRun()
{
	m_hThread = (HANDLE) _beginthreadex(NULL, 0, WorkerThreadFunc, (void*)this, 0, NULL);

	((CButton*)GetDlgItem(IDC_BTN_RUN))->EnableWindow(FALSE);
}

void CSubmitDlg::AutoRun() {
	const int interval_ms = 500;     // 0.5초
	const int total_time_ms = 5000; // 5초
	const int repeat_count = total_time_ms / interval_ms;

	for (int i = 0; i < repeat_count; ++i) {



		Sleep(interval_ms);
	}

	((CButton*)GetDlgItem(IDC_BTN_RUN))->EnableWindow(TRUE);

}