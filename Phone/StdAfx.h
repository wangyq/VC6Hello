// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__0B380471_9865_4A79_B73B_38A16AE92548__INCLUDED_)
#define AFX_STDAFX_H__0B380471_9865_4A79_B73B_38A16AE92548__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include<Afxsock.h> //added by xie 2009-7-22

#include <windows.h>  //added by xie 2009-7-22


#include "GSMUSignalFormat.h" //added by xie 2009-7-23

#include "GSMUSignalProcess.h"  //added by xie 2009-7-24


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__0B380471_9865_4A79_B73B_38A16AE92548__INCLUDED_)
