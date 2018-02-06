/*
* wwy: todo:ʹ�� qt��ȡ sqlit3 ���ݿ� ��ȡ��Ʊ����
*/

#include "flashingorder.h"
 
#include <windows.h>    
#include <TlHelp32.h>    
   
#include <locale.h>     
#include <stdio.h>

#include <QDebug>

#include "gloakm_capture_api.h"

#define WINDOW_TEXT_LENGTH 256    

#define MAIN_PROCESS_WIN_TAG "����֤ȯȪ��ͨ"

char g_win_process_win_tag[256] = "����֤ȯȪ��ͨ";

bool g_is_accept_order_ = false;

BOOL CALLBACK EnumChildWindowCallBack(HWND hWnd, LPARAM lParam)    
{    
	DWORD dwPid = 0;    
	GetWindowThreadProcessId(hWnd, &dwPid); // ����ҵ����������Ľ���    
	if(dwPid == lParam) // �ж��Ƿ���Ŀ����̵Ĵ���    
	{    
		//printf("%d    ", hWnd);     
		TCHAR buf[WINDOW_TEXT_LENGTH];    
		SendMessage(hWnd, WM_GETTEXT, WINDOW_TEXT_LENGTH, (LPARAM)buf);    
		//wprintf(L"%s/n", buf);    
		printf("%s \n", buf);
		qDebug() << "hwnd: "<< (int)hWnd << " " << QString::fromLocal8Bit(buf) << "\n";
		EnumChildWindows(hWnd, EnumChildWindowCallBack, lParam);     
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
		if( strstr(buf, MAIN_PROCESS_WIN_TAG) )
		{
			//wprintf(L"%s/n", buf);    
			qDebug() << "hwnd: " << (int)hWnd << "  " << QString::fromLocal8Bit(buf) << "\n";
			EnumChildWindows(hWnd, EnumChildWindowCallBack, lParam);    // ���������Ӵ��� 
			return FALSE;
		}else
			qDebug() << "hwnd: " << (int)hWnd << "  " << QString::fromLocal8Bit(buf) << "\n";
		//EnumChildWindows(hWnd, EnumChildWindowCallBack, lParam);        
	}    
	return TRUE;    
}    

int CallBackFunc(BOOL is_buy, char *stock)
{
	qDebug() << "CallBackFunc: " << "\n";
	if( g_is_accept_order_ )
	{
		// todo:
		qDebug() << "CallBackFunc: todo " << (is_buy ? "buy " : "sell ") << stock << "\n";
	}
    return 0;
}

FlashingOrder::FlashingOrder(QWidget *parent)
	: QWidget(parent) 
{
	ui.setupUi(this);
	target_win_title_tag_ = MAIN_PROCESS_WIN_TAG;
	connect(&normal_timer_, SIGNAL(timeout()), this, SLOT(DoNormalTimer()));
}

FlashingOrder::~FlashingOrder()
{
	UnInstallLaunchEv();
}

bool FlashingOrder::Init()
{ 
#if 1
	normal_timer_.start(2000);

   BOOL ret = InstallLaunchEv(CallBackFunc, const_cast<char*>(target_win_title_tag_.c_str()));
#else

#endif
	return true;
}

void FlashingOrder::DoNormalTimer()
{
#if 1
     HWND wnd = GetForegroundWindow(); 
    //HWND wnd1 = FindWindow(NULL, NULL);  // not ok
    char buf[1024] = {0};
    GetWindowText(wnd, buf, sizeof(buf)); // "����֤ȯȪ��ͨרҵ��V6.58 - [���ͼ-�й�ƽ��]" 
    qDebug() << "FlashingOrder::DoNormalTimer wnd: "<< QString::fromLocal8Bit(buf) << "\n";

	if( strstr(buf, target_win_title_tag_.c_str()) )
		g_is_accept_order_ = true;
	else
		g_is_accept_order_ = false;

#else
	setlocale(LC_CTYPE, "chs");    

	DWORD targetPid = 0;    // ����id    
	PROCESSENTRY32 pe;  // ������Ϣ    
	pe.dwSize = sizeof(PROCESSENTRY32);    
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); // ���̿���    
	if(!Process32First(hSnapshot, &pe)) // �õ���һ�����̵Ŀ���    
		return;   

	bool is_find = false;
	do    
	{    
		if(!Process32Next(hSnapshot, &pe))    
			return;   
		is_find = strstr(pe.szExeFile, "TdxW.exe") != nullptr;
	} while( !is_find );

	//while (StrCmp(pe.szExeFile, L"QQ.exe"));  // ��������ֱ���Ҵ�Ŀ�����    
	if( is_find )
	{
		targetPid = pe.th32ProcessID;    
		// wprintf(L"Find QQ.exe process: 0x%08X/n", qqPid);    
		//printf( "Find TdxW.exe process: 0x%08X/n", targetPid);  
		qDebug() << "Find TdxW.exe process: "<< targetPid << "\n";
		EnumWindows(EnumWindowCallBack, targetPid);   
	}
#endif
}