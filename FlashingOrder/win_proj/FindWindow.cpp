
#include <windows.h>    
#include <TlHelp32.h>    
//#include <atlstr.h>    
#include <locale.h>    
      
#include <stdio.h>

#define WINDOW_TEXT_LENGTH 256    
       
#define MAIN_PROCESS_WIN_TAG "����֤ȯȪ��ͨ"

   BOOL CALLBACK EnumChildWindowCallBack(HWND hWnd, LPARAM lParam)    
   {    
       DWORD dwPid = 0;    
       GetWindowThreadProcessId(hWnd, &dwPid); // ����ҵ����������Ľ���    
       if(dwPid == lParam) // �ж��Ƿ���Ŀ����̵Ĵ���    
       {    
           printf("%d    ", hWnd); // ���������Ϣ    
           TCHAR buf[WINDOW_TEXT_LENGTH];    
           SendMessage(hWnd, WM_GETTEXT, WINDOW_TEXT_LENGTH, (LPARAM)buf);    
           //wprintf(L"%s/n", buf);    
           printf("%s \n", buf);
           EnumChildWindows(hWnd, EnumChildWindowCallBack, lParam);    // �ݹ�����Ӵ���    
       }    
       return TRUE;    
   }    
       
   BOOL CALLBACK EnumWindowCallBack(HWND hWnd, LPARAM lParam)    
   {    
       DWORD dwPid = 0;    
       GetWindowThreadProcessId(hWnd, &dwPid); // ����ҵ����������Ľ���    
       if(dwPid == lParam) // �ж��Ƿ���Ŀ����̵Ĵ���    
       {    
           //printf("hwnd: %d ", (int)hWnd); // ���������Ϣ    
           TCHAR buf[WINDOW_TEXT_LENGTH];    
           SendMessage(hWnd, WM_GETTEXT, WINDOW_TEXT_LENGTH, (LPARAM)buf);    
		   printf("hwnd :%d %s\n", (int)hWnd, buf);
           if( strstr(buf, MAIN_PROCESS_WIN_TAG) )
           {
                //wprintf(L"%s/n", buf);   
                printf("hwnd :%d %s\n", (int)hWnd, buf);
                EnumChildWindows(hWnd, EnumChildWindowCallBack, lParam);    // ���������Ӵ��� 
                return FALSE;
           }
           //EnumChildWindows(hWnd, EnumChildWindowCallBack, lParam);    // ���������Ӵ���    
       }    
       return TRUE;    
   }    
       
int main()    
{    
    setlocale(LC_CTYPE, "chs");    

    DWORD targetPid = 0;    // ����id    
    PROCESSENTRY32 pe;  // ������Ϣ    
    pe.dwSize = sizeof(PROCESSENTRY32);    
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); // ���̿���    
    if(!Process32First(hSnapshot, &pe)) // �õ���һ�����̵Ŀ���    
        return 0;   

    bool is_find = false;
    do    
    {    
        if(!Process32Next(hSnapshot, &pe))    
            return 0;   
        is_find = strstr(pe.szExeFile, "TdxW.exe") != nullptr;
    } while( !is_find );

    //while (StrCmp(pe.szExeFile, L"QQ.exe"));  // ��������ֱ���Ҵ�Ŀ�����    
    if( is_find )
    {
        targetPid = pe.th32ProcessID;    
        // wprintf(L"Find QQ.exe process: 0x%08X/n", qqPid);    
        printf( "Find TdxW.exe process: 0x%08X/n", targetPid);    
        EnumWindows(EnumWindowCallBack, targetPid);   
    }

    getchar();
    return 0;    
}  

