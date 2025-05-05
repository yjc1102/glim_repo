
// SubmitDlg.h: 헤더 파일
//

#pragma once


// CSubmitDlg 대화 상자
class CSubmitDlg : public CDialogEx
{
// 생성입니다.
public:
	CSubmitDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.
	~CSubmitDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SUBMIT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


private:
	RECT m_rtCanvas;
	POINT m_arPoints[3];
	RECT m_arRtPoints[3];
	HBRUSH m_hBR_Canvas, m_hBR_Points, m_hBR_CenterPT;
	int m_nDragPtIdx;
	HANDLE m_hThread;
	
private:
	void _ResetPoints();
	int _SetPoint(POINT pt);
	void _SetPoint(int nPtIdx,POINT pt);
	BOOL _IsValidPoint(POINT pt);
	void _DrawCanvas(HDC hDC);
	void _DrawTotalCircle(HDC hDC);
	void _DrawPointCircle(HDC hDC);
	void _DrawEllipse(HDC hDC, POINT pt, int nRadius);

	BOOL _GetCenterPoint(POINT &refPTCenter, LONG &lnRadius);

public:
	void AutoRun();

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedBtnReset();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedBtnRun();
};
