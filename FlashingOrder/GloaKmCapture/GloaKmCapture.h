// GloaKmCapture.h : GloaKmCapture DLL ����ͷ�ļ�
#ifndef GLOAKM_CAPTURE_SFSD34_H_
#define GLOAKM_CAPTURE_SFSD34_H_

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������

 
  
class CGloaKmCaptureApp : public CWinApp
{
public:
	CGloaKmCaptureApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

#endif // GLOAKM_CAPTURE_SFSD34_H_