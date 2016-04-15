// BaseStation.h : main header file for the BASESTATION application
//

#if !defined(AFX_BASESTATION_H__3C9F3868_7B23_40F9_8666_E2B6DA4D6486__INCLUDED_)
#define AFX_BASESTATION_H__3C9F3868_7B23_40F9_8666_E2B6DA4D6486__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CBaseStationApp:
// See BaseStation.cpp for the implementation of this class
//

class CBaseStationApp : public CWinApp
{
public:
	CBaseStationApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBaseStationApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CBaseStationApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BASESTATION_H__3C9F3868_7B23_40F9_8666_E2B6DA4D6486__INCLUDED_)
