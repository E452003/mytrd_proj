
#include "common.h"

#include <cctype>
#include <algorithm>
#include <regex>
 
#include <TLib/core/tsystem_time.h>
#include <TLib/core/tsystem_utility_functions.h>

QString ToQString(TypeTask val)
{
    switch(val)
    {
    case TypeTask::INFLECTION_BUY:
        return QString::fromLocal8Bit("�յ�����");
    case TypeTask::BREAKUP_BUY:
        return QString::fromLocal8Bit("ͻ������");
    case TypeTask::BATCHES_BUY:
        return QString::fromLocal8Bit("��������");
    case TypeTask::INFLECTION_SELL:
        return QString::fromLocal8Bit("�յ�����");
    case TypeTask::BREAK_SELL:
        return QString::fromLocal8Bit("��λ����");
    case TypeTask::FOLLOW_SELL:
        return QString::fromLocal8Bit("����ֹӯ");
    case TypeTask::BATCHES_SELL:
        return QString::fromLocal8Bit("��������");
    default: assert(0);
    }
    return "";
}

QString ToQString(TypeQuoteLevel val)
{
    switch(val)
    {
    case TypeQuoteLevel::PRICE_CUR:
        return QString::fromLocal8Bit("�ּ�");
    case TypeQuoteLevel::PRICE_BUYSELL_1:
        return QString::fromLocal8Bit("��һ����һ");
    case TypeQuoteLevel::PRICE_BUYSELL_2:
        return QString::fromLocal8Bit("���������");
    case TypeQuoteLevel::PRICE_BUYSELL_3:
        return QString::fromLocal8Bit("����������");
    case TypeQuoteLevel::PRICE_BUYSELL_4:
        return QString::fromLocal8Bit("���ĺ�����");
    case TypeQuoteLevel::PRICE_BUYSELL_5:
        return QString::fromLocal8Bit("���������");
    default: assert(0);
    }
    return "";
}

QString ToQString(TaskCurrentState val)
{
    switch(val)
    {
    case TaskCurrentState::STOP:
        return QString::fromLocal8Bit("ֹͣ");
    case TaskCurrentState::TORUN:
    case TaskCurrentState::WAITTING:
        return QString::fromLocal8Bit("�ȴ�");
    case TaskCurrentState::RUNNING:
        return QString::fromLocal8Bit("����");
    default: assert(0);
    }
    return "";
}

void Delay(unsigned short mseconds)
{
    //TSystem::WaitFor([]()->bool { return false;}, mseconds); // only make effect to timer
    std::this_thread::sleep_for(std::chrono::system_clock::duration(std::chrono::milliseconds(mseconds)));
}

bool compare(T_BrokerInfo &lh, T_BrokerInfo &rh)
{
     return lh.id < rh.id; 
}

bool Equal(double lh, double rh)
{
    return fabs(lh-rh) < 0.0001;
}

QTime Int2Qtime(int val)
{
    return QTime(val / 10000, (val % 10000) / 100, val % 100);
}

bool IsStrAlpha(const std::string& str)
{
	try
	{
		auto iter = std::find_if_not( str.begin(), str.end(), [](int val) 
		{ 
			if( val < 0 || val > 99999 ) 
				return 0;
			return isalpha(val);
		});
		 return iter == str.end();
	}catch(...)
	{
		return false;
	}
   
}

bool IsStrNum(const std::string& str)
{
	try
	{
		auto iter = std::find_if_not( str.begin(), str.end(), [](int val) 
		{ 
			if( val < 0 || val > 99999 ) 
				return 0;
			return isalnum(val);
		});
		return iter == str.end();
	}catch(...)
	{
		return false;
	}
    
}
 
std::string TagOfLog()
{
    return TSystem::utility::FormatStr("app_%d", TSystem::Today());
    //return  "OrderData_" + TSystem::Today()
}

std::string TagOfOrderLog()
{
    return TSystem::utility::FormatStr("OrderData_%d", TSystem::Today());
    //return  "OrderData_" + TSystem::Today()
}

std::tuple<int, std::string> CurrentDateTime()
{
    time_t rawtime;
    time(&rawtime);

    const int cst_buf_len = 256;
	char szContent[cst_buf_len] = {0};
	  
    struct tm * timeinfo = localtime(&rawtime);
    sprintf_s( szContent, cst_buf_len, "%02d:%02d:%02d"
				, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec ); 

    return std::make_tuple((timeinfo->tm_year + 1900) * 10000 + (timeinfo->tm_mon + 1) * 100 + timeinfo->tm_mday
                          , std::string(szContent));

}