#include "flashingorder.h"
 
#include <windows.h>    
#include <TlHelp32.h>    
//#include <atlstr.h>    
#include <locale.h>     
#include <stdio.h>

#include <QDebug>

#define WINDOW_TEXT_LENGTH 256    

#define MAIN_PROCESS_WIN_TAG "����֤ȯȪ��ͨ"

BOOL CALLBACK EnumChildWindowCallBack(HWND hWnd, LPARAM lParam)    
{    
	DWORD dwPid = 0;    
	GetWindowThreadProcessId(hWnd, &dwPid); // ����ҵ����������Ľ���    
	if(dwPid == lParam) // �ж��Ƿ���Ŀ����̵Ĵ���    
	{    
		//printf("%d    ", hWnd); // ���������Ϣ    
		TCHAR buf[WINDOW_TEXT_LENGTH];    
		SendMessage(hWnd, WM_GETTEXT, WINDOW_TEXT_LENGTH, (LPARAM)buf);    
		//wprintf(L"%s/n", buf);    
		printf("%s \n", buf);
		qDebug() << "hwnd: "<< (int)hWnd << " " << QString::fromLocal8Bit(buf) << "\n";
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
		if( strstr(buf, MAIN_PROCESS_WIN_TAG) )
		{
			//wprintf(L"%s/n", buf);    
			qDebug() << "hwnd: " << (int)hWnd << "  " << QString::fromLocal8Bit(buf) << "\n";
			EnumChildWindows(hWnd, EnumChildWindowCallBack, lParam);    // ���������Ӵ��� 
			return FALSE;
		}else
			qDebug() << "hwnd: " << (int)hWnd << "  " << QString::fromLocal8Bit(buf) << "\n";
		//EnumChildWindows(hWnd, EnumChildWindowCallBack, lParam);    // ���������Ӵ���    
	}    
	return TRUE;    
}    

FlashingOrder::FlashingOrder(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	connect(&normal_timer_, SIGNAL(timeout()), this, SLOT(DoNormalTimer()));
}

FlashingOrder::~FlashingOrder()
{

}

bool FlashingOrder::Init()
{
	normal_timer_.start(2000);
	return true;
}

void FlashingOrder::DoNormalTimer()
{
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
}