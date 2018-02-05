// GloaKmCapture.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "GloaKmCapture.h"

#include "gloakm_capture_api.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: ����� DLL ����� MFC DLL �Ƕ�̬���ӵģ�
//		��Ӵ� DLL �������κε���
//		MFC �ĺ������뽫 AFX_MANAGE_STATE ����ӵ�
//		�ú�������ǰ�档
//
//		����:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �˴�Ϊ��ͨ������
//		}
//
//		�˺������κ� MFC ����
//		������ÿ��������ʮ����Ҫ������ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//

// CGloaKmCaptureApp

BEGIN_MESSAGE_MAP(CGloaKmCaptureApp, CWinApp)
END_MESSAGE_MAP()


HHOOK Hook;
LRESULT CALLBACK LauncherHook(int nCode,WPARAM wParam,LPARAM lParam);
void SaveLog(char* c);

CGloaKmCaptureApp::CGloaKmCaptureApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CGloaKmCaptureApp ����

CGloaKmCaptureApp theApp;


// CGloaKmCaptureApp ��ʼ��

BOOL CGloaKmCaptureApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}


extern "C"  DllExport void WINAPI InstallLaunchEv()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	//WH_KEYBOARD
	Hook = (HHOOK)SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)LauncherHook, theApp.m_hInstance, 0);
}

LRESULT CALLBACK LauncherHook(int nCode, WPARAM wParam, LPARAM lParam)
{
#define  PRE_CALL
#ifdef PRE_CALL
	LRESULT Result = CallNextHookEx(Hook,nCode,wParam,lParam);
#endif 
	char buf[1024] = {0};
	sprintf_s(buf, sizeof(buf), "nCode:%d wParam:%d\n\0", nCode, wParam);
	OutputDebugString(buf);
	//if(nCode == HC_ACTION)
	//{
	//	if(lParam & 0x80000000)
	//	{
	//		char c[1];
	//		c[0]=wParam;
	//		//SaveLog(c);
	//	}
	//} 
	KBDLLHOOKSTRUCT* pSturct = (KBDLLHOOKSTRUCT*)lParam;

	switch(wParam)
	{
	//case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
		switch(pSturct->vkCode)
		{
		case VK_LMENU:
			AfxMessageBox(_T("Left ALT"));
			break;

		case VK_RMENU:
			AfxMessageBox(_T("Right ALT"));
			break;
		}
		break;

	default:
		break;
	}
#ifdef PRE_CALL
	return Result;
#else
	return CallNextHookEx(Hook,nCode,wParam,lParam);
#endif
}