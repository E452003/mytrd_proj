/*
* wwy: todo:ʹ�� qt��ȡ sqlit3 ���ݿ� ��ȡ��Ʊ����
*/

#include "flashingorder.h"
 
#include <windows.h>    
#include <TlHelp32.h>    
   
#include <locale.h>     
#include <stdio.h>

#include <Qdir.h>
#include <QSettings>
#include <QMessageBox>
#include <QDebug>

#include "gloakm_capture_api.h"

#include "dbmoudle.h"
#include "ticker.h"

#define WINDOW_TEXT_LENGTH 256    

#define MAIN_PROCESS_WIN_TAG "����֤ȯȪ��ͨ"  //"ͨ����"

char g_win_process_win_tag[256] = MAIN_PROCESS_WIN_TAG; // "����֤ȯȪ��ͨ";  

bool g_is_accept_order_ = false;

FlashingOrder& AppInstance()
{
	static FlashingOrder app;
	return app;
}

BOOL CALLBACK EnumChildWindowCallBack(HWND hWnd, LPARAM lParam)    
{    
	DWORD dwPid = 0;    
	GetWindowThreadProcessId(hWnd, &dwPid); // ����ҵ����������Ľ���    
	if(dwPid == lParam) // �ж��Ƿ���Ŀ����̵Ĵ���    
	{         
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

int CallBackFunc(BOOL is_buy, char *stock_name)
{
	qDebug() << "CallBackFunc: " << "\n";
	std::string title;
	std::string name;
	if( AppInstance().GetWinTileAndStockName(title, name) )
	{
		return 1;
		qDebug() << "CallBackFunc: todo " << (is_buy ? "buy " : "sell ") << name.c_str() << "\n";
		AppInstance().HandleOrder(is_buy, name);
	}
    return 0;
}

FlashingOrder::FlashingOrder(QWidget *parent)
	: QWidget(parent) 
	, stock_name2code_(1024*4)
	, trade_proxy_()
	, trade_client_id_(0)
	, ticker_(std::make_shared<Ticker>())
{
	ui.setupUi(this);
    //------------  
    //auto str0 = QDir::currentPath();
    QString iniFilePath = "flashingorder.ini";  
    QSettings settings(iniFilePath, QSettings::IniFormat);  
    QString app_title = QString::fromLocal8Bit(settings.value("config/broker_app_title").toString().toLatin1().constData());
    qDebug() << app_title << "\n";
    target_win_title_tag_ = app_title.toLocal8Bit();

	connect(&normal_timer_, SIGNAL(timeout()), this, SLOT(DoNormalTimer()));
}

FlashingOrder::~FlashingOrder()
{
	UnInstallLaunchEv();
}

bool FlashingOrder::Init(int argc, char *argv[])
{ 
    DBMoudle  dbobj(this);

	int broker_id = 3;
	int account_id = 3;
    if( !dbobj.Init(broker_id, account_id) )
	{
		// todo: msg error
		return false;
	}
	
	if( !ticker_->Init() )
	{
		// todo: msg error
		return false;
	}
	// tradeProxy init ----------------
	 
	if( !trade_proxy_.Setup(broker_info_.type, account_info_.account_no_in_broker_) )
		return false;

	assert(trade_proxy_.IsInited());
	Buffer result(1024);
	char error[1024] = {0};
	   
#if 1
	/*trade_client_id_ = trade_proxy_.Logon("122.224.113.121"
	, 7708, "2.20", 1, "32506627"
	, "32506627", "626261", "", error_info);*/
	//p_user_account_info->comm_pwd_;
#endif
	 trade_client_id_ = trade_proxy_.Logon(const_cast<char*>(broker_info_.ip.c_str())
		, broker_info_.port
		, const_cast<char*>(broker_info_.com_ver.c_str())
		, 1
		, const_cast<char*>(account_info_.account_no_in_broker_.c_str())
		, const_cast<char*>(account_info_.account_no_in_broker_.c_str())  // default trade no is account no  
		, const_cast<char*>(account_info_.trade_pwd_.c_str())
		, broker_info_.type == TypeBroker::ZHONGYGJ ? account_info_.trade_pwd_.c_str() : ""// communication password 
		, error);

	 trade_proxy_.QueryData(trade_client_id_, (int)TypeQueryCategory::SHARED_HOLDER_CODE, result.data(), error);
	 if( strlen(error) != 0 )
	 { 
		 QMessageBox::information(nullptr, "alert", QString::fromLocal8Bit("��ѯ��Ȩ����ʧ��!"));
		 return false;
	 }
	 qDebug() << QString::fromLocal8Bit(result.data()) << "\n";
	  
	 trade_proxy_.SetupAccountInfo(result.data());
	 // end --------------------------------

	 
#if 1
	//normal_timer_.start(2000);

   BOOL ret = InstallLaunchEv(CallBackFunc, const_cast<char*>(target_win_title_tag_.c_str()));
#else

#endif
	return true;
}

void FlashingOrder::HandleOrder(bool is_buy, const std::string &stock_name)
{
	assert(ticker_);
	 
#if 1
	Buffer result(1024);
	char error[1024] = {0};

	float price = 0.0;  
	
	int quantity = 100; // ndedt:
	auto iter = stock_name2code_.find(stock_name);
	if( iter == stock_name2code_.end() )
	{
		qDebug() << "FlashingOrder::HandleOrder cant find " << stock_name.c_str() << "\n";
		return;
	}
	QuotesData quote_data;
	if( !ticker_->GetQuotes(const_cast<char*>(iter->second.c_str()), quote_data) )
	{
		qDebug() << "FlashingOrder::HandleOrder GetQuotes fail " << stock_name.c_str() << "\n";
		return;
	} 

	trade_proxy_.SendOrder(trade_client_id_
		, (is_buy ? (int)TypeOrderCategory::BUY : (int)TypeOrderCategory::SELL)
		, 0
		, const_cast<T_AccountData *>( trade_proxy_.account_data( GetStockMarketType(iter->second.c_str()) ) )
		->shared_holder_code
		, const_cast<char*>(iter->second.c_str())
		, (is_buy ? quote_data.price_s_1 : quote_data.price_b_1)
		, quantity
		, result.data()
		, error); 
#endif
}

bool FlashingOrder::GetWinTileAndStockName(std::string& title, std::string& stock_name)
{
	HWND wnd = GetForegroundWindow(); 
	//HWND wnd1 = FindWindow(NULL, NULL);  // not ok
	char buf[1024] = {0};
	GetWindowText(wnd, buf, sizeof(buf)); // "����֤ȯȪ��ͨרҵ��V6.58 - [���ͼ-�й�ƽ��]" 
	qDebug() << "FlashingOrder::GetWinTileAndStockName wnd: "<< QString::fromLocal8Bit(buf) << "\n";
	
	/*char *pos = strstr(buf, "[���ͼ-");
	if( !pos )
	return false;*/

	title = buf; 
	std::string key_str = "[���ͼ-";

	auto pos = title.find(key_str);
	if( std::string::npos == pos )
		return false; 

	stock_name = title.substr(pos + key_str.length(), title.length() - pos - key_str.length() - 1 );
		/*
		if( strstr(buf, target_win_title_tag_.c_str()) )
		g_is_accept_order_ = true;
		else
		g_is_accept_order_ = false;*/
	return stock_name.length() > 0;
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