//////////////////////////////////////////////////////////////////////
// ProgressDialog.cpp
//
// Refer to UTFXMLDlg.cpp for software documentation
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Include Files
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UTFXML.h"
#include "UTFXMLDlg.h"
#include "ProgressDialog.h"


//////////////////////////////////////////////////////////////////////
// Definitions
//////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////
// ProgressDialog
//////////////////////////////////////////////////////////////////////

ProgressDialog::ProgressDialog(CWnd* pParent /* = NULL */)
	: CDialog(ProgressDialog::IDD, pParent)
{
    m_Parent = (UTFXMLDlg *) pParent;
	//{{AFX_DATA_INIT(ProgressDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void ProgressDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ProgressDialog)
	DDX_Control(pDX, IDC_PROGRESS, m_ProgressCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ProgressDialog, CDialog)
	//{{AFX_MSG_MAP(ProgressDialog)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void ProgressDialog::OnCancel() 
{
    m_Parent->m_ConversionAborted = true;

    CDialog::OnCancel();
}


BOOL ProgressDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetDlgItemText(IDC_STATUS, (m_Parent->m_FindingMaterials)
							   ? "Searching UTF files for material names..."
							   : "Counting UTF files...");

    BeginWaitCursor();

	SetTimer(1, 100, NULL);

    return TRUE;
}


BOOL ProgressDialog::DestroyWindow() 
{
	KillTimer(1);

    EndWaitCursor();
	
	return CDialog::DestroyWindow();
}


void ProgressDialog::OnTimer(UINT nIDEvent) 
{
    if (nIDEvent == 1)
    {
        char source_path[MAX_PATH];
	    EnterCriticalSection(&m_Parent->m_Mutex);
        int file_count = m_Parent->m_UtfFileCount;
        int files_opened = m_Parent->m_NumUtfFilesOpened;
        strcpy(source_path, m_Parent->m_CurrentSourcePath + m_Parent->m_PathTrim);
	    LeaveCriticalSection(&m_Parent->m_Mutex);

        // Has the background thread finished counting the files?
        if (file_count)
        {
            // Yes, display the conversion percent progress
            CString status;
			status.Format("Processed %d of %d UTF files (%d%%)", files_opened, file_count, 100 * files_opened / file_count);
			SetDlgItemText(IDC_STATUS, status);
            m_ProgressCtrl.SetRange(0, file_count);
            m_ProgressCtrl.SetPos(files_opened);
            // Have all files been processed?
            if (file_count == files_opened)
            {
                // Done! Close the dialog and continue
                Sleep(1000);
                OnOK();
                return;
            }
        }
        else
        {
			CString status;
			if (m_Parent->m_FindingMaterials)
			{
				if (!m_Parent->m_ConversionInProgress)
				{
					OnOK();
					return;
				}
				status.Format("Searching UTF files for material names... (%d)", files_opened);
			}
			else
			{
				status.Format("Counting UTF files... (%d)", files_opened);
			}
			SetDlgItemText(IDC_STATUS, status);
        }

        // Display the current source path
		SetDlgItemText(IDC_PATH, source_path);
    }

    CDialog::OnTimer(nIDEvent);
}


//////////////////////////////////////////////////////////////////////
// End ProgressDialog.cpp
//////////////////////////////////////////////////////////////////////

