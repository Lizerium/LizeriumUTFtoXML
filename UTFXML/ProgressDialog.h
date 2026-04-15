// ProgressDialog.h
//
// Refer to UTFXMLDlg.cpp for software documentation

#ifndef _PROGRESS_DIALOG_H_INCLUDED_
#define _PROGRESS_DIALOG_H_INCLUDED_

// Forward reference
class UTFXMLDlg;

/////////////////////////////////////////////////////////////////////////////
// ProgressDialog dialog

class ProgressDialog : public CDialog
{
// Construction
public:
	ProgressDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ProgressDialog)
	enum { IDD = IDD_PROGRESS_DIALOG };
	CProgressCtrl	m_ProgressCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ProgressDialog)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ProgressDialog)
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    UTFXMLDlg *m_Parent;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif _PROGRESS_DIALOG_H_INCLUDED_

