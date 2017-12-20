#include "index_task.h"

#include <TLib/core/tsystem_utility_functions.h>

#include "winner_app.h"

IndexTask::IndexTask(T_TaskInformation &task_info, WinnerApp *app)
    : StrategyTask(task_info, app)
    , app_(app)
{

}

void IndexTask::HandleQuoteData()
{
    if( is_waitting_removed_ )
        return;
    assert( !quote_data_queue_.empty() );
    auto data_iter = quote_data_queue_.rbegin();
    std::shared_ptr<QuotesData> & iter = *data_iter;
    assert(iter);

    double pre_price = quote_data_queue_.size() > 1 ? (*(++data_iter))->cur_price : iter->cur_price;

    bool is_trigger = para_.index_rel_task.is_down_trigger ? iter->cur_price < para_.alert_price : iter->cur_price > para_.alert_price;

    if( iter->cur_price > 0 && is_trigger )
    {
        if( time_point_open_warning_ != 0 )
        {
			if( iter->time_stamp - time_point_open_warning_  >= para_.continue_second )
            {
				const auto time_span = iter->time_stamp - time_point_open_warning_;
                time_point_open_warning_ = 0; //reset
                is_waitting_removed_ = true;
                
                app_->trade_strand().PostTask([iter, time_span, this]()
                {
                    std::string *str = nullptr;
                    std::string str_header = utility::FormatStr("任务:%d %s %s %.2f "
                            , para_.id, para_.stock.c_str(), (para_.index_rel_task.is_down_trigger ? "下穿" : "上穿"), this->para_.alert_price);
                    switch( para_.index_rel_task.rel_type )
                    {
                    case TindexTaskType::ALERT:
                    { 
                    }
                    break;
                    case TindexTaskType::RELSTOCK:
                    {
                        str = new std::string(str_header + utility::FormatStr("执行指数关联股票:%s", this->para_.index_rel_task.stock_code.c_str() ));
                    }
                    break;
                    case TindexTaskType::CLEAR:
                    {
                        str = new std::string(str_header + "清仓");
                    }
                    break;
                    default: assert(0);break;
                    }
                    this->app_->local_logger().LogLocal(TagOfOrderLog(), *str);
                    this->app_->AppendLog2Ui(str->c_str());
                    this->app_->EmitSigShowLongUi(str, true);
                });

                

                this->app_->RemoveTask(this->task_id(), TypeTask::INDEX_RISKMAN);
            }
        }
    }
}
