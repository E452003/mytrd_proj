
#include "winner_app.h"

#include <qmessagebox.h>
#include <qdebug.h>
#include <QtWidgets/QApplication>
#include <qtimer.h>

#include <boost/lexical_cast.hpp>
#include <TLib/core/tsystem_utility_functions.h>

#include "common.h"
#include "db_moudle.h"

#include "breakdown_task.h"
#include "stock_ticker.h"
#include "task_factory.h"

#include "login_win.h"
#include "broker_cfg_win.h"
#include "message_win.h"

static bool SetCurrentEnvPath();

static const int cst_update_interval = 2000;  //ms :notice value have to be bigger than 1000

WinnerApp::WinnerApp(int argc, char* argv[])
    : QApplication(argc, argv)
    , ServerClientAppBase("client", "trade_winner", "0.1")
    , ticker_strand_(task_pool())
    , task_calc_strand_(task_pool())
    , stock_ticker_life_count(0)
    , trade_strand_(task_pool())
    , task_infos_(256)
    , msg_win_(new MessageWin())
    , winner_win_(this, nullptr)
    , login_win_(this)
    , exit_flag_(false)
    , trade_client_id_(-1)
    , cookie_()
    , db_moudle_(this)
    , user_info_()
    , trade_agent_()
    , strategy_tasks_timer_(std::make_shared<QTimer>())
    , normal_timer_(std::make_shared<QTimer>())
    , stocks_position_(256)
	, stk_quoter_moudle_(nullptr)
	, StkQuote_GetQuote(nullptr)
	, stocks_price_info_(256)
    , p_user_account_info_(nullptr)
    , p_user_broker_info_(nullptr)
{ 
    //msg_win_ = new MessageWin();

    connect(strategy_tasks_timer_.get(), SIGNAL(timeout()), this, SLOT(DoStrategyTasksTimeout()));
    connect(normal_timer_.get(), SIGNAL(timeout()), this, SLOT(DoNormalTimer()));

}

WinnerApp::~WinnerApp()
{
	if( stk_quoter_moudle_ ) 
		FreeLibrary(stk_quoter_moudle_);
    if( msg_win_ )
    {
        msg_win_->close();
        delete msg_win_;
    }
}

bool WinnerApp::Init()
{
    option_dir_type(AppBase::DirType::STAND_ALONE_APP);
	option_validate_app(false);

    std::string cur_dir(".//");
	work_dir(cur_dir);
    local_logger_.SetDir(cur_dir);

    int ret = 0;
    auto val_ret = cookie_.Init();
    if( val_ret != Cookie::TRetCookie::OK )
    {
        switch(Cookie::TRetCookie::ERROR_FILE_OPEN)
        {
        case Cookie::TRetCookie::ERROR_FILE_OPEN: QMessageBox::information(nullptr, "alert", QString::fromLocal8Bit("�����Ѿ���!")); break;
        default: QMessageBox::information(nullptr, "alert", QString::fromLocal8Bit("cookie�쳣!")); break;
        }
        return false;
    }
     
#if 1
    db_moudle_.Init();
    
#endif

#if 1
    login_win_.Init();
    ret = login_win_.exec();
    //login_win_.setWindowModality(Qt::ApplicationModal);
    if( ret != QDialog::Accepted )
    {
        Stop();
        return false;
    }

    db_moudle_.LoadAllUserBrokerInfo();

    p_user_account_info_ = db_moudle_.FindUserAccountInfo(user_info_.id);
    p_user_broker_info_ = db_moudle_.FindUserBrokerByUser(user_info_.id);
    assert(p_user_account_info_ && p_user_broker_info_);

#endif
#ifdef USE_TRADE_FLAG
    
    BrokerCfgWin  bcf_win(this);
    bcf_win.Init();
    ret = bcf_win.exec();
    if( ret != QDialog::Accepted )
    {
        Stop();
        return false;
    }
    
#endif

#ifdef USE_TRADE_FLAG
    //=========================trade account login =================
    //char error_info[256] = {0};
    Buffer result(1024);
    char error[1024] = {0};

    trade_agent_.QueryData(trade_client_id_, (int)TypeQueryCategory::SHARED_HOLDER_CODE, result.data(), error);
    if( strlen(error) != 0 )
    { 
        QMessageBox::information(nullptr, "alert", QString::fromLocal8Bit("��ѯ��Ȩ����ʧ��!"));
    }
    qDebug() << QString::fromLocal8Bit(result.data()) << "\n";
    local_logger().LogLocal(result.data());

    trade_agent_.SetupAccountInfo(result.data());
#endif
    //------------------------ create tasks ------------------
#if 0
    std::shared_ptr<StrategyTask> task0 = std::make_shared<BreakDownTask>(0, "000001", TypeMarket::SZ, this);

    std::shared_ptr<StrategyTask> task1 = std::make_shared<BreakDownTask>(1, "600030", TypeMarket::SH, this);
    std::shared_ptr<StrategyTask> task2 = std::make_shared<BreakDownTask>(2, "000959", TypeMarket::SZ, this);
    task1->is_to_run(true);
    task2->is_to_run(true);

    strategy_tasks_.push_back(std::move(task0));
    strategy_tasks_.push_back(std::move(task1));
    strategy_tasks_.push_back(std::move(task2));
#else

    db_moudle_.LoadAllTaskInfo(task_infos_);

    winner_win_.Init(); // inner use task_info
    winner_win_.show();
    bool ret1 = QObject::connect(this, SIGNAL(SigTaskStatChange(StrategyTask*, int)), &winner_win_, SLOT(DoTaskStatChangeSignal(StrategyTask*, int)));
    ret1 = QObject::connect(this, SIGNAL(SigRemoveTask(int)), &winner_win_, SLOT(RemoveByTaskId(int)));
    //ret1 = QObject::connect(this, SIGNAL(SigShowUi(std::shared_ptr<std::string>)), this, SLOT(DoShowUi(std::shared_ptr<std::string>)));
    ret1 = QObject::connect(this, SIGNAL(SigShowUi(std::string *)), this, SLOT(DoShowUi(std::string *)));
    
    TaskFactory::CreateAllTasks(task_infos_, strategy_tasks_, this);

#endif
     
#if 1 

    stock_ticker_ = std::make_shared<StockTicker>(this->local_logger());
    stock_ticker_->Init();

	//-----------------------StkQuoter-------------
    //SetCurrentEnvPath();
    char chBuf[1024] = {0};
    if( !::GetModuleFileName(NULL, chBuf, MAX_PATH) )  
       return false;   
   std::string strAppPath(chBuf);  

   auto nPos = strAppPath.rfind('\\');
   if( nPos > 0 )
   {   
       strAppPath = strAppPath.substr(0, nPos+1);  
   }  
   std::string stk_quote_full_path = strAppPath + "StkQuoter.dll";
   stk_quoter_moudle_ = LoadLibrary(stk_quote_full_path.c_str());
	if( !stk_quoter_moudle_ )
    {
        auto erro = GetLastError();
        QString info_str = QString("load %1 fail! error:%2").arg(stk_quote_full_path.c_str()).arg(erro);
        QMessageBox::information(nullptr, "info", info_str);
        //throw excepton;
        return false;
    }
	StkQuote_GetQuote = (StkQuoteGetQuoteDelegate)GetProcAddress(stk_quoter_moudle_, "StkQuoteGetQuote");
	assert(StkQuote_GetQuote);

    //-----------ticker main loop----------
    task_pool().PostTask([this]()
    {
        while(!this->exit_flag_)
        {
            ticker_strand_.PostTask([this]()
            {
                this->stock_ticker_->Procedure();
                this->stock_ticker_life_count  = 0;
            });

            Delay(cst_update_interval);
        }
        qDebug() << "out loop \n";
    });
    //----------------
#endif

    strategy_tasks_timer_->start(1000); //msec invoke DoStrategyTasksTimeout
    normal_timer_->start(2000); // invoke DoNormalTimeout
    return true;
}

void WinnerApp::Stop()
{
    exit_flag_ = true;
    //FireShutdown();
    Shutdown();
}

void WinnerApp::RemoveTask(unsigned int task_id)
{
    ticker_strand().PostTask([task_id, this]()
    {
        this->stock_ticker_->UnRegister(task_id);
        this->EmitSigRemoveTask(task_id); // invoke WinnerWin::RemoveByTaskId
        DelTaskById(task_id);

        /*auto strategy_task = this->FindStrategyTask(task_id);
        assert(strategy_task);
        this->Emit(strategy_task.get(), static_cast<int>(TaskStatChangeType::CUR_STATE_CHANGE));*/
    });
}

 int WinnerApp::Cookie_NextTaskId()
 {
     std::lock_guard<std::mutex> locker(cookie_mutex_);
     return ++ cookie_.data_->max_task_id;
 }

int WinnerApp::Cookie_MaxTaskId()
{
    std::lock_guard<std::mutex> locker(cookie_mutex_);
    return cookie_.data_->max_task_id;
}

void WinnerApp::Cookie_MaxTaskId(int task_id)
{
    std::lock_guard<std::mutex> locker(cookie_mutex_);
    cookie_.data_->max_task_id = task_id;
}

bool WinnerApp::LoginBroker(int broker_id, int depart_id, const std::string& account, const std::string& password)
{
    assert(trade_agent_.IsInited());

    char error_info[256] = {0};
     
    auto p_broker_info = db_moudle_.FindUserBrokerByBroker(broker_id);
    if( !p_broker_info) 
        return false;
    auto p_user_account_info = db_moudle_.FindUserAccountInfo(user_info_.id); 
   //assert(p_user_account_info && p_user_broker_info);

    /*trade_client_id_ = trade_agent_.Logon("122.224.113.121"
        , 7708, "2.20", 1, "32506627"
        , "32506627", "626261", "", error_info);*/
    //p_user_account_info->comm_pwd_;
    trade_client_id_ = trade_agent_.Logon(const_cast<char*>(p_broker_info->ip.c_str())
            , p_broker_info->port
            , const_cast<char*>(p_broker_info->com_ver.c_str())
            , 1
            , const_cast<char*>(account.c_str())
            , const_cast<char*>(account.c_str())  // default trade no is account no  
            , const_cast<char*>(password.c_str())
            , p_broker_info->type == TypeBroker::ZHONGYGJ ? password.c_str() : ""// communication password 
            , error_info);
    if( trade_client_id_ == -1 ) 
    {
        // QMessageBox::information(nullptr, "alert", "login fail!");
        return false;
    } 
    p_user_broker_info_ = p_broker_info;
    p_user_account_info_  = p_user_account_info;
    return true;
}

//QString WinnerApp::GetBrokerName(TypeBroker type)
//{
//    db_moudle_.FindBorkerIdByAccountID
//}

void WinnerApp::AppendTaskInfo(int id, std::shared_ptr<T_TaskInformation>& info)
{
    WriteLock  locker(task_infos_mutex_);
    if( task_infos_.find(id) != task_infos_.end() )
    {
        assert(false); 
        // log error
        return;
    }

    task_infos_.insert(std::make_pair(id, info));
}

void WinnerApp::AppendStrategyTask(std::shared_ptr<StrategyTask> &task)
{ 
    WriteLock  locker(strategy_tasks_mutex_);
    strategy_tasks_.push_back(task);
}

std::shared_ptr<T_TaskInformation> WinnerApp::FindTaskInfo(int task_id)
{
    std::shared_ptr<T_TaskInformation> p_info = nullptr;

    ReadLock locker(task_infos_mutex_);
    auto iter = task_infos_.find(task_id);
    if( iter != task_infos_.end() )
        return iter->second;
    return p_info;
}

bool WinnerApp::DelTaskById(int task_id)
{ 
    ticker_strand().PostTask([task_id, this]()
    {
        this->stock_ticker_->UnRegister(task_id);
    });

    {
    ReadLock  locker(task_infos_mutex_);
    auto iter = task_infos_.find(task_id);
    assert(iter != task_infos_.end());
    db_moudle().AddHisTask(iter->second);
    }

    // del database related records
    db_moudle().DelTaskInfo(task_id);

    WriteLock  locker(strategy_tasks_mutex_);
    for( auto iter = strategy_tasks_.begin(); iter != strategy_tasks_.end(); ++iter )
    {
        if( (*iter)->task_id() == task_id )
        {
            strategy_tasks_.erase(iter);
            return true;
        }
    }
    return false;
}

std::shared_ptr<StrategyTask> WinnerApp::FindStrategyTask(int task_id)
{
    ReadLock  locker(strategy_tasks_mutex_);
    for( auto iter = strategy_tasks_.begin(); iter != strategy_tasks_.end(); ++iter )
    {
        if( (*iter)->task_id() == task_id )
        { 
            return *iter;
        }
    }
    return nullptr;
}

std::unordered_map<std::string, T_PositionData> WinnerApp::QueryPosition()
{ 
    auto result = std::make_shared<Buffer>(5*1024);
     
    char error[1024] = {0};
#ifdef USE_TRADE_FLAG
    trade_agent_.QueryData(trade_client_id_, (int)TypeQueryCategory::STOCK, result->data(), error);
    if( strlen(error) != 0 )
    { 
        qDebug() << "query  fail! " << "\n";
        return std::unordered_map<std::string, T_PositionData>();
    }
    qDebug() << QString::fromLocal8Bit( result->data() ) << "\n";
#endif
    std::string str_result = result->c_data();
     qDebug() << " line 374" << " ----\n";
     qDebug() << str_result.c_str() << " ----\n";

    /*TSystem::utility::replace_all_distinct(str_result, "\n\t", "\t");
    TSystem::utility::replace_all_distinct(str_result, "\t\n", "\t");
    qDebug() << " line 378" << "\n";
    qDebug() << str_result.c_str() << " ----\n"; no effect*/

    TSystem::utility::replace_all_distinct(str_result, "\n", "\t");
    /*qDebug() << " line 382" << "\n";
    qDebug() << str_result.c_str() << " ----\n";*/
    auto result_array = TSystem::utility::split(str_result, "\t");
    
    std::lock_guard<std::mutex>  locker(stocks_position_mutex_);

    
    int start = 14;
    if( p_user_broker_info_->type == TypeBroker::ZHONGYGJ )
        start = 15;
    for( int n = 0; n < (result_array.size() - 14) / 13; ++n )
    {
        T_PositionData  pos_data;
        pos_data.code = result_array.at( start + n * 13);
        TSystem::utility::replace_all_distinct(pos_data.code, "\t", "");
        double qty_can_sell = 0;
        try
        {
           // ndchk
           pos_data.pinyin = result_array.at( start + n * 13 + 1);
           pos_data.total = boost::lexical_cast<double>(result_array.at( start + n * 13 + 2 ));
           pos_data.avaliable = boost::lexical_cast<double>(result_array.at(start + n * 13 + 3));
           pos_data.cost = boost::lexical_cast<double>(result_array.at(start + n * 13 + 4));
           pos_data.value = boost::lexical_cast<double>(result_array.at(start + n * 13 + 6));
           pos_data.profit = boost::lexical_cast<double>(result_array.at(start + n * 13 + 7));
           pos_data.profit_percent = boost::lexical_cast<double>(result_array.at(start + n * 13 + 8));

        }catch(boost::exception &e)
        { 
            continue;
        } 

        auto iter = stocks_position_.find(pos_data.code);
        if( iter == stocks_position_.end() )
        {
            stocks_position_.insert(std::make_pair(pos_data.code, std::move(pos_data)));
        }else
            iter->second = pos_data;
    }
    return stocks_position_;
}

T_PositionData* WinnerApp::QueryPosition(const std::string& code)
{
    QueryPosition();
    std::lock_guard<std::mutex>  locker(stocks_position_mutex_);
    auto iter = stocks_position_.find(code);
    if( iter == stocks_position_.end() )
    { 
        return nullptr;
    }
    return std::addressof(iter->second);
}

T_Capital WinnerApp::QueryCapital()
{
    T_Capital capital = {0};
      
    auto result = std::make_shared<Buffer>(5*1024);
     
    char error[1024] = {0};
#ifdef USE_TRADE_FLAG
    trade_agent_.QueryData(trade_client_id_, (int)TypeQueryCategory::CAPITAL, result->data(), error);
    if( strlen(error) != 0 )
    { 
        qDebug() << "query  fail! " << "\n";
    }
    qDebug() << QString::fromLocal8Bit( result->data() ) << "\n";
#endif
    std::string str_result = result->c_data();
    TSystem::utility::replace_all_distinct(str_result, "\n", "\t");
     
    auto result_array = TSystem::utility::split(str_result, "\t");
    if( result_array.size() < 13 )
        return capital;
    try
    {
     capital.remain = boost::lexical_cast<double>(result_array.at(11));
     capital.available = boost::lexical_cast<double>(result_array.at(12));
     capital.total = boost::lexical_cast<double>(result_array.at(15));
    }catch(boost::exception &e)
    { 
    }
     
    return capital;
}

T_StockPriceInfo * WinnerApp::GetStockPriceInfo(const std::string& code)
{
	char stocks[1][16];
	auto iter = stocks_price_info_.find(code);
	if( iter != stocks_price_info_.end() )
		return std::addressof(iter->second);

	strcpy_s(stocks[0], code.c_str());
#if 1
	T_StockPriceInfo price_info[1];
	auto num = StkQuote_GetQuote(stocks, 1, price_info);
	if( num < 1 )
		return nullptr;
#endif

    T_StockPriceInfo& info = stocks_price_info_.insert(std::make_pair(code, price_info[0])).first->second;
    return &info;
}

void WinnerApp::DoStrategyTasksTimeout()
{
    auto cur_time = QTime::currentTime();
    // register 
    ReadLock locker(strategy_tasks_mutex_);
    std::for_each( std::begin(strategy_tasks_), std::end(strategy_tasks_), [&cur_time, this](std::shared_ptr<StrategyTask>& entry)
    {
        /// qDebug() << "taskid" << entry->task_id() << "strategy_tasks_ size:" << strategy_tasks_.size() << " stock " << entry->stock_code() << " istorun:" << entry->is_to_run();
        if( entry->is_to_run() 
            && (entry->cur_state() == TaskCurrentState::STOP || entry->cur_state() == TaskCurrentState::WAITTING)
            && cur_time >= entry->tp_start() && cur_time <= entry->tp_end()
            )
        {
            this->ticker_strand_.PostTask([entry, this]()
            {
                this->stock_ticker_->Register(entry);
            });
        }else if( entry->cur_state() == TaskCurrentState::RUNNING
            && (cur_time < entry->tp_start() || cur_time > entry->tp_end())
            )
        {
             this->ticker_strand_.PostTask([entry, this]()
            {
                this->stock_ticker_->UnRegister(entry->task_id());
				this->Emit(entry.get(), static_cast<int>(TaskStatChangeType::CUR_STATE_CHANGE));
            });
        }
    });

}

//void WinnerApp::DoShowUi(std::shared_ptr<std::string> str)
void WinnerApp::DoShowUi(std::string* str)
{
    assert(str);
    msg_win().ShowUI("��ʾ", *str);
    delete str; str = nullptr;
}

void WinnerApp::DoNormalTimer()
{
    if( ++stock_ticker_life_count > 10 )
    {
        local_logger().LogLocal("thread stock_ticker procedure stop!");
    }
}


void WinnerApp::AppendLog2Ui(const char *fmt, ...)
{
	va_list ap;

	const int cst_buf_len = 1024;
	char szContent[cst_buf_len] = {0};
	char *p_buf = new char[cst_buf_len]; 
	memset(p_buf, 0, cst_buf_len);

	va_start(ap, fmt);
	vsprintf_s(szContent, sizeof(szContent), fmt, ap);
	va_end(ap);

	time_t rawtime;
	struct tm * timeinfo;
	time( &rawtime );
	timeinfo = localtime( &rawtime );

	sprintf_s( p_buf, cst_buf_len, "[%d %02d:%02d:%02d] %s \n"
        , (timeinfo->tm_year+1900)*10000 + (timeinfo->tm_mon+1)*100 + timeinfo->tm_mday
        , timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, szContent ); 

	emit SigAppendLog(p_buf); //will invoke SlotAppendLog
}

bool SetCurrentEnvPath()  
{  
   char chBuf[0x8000]={0};  
   DWORD dwSize = GetEnvironmentVariable("path", chBuf, 0x10000);  
   std::string strEnvPaths(chBuf);  
    
   if(!::GetModuleFileName(NULL, chBuf, MAX_PATH))  
       return false;   
   std::string strAppPath(chBuf);  

   auto nPos = strAppPath.rfind('\\');
   if( nPos > 0 )
   {   
       strAppPath = strAppPath.substr(0, nPos+1);  
   }  
    
   strEnvPaths += ";" + strAppPath +";";  
   bool bRet = SetEnvironmentVariable("path",strEnvPaths.c_str());  

   return bRet;  
}  
