// traverwin.cpp : Defines the entry point for the console application.
//
// GetDesktopWindow.cpp : �������̨Ӧ�ó������ڵ㡣
#include "stdafx.h"
#define _AFXDLL
#include <afxwin.h>

#ifdef  GETDESKTOPWIN
//����    1   error C1189: #error :  Building MFC application with /MD[d] (CRT dll version) requires MFC shared dll version. 
//Please #define _AFXDLL or do not use /MD[d]   e:\programfilesx86\microsoftvisualstudio10\vc\atlmfc\include\afx.h  24  1   GetDesktopWindow

int _tmain(int argc, char* argv[])
{
#if 0
    //1.�Ȼ�����洰��
        CWnd* pDesktopWnd = CWnd::GetDesktopWindow();
    //2.���һ���Ӵ���
        CWnd* pWnd = pDesktopWnd->GetWindow(GW_CHILD);
    //3.ѭ��ȡ�������µ������Ӵ���
        while(pWnd != NULL)
        {
            //��ô�������
            CString strClassName = _T("");
            ::GetClassName(pWnd->GetSafeHwnd(),strClassName.GetBuffer(256),256);

            //��ô��ڱ���
            CString strWindowText = _T("");
            ::GetWindowText(pWnd->GetSafeHwnd(),strWindowText.GetBuffer(256),256);
            printf("%s \n", (LPCTSTR)strWindowText);
            //������һ���Ӵ���
            pWnd = pWnd->GetWindow(GW_HWNDNEXT);
        }
#elif 0
    HWND active_wh = GetActiveWindow(); 
    CString strWindowText = _T("");
    ::GetWindowText(active_wh, strWindowText.GetBuffer(256), 256);
    printf("%s \n", (LPCTSTR)strWindowText);
#else
    while (1)
    {
        HWND hWnd = ::GetForegroundWindow();
        if(hWnd)
        {
            int nLen = ::GetWindowTextLength(hWnd);
            TCHAR* buf = new TCHAR[nLen+1];
            ::GetWindowText(hWnd, buf, nLen+1);
            printf("%s \n", buf);
            // ...
            delete[] buf;
            buf = NULL;
        }
        Sleep(1000);
    }
#endif
    getchar();
    return 0;
}
#endif