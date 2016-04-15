// BaseStationDlg.h : header file
//

#if !defined(AFX_BASESTATIONDLG_H__57870988_2ECB_45C4_B42E_88E4E86F2711__INCLUDED_)
#define AFX_BASESTATIONDLG_H__57870988_2ECB_45C4_B42E_88E4E86F2711__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CBaseStationDlg dialog
#define WM_RECVDATA WM_USER+1  //接收消息


#define TIMER_CONNECT 3               //setup 信令的定时器 ID 为 3
#define TIMER_DISCONNECT 5          //connnect_ack 信令的ID 为 5


class CBaseStationDlg : public CDialog
{
// Construction
public:
	void SendAlteringSignal(void);
	void SendDisConnectSignal(void);
	void SendConnectSignal(void);
	static DWORD WINAPI RecvProc(LPVOID lpParameter); //接收数据进程. xie 2009-7-22
	BOOL InitSocket();
	
	CBaseStationDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CBaseStationDlg)
	enum { IDD = IDD_BASESTATION_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBaseStationDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CBaseStationDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnRadioNoErr();
	afx_msg void OnRadioConnectLost();
	afx_msg void OnRadioDisconnectLost();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBtnClear();
	//}}AFX_MSG
	afx_msg void OnRecvData(WPARAM wParam, LPARAM lParam); //接收消息函数原型. xie 2009-7-22
	DECLARE_MESSAGE_MAP()
private:
	SOCKET m_socketUp;
	SOCKET m_socketDown;
	SOCKADDR_IN m_addrToPhone;
};

struct RECVPARAM
{
	SOCKET socketUp;
	SOCKET socketDown;
	HWND hwnd;
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BASESTATIONDLG_H__57870988_2ECB_45C4_B42E_88E4E86F2711__INCLUDED_)
