// PhoneDlg.h : header file
//

#if !defined(AFX_PHONEDLG_H__450673AD_191F_4C62_AA3E_0A47CF2D3300__INCLUDED_)
#define AFX_PHONEDLG_H__450673AD_191F_4C62_AA3E_0A47CF2D3300__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPhoneDlg dialog



#define WM_PHONERECVDATA WM_USER+100  //������Ϣ

#define TIMER_SETUP 1               //setup ����Ķ�ʱ�� ID Ϊ 1
#define TIMER_CONNECTACK 4          //connnect_ack �����ID Ϊ 4
#define TIMER_AUTOCLOSE 100           //�Զ��ر�ͨ���Ի���
#define TIMER_MOVE_PHONEMSG  3               //�����ƶ���ʱ��
#define TIMER_MOVE_BSMSG 5
class CPhoneDlg : public CDialog
{
// Construction
public:

	
	void SendReleaseSignal(void);
	void SendConnectAckSignal(void);
	static DWORD WINAPI RecvProc(LPVOID lpParameter); //�������ݽ���. xie 2009-7-22
	BOOL InitSocket();
	CPhoneDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPhoneDlg)
	enum { IDD = IDD_PHONE_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPhoneDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPhoneDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnSend();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnRadioSetuplost();
	afx_msg void OnRadioConnectacklost();
	afx_msg void OnRadioNoErr();
	afx_msg void OnBtnClear();
	//}}AFX_MSG
	afx_msg void OnPhoneRecvData(WPARAM wParam, LPARAM lParam); //������Ϣ����ԭ��. xie 2009-7-22
	DECLARE_MESSAGE_MAP()

private:
	
	BOOL m_bConnectACKSendSuccess;
	BOOL m_bSetupSendSuccess;
	SOCKET m_socketUp;
	SOCKET m_socketDown;
	SOCKADDR_IN m_addrToBS;
	
};

struct RECVPARAM
{
	SOCKET socketUp;
	SOCKET socketDown;
	HWND hwnd;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PHONEDLG_H__450673AD_191F_4C62_AA3E_0A47CF2D3300__INCLUDED_)
