/* 
section max(stop setion) if enter this section, then stop task
----------------------------->= max price---

-------------------------s---->= -----

-----------11.00---------s---->= ---

------------10.00------on---------represent price is 11.00; if < 11.00 && > 9.00 , it's in no op section

-------------9.00--------b--<= ----

section 2
---------------------b------<= ----
section 1
--------------------- < min price
section 0( clearing section) if enter this section, then clearing position

*)after trade filled,  reconstuct the sections 
*)section date is saved in db table's a filed, use # or * to devide
format: section0_type$section0_price#section1_type$section1_price
*/

#include "equal_section_task.h"

#include <TLib/core/tsystem_utility_functions.h>

#include "winner_app.h"

void EqualSectionTask::CalculateSections(double price, IN T_TaskInformation &task_info, OUT std::vector<T_SectionAutom> &sections)
{
	assert( price > 0.0);
	assert( task_info.secton_task.raise_percent > 0 && task_info.secton_task.fall_percent > 0 );
	assert( task_info.secton_task.fall_percent < 100 );
	assert( task_info.secton_task.min_trig_price <= task_info.secton_task.max_trig_price );

	sections.clear();
	// construct clearing section
	sections.emplace_back(TypeEqSection::CLEAR, task_info.secton_task.min_trig_price); // index 0

	// calculate buy_sec_num
	int buy_sec_num = 0;
	double temp_price = price * (100.00 - (buy_sec_num + 1) * task_info.secton_task.fall_percent) / 100.00;
	while( temp_price > task_info.secton_task.min_trig_price && temp_price > 0.01 + task_info.secton_task.min_trig_price )
	{
		++buy_sec_num;
		if( 100.00 < (buy_sec_num + 1) * task_info.secton_task.fall_percent )
			break;
		temp_price = price * (100.00 - (buy_sec_num + 1) * task_info.secton_task.fall_percent) / 100.00;
	}
	// construct buy sections 
	for( int i = buy_sec_num; i > 0; --i )
	{
		sections.emplace_back(TypeEqSection::BUY, price * (100.00 - i * task_info.secton_task.fall_percent) / 100.00); //index n
	}
	
	// construct noop section
	sections.emplace_back(TypeEqSection::NOOP, price * (100.00 + task_info.secton_task.raise_percent) / 100.00); 

	// calculate sell sections
	int sell_sec_num = 0;
	temp_price = price * (100.00 + (sell_sec_num + 1) * task_info.secton_task.raise_percent) / 100.00;
	while( temp_price < task_info.secton_task.max_trig_price && task_info.secton_task.max_trig_price > 0.01 + temp_price )
	{
		++sell_sec_num;
		temp_price = price * (100.00 + (sell_sec_num + 1) * task_info.secton_task.raise_percent) / 100.00;
	}
	// construct sell sections 
	for( int i = 0; i < sell_sec_num; ++i )
	{
		sections.emplace_back(TypeEqSection::SELL, price * (100.00 + (i + 1) * task_info.secton_task.raise_percent) / 100.00); //index n
	}

	// construct stop section
	sections.emplace_back(TypeEqSection::STOP, task_info.secton_task.max_trig_price); // index top
	 
}

void EqualSectionTask::TranslateSections(IN std::vector<T_SectionAutom> &sections, OUT std::string &sections_str)
{ 
	for( int i = 0; i < sections.size(); )
	{
		sections_str.append(utility::FormatStr("%d$", (int)sections[i].section_type));
		sections_str.append(utility::FormatStr("%.2f", sections[i].represent_price));
		++i;
		if( i < sections.size() )
			sections_str.append("#");
	}
}

EqualSectionTask::EqualSectionTask(T_TaskInformation &task_info, WinnerApp *app)
	: StrategyTask(task_info, app)
{ 
	CalculateSections(task_info.alert_price, task_info, sections_);
	 
}

void EqualSectionTask::HandleQuoteData()
{
	if( is_waitting_removed_ )
        return;

    assert( !quote_data_queue_.empty() );
    auto data_iter = quote_data_queue_.rbegin();
    std::shared_ptr<QuotesData> & iter = *data_iter;
    assert(iter);

	double pre_price = quote_data_queue_.size() > 1 ? (*(++data_iter))->cur_price : iter->cur_price;
      
	TypeOrderCategory order_type = TypeOrderCategory::SELL;
	int index = 0;
	for( ; index < sections_.size(); ++index )
	{
		switch(sections_[index].section_type)
		{
		case TypeEqSection::CLEAR:
			if( iter->cur_price < sections_[index].represent_price ) order_type = TypeOrderCategory::SELL; break;
		case TypeEqSection::BUY:
			if( iter->cur_price <= sections_[index].represent_price ) order_type = TypeOrderCategory::BUY; break;
		case TypeEqSection::SELL:
			if( iter->cur_price >= sections_[index].represent_price ) order_type = TypeOrderCategory::SELL; break;
		case TypeEqSection::NOOP:
			if( iter->cur_price < sections_[index].represent_price ) return; break;
		case TypeEqSection::STOP:
			if( iter->cur_price >= sections_[index].represent_price ) return; break;
		default: assert(false);break;
		}
	}
	
	app_->trade_strand().PostTask([&, this]()
    {
        char result[1024] = {0};
        char error_info[1024] = {0};
	            
        // to choice price to order
		const auto price = GetQuoteTargetPrice(*iter, order_type == TypeOrderCategory::BUY ? true : false);
#ifdef USE_TRADE_FLAG
        assert(this->app_->trade_agent().account_data(market_type_));

        //auto sh_hld_code  = const_cast<T_AccountData *>(this->app_->trade_agent().account_data(market_type_))->shared_holder_code;
        std::string cn_order_str = order_type == TypeOrderCategory::BUY ? "����" : "����";
		this->app_->local_logger().LogLocal(TagOfOrderLog(), 
			TSystem::utility::FormatStr("��������:%d %s %s �۸�:%.2f ����:%d ", para_.id, cn_order_str.c_str(), this->code_data(), price, para_.quantity)); 
        this->app_->AppendLog2Ui("��������:%d %s %s �۸�:%.2f ����:%d ", para_.id, cn_order_str.c_str(), this->code_data(), price, para_.quantity);
        
		// order the stock
        this->app_->trade_agent().SendOrder(this->app_->trade_client_id()
            , (int)order_type, 0
            , const_cast<T_AccountData *>(this->app_->trade_agent().account_data(market_type_))->shared_holder_code, this->code_data()
            , price, para_.quantity
            , result, error_info); 
#endif
        // judge result 
        if( strlen(error_info) > 0 )
        {
            auto ret_str = new std::string(utility::FormatStr("error %d %s %s %.2f %d error:%s"
                        , para_.id, cn_order_str.c_str(), para_.stock.c_str(), price, para_.quantity, error_info));
           this->app_->local_logger().LogLocal(TagOfOrderLog(), *ret_str);
           this->app_->AppendLog2Ui(ret_str->c_str());
           this->app_->EmitSigShowUi(ret_str);

        }else
        {
            auto ret_str = new std::string(utility::FormatStr("��������:%d %s %s %.2f %d �ɹ�!", para_.id, cn_order_str.c_str(), para_.stock.c_str(), price, para_.quantity));
            this->app_->EmitSigShowUi(ret_str);
        }

		CalculateSections(iter->cur_price, para_, sections_);
    });

}