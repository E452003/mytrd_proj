#include "winner_win.h"

#include <qdebug.h>
#include <qmessagebox.h> 
#include <QtGui/QStandardItemModel>

#include <TLib/core/tsystem_utility_functions.h>
#include <TLib/core/tsystem_time.h>

//#include "mythread.h"
#include "winner_app.h"

#include "strategy_task.h"
#include "breakdown_task.h"

#include "stock_ticker.h"
#include "MySpinBox.h"
#include "HintList.h"

//static const QString cst_str_breakout_buy = QString::fromLocal8Bit("ͻ������");
//static const QString cst_str_Inflection_buy = QString::fromLocal8Bit("�յ�����");


static const unsigned short cst_col_count = 10;

static const int cst_tbview_tasks_rowindex_task_id = 0;
static const int cst_tbview_tasks_rowindex_state = 1;
static const int cst_tbview_tasks_rowindex_task_name = 2;
static const int cst_tbview_tasks_rowindex_stock_name = 3;
static const int cst_tbview_tasks_rowindex_cur_price = 4;
static const int cst_tbview_tasks_rowindex_trigger_price = 5;
static const int cst_tbview_tasks_rowindex_quantity = 6;
static const int cst_tbview_tasks_rowindex_price_level = 7;
static const int cst_tbview_tasks_rowindex_start_time = 8;
static const int cst_tbview_tasks_rowindex_end_time = 9;

static const int cst_tab_capital = 0;
static const int cst_tab_index_task_list = 1;
static const int cst_tab_index_sell_task = 2;
static const int cst_tab_index_buy_task = 3;
static const int cst_tab_index_log = 4;

WinnerWin::WinnerWin(WinnerApp *app, QWidget *parent)
    : QMainWindow(parent)
    , app_(app)
    , tbv_tasks_popMenu_(nullptr)
    , m_list_hint_(nullptr)
	, is_open_hint_(true)
    , m_bt_list_hint_(nullptr)
    , pre_close_price_(0.0)
    , buytask_pre_close_price_(0.0)
{
    ui.setupUi(this);

    connect(ui.pbtn_query_capital, SIGNAL(clicked()), this, SLOT(DoQueryCapital()));
#if 1
    //------------------tab task list 
    //QStandardItemModel * model = static_cast<QStandardItemModel *>(ui.tbview_tasks->model());
    QStandardItemModel * model = new QStandardItemModel(0, cst_col_count, this);
    model->setHorizontalHeaderItem(cst_tbview_tasks_rowindex_task_id, new QStandardItem(QString::fromLocal8Bit("�����")));
    model->horizontalHeaderItem(cst_tbview_tasks_rowindex_task_id)->setTextAlignment(Qt::AlignCenter);
    
    model->setHorizontalHeaderItem(cst_tbview_tasks_rowindex_state, new QStandardItem(QString::fromLocal8Bit("����״̬")));
    model->horizontalHeaderItem(cst_tbview_tasks_rowindex_state)->setTextAlignment(Qt::AlignCenter);
    
    model->setHorizontalHeaderItem(cst_tbview_tasks_rowindex_task_name, new QStandardItem(QString::fromLocal8Bit("��������")));
    model->horizontalHeaderItem(cst_tbview_tasks_rowindex_task_name)->setTextAlignment(Qt::AlignCenter);
    
    model->setHorizontalHeaderItem(cst_tbview_tasks_rowindex_stock_name, new QStandardItem(QString::fromLocal8Bit("��Ʊ")));
    model->horizontalHeaderItem(cst_tbview_tasks_rowindex_stock_name)->setTextAlignment(Qt::AlignCenter);
    
    model->setHorizontalHeaderItem(cst_tbview_tasks_rowindex_cur_price, new QStandardItem(QString::fromLocal8Bit("��ǰ�۸�")));
    model->horizontalHeaderItem(cst_tbview_tasks_rowindex_cur_price)->setTextAlignment(Qt::AlignCenter);
    
    model->setHorizontalHeaderItem(cst_tbview_tasks_rowindex_trigger_price, new QStandardItem(QString::fromLocal8Bit("�����۸�")));
    model->horizontalHeaderItem(cst_tbview_tasks_rowindex_trigger_price)->setTextAlignment(Qt::AlignCenter);
    
    model->setHorizontalHeaderItem(cst_tbview_tasks_rowindex_quantity, new QStandardItem(QString::fromLocal8Bit("��������")));
    model->horizontalHeaderItem(cst_tbview_tasks_rowindex_quantity)->setTextAlignment(Qt::AlignCenter);
    
    model->setHorizontalHeaderItem(cst_tbview_tasks_rowindex_price_level, new QStandardItem(QString::fromLocal8Bit("����ʹ�ü۸�")));
    model->horizontalHeaderItem(cst_tbview_tasks_rowindex_price_level)->setTextAlignment(Qt::AlignCenter);
    
    model->setHorizontalHeaderItem(cst_tbview_tasks_rowindex_start_time, new QStandardItem(QString::fromLocal8Bit("��ʼʱ��")));
    model->horizontalHeaderItem(cst_tbview_tasks_rowindex_start_time)->setTextAlignment(Qt::AlignCenter);
    
    model->setHorizontalHeaderItem(cst_tbview_tasks_rowindex_end_time, new QStandardItem(QString::fromLocal8Bit("����ʱ��")));
    model->horizontalHeaderItem(cst_tbview_tasks_rowindex_end_time)->setTextAlignment(Qt::AlignCenter);

    ui.tbview_tasks->setModel(model);
   /* ui.tbview_tasks->setColumnWidth(0, 100); */

    //------------------tab add sell task
    SetupSellTaskWin();
    InitSellTaskWin();

#endif
   
    //------------------tab add buy task------------ 
    InitBuyTaskWin();
    //----------------------------------
}

WinnerWin::~WinnerWin()
{
}

void WinnerWin::InsertIntoTbvTasklist(QTableView *tbv , T_TaskInformation &task_info)
{
    QStandardItemModel * model = static_cast<QStandardItemModel *>(tbv->model());
    model->insertRow(model->rowCount());
    int row_index = model->rowCount() - 1;
    auto align_way = Qt::AlignCenter;
    auto item = new QStandardItem( utility::FormatStr("%d", task_info.id).c_str() );
    model->setItem(row_index, cst_tbview_tasks_rowindex_task_id, item);
    model->item(row_index, cst_tbview_tasks_rowindex_task_id)->setTextAlignment(align_way);
    // run state
	item = new QStandardItem(ToQString(static_cast<TaskCurrentState>(task_info.state)));
    model->setItem(row_index, cst_tbview_tasks_rowindex_state, item);
    model->item(row_index, cst_tbview_tasks_rowindex_state)->setTextAlignment(align_way);
    // task name
    item = new QStandardItem(ToQString(task_info.type));
    model->setItem(row_index, cst_tbview_tasks_rowindex_task_name, item);
    model->item(row_index, cst_tbview_tasks_rowindex_task_name)->setTextAlignment(align_way);
    //  stock name 
	QString stock_str = QString("%1/%2").arg(task_info.stock.c_str()).arg(QString::fromLocal8Bit(task_info.stock_pinyin.c_str()));
    item = new QStandardItem(stock_str);
    model->setItem(row_index, cst_tbview_tasks_rowindex_stock_name, item);
    model->item(row_index, cst_tbview_tasks_rowindex_stock_name)->setTextAlignment(align_way);
    // current price
    item = new QStandardItem("");
    model->setItem(row_index, cst_tbview_tasks_rowindex_cur_price, item);
    model->item(row_index, cst_tbview_tasks_rowindex_cur_price)->setTextAlignment(align_way);
    // trigger price 
    item = new QStandardItem( utility::FormatStr("%.2f", task_info.alert_price).c_str() );
    model->setItem(row_index, cst_tbview_tasks_rowindex_trigger_price, item);
    model->item(row_index, cst_tbview_tasks_rowindex_trigger_price)->setTextAlignment(align_way);
    // quntity
    item = new QStandardItem( utility::FormatStr("%d", task_info.quantity).c_str() );
    model->setItem(row_index, cst_tbview_tasks_rowindex_quantity, item);
    model->item(row_index, cst_tbview_tasks_rowindex_quantity)->setTextAlignment(align_way);
    // price level
    item = new QStandardItem( ToQString(static_cast<TypeQuoteLevel>(task_info.target_price_level)) );
    model->setItem(row_index, cst_tbview_tasks_rowindex_price_level, item);
    model->item(row_index, cst_tbview_tasks_rowindex_price_level)->setTextAlignment(align_way);
    // start time
    item = new QStandardItem( utility::FormatStr("%d", task_info.start_time).c_str() );
    model->setItem(row_index, cst_tbview_tasks_rowindex_start_time, item);
    model->item(row_index, cst_tbview_tasks_rowindex_start_time)->setTextAlignment(align_way);
    // end time
    item = new QStandardItem( utility::FormatStr("%d", task_info.end_time).c_str() );
    model->setItem(row_index, cst_tbview_tasks_rowindex_end_time, item);
    model->item(row_index, cst_tbview_tasks_rowindex_end_time)->setTextAlignment(align_way);        
            
}
 
void WinnerWin::Init()
{
    //todo: load strategy tasks 
    bool ret = connect(this->app_, SIGNAL(SigAppendLog(char*)), this, SLOT(SlotAppendLog(char*)));
	connect(ui.tabwid_holder, SIGNAL(currentChanged(int)), this, SLOT(SlotTabChanged(int)));

    ui.tbview_tasks->setContextMenuPolicy(Qt::CustomContextMenu);
    ui.tbview_tasks->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.tbview_tasks->setSelectionMode(QAbstractItemView::SingleSelection);

    //set menu context of tableview tasklist -------------------
    auto action_start = new QAction (this);
    action_start ->setText ( QStringLiteral( "����" ));
    connect(action_start, SIGNAL(triggered(bool)), this, SLOT(SlotTbvTasksActionStart(bool)));

    auto action_stop = new QAction (this);
    action_stop ->setText ( QStringLiteral( "ֹͣ" ));
    connect(action_stop, SIGNAL(triggered(bool)), this, SLOT(SlotTbvTasksActionStop(bool)));

    auto action_del = new QAction (this);
    action_del ->setText ( QStringLiteral( "ɾ��" ));
    bool ret_con_del = connect(action_del, SIGNAL(triggered(bool)), this, SLOT(SlotTbvTasksActionDel(bool)));

    auto action_detail = new QAction (this);
    action_detail ->setText ( QStringLiteral( "�鿴����" ));
    connect(action_detail, SIGNAL(triggered(bool)), this, SLOT(SlotTbvTasksActionDetail(bool)));

    tbv_tasks_popMenu_ = new QMenu(ui.tbview_tasks);
    tbv_tasks_popMenu_->addAction(action_start);
    tbv_tasks_popMenu_->addAction(action_stop);
    tbv_tasks_popMenu_->addAction(action_del);
    
    tbv_tasks_popMenu_->addAction(action_detail);
    
    bool ret_of_con = connect(ui.tbview_tasks, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(SlotTbvTasksContextMenu(QPoint))); 
    //-----------------------------------------

    std::for_each( std::begin(app_->task_infos()), std::end(app_->task_infos()), [&, this](WinnerApp::T_IdMapTaskInfo::reference entry)
    {
        InsertIntoTbvTasklist(ui.tbview_tasks, *entry.second);
    });

    // load log content -------------------
    for(int i = 0; i < 10; ++i)
    {
        std::ifstream input_file(utility::FormatStr("trade_winner/trade_winner.OrderData_%d.%d.log", Today(), i));
        if( !input_file ) 
           break;
        // get length of file:
        input_file.seekg (0, input_file.end);
        int length = input_file.tellg();
        input_file.seekg (0, input_file.beg);

        // allocate memory:
        auto buf = std::make_shared<Buffer>(length);  
        // read data as a block:
        input_file.read (buf->data(), length);
        input_file.close();
     
        ui.pte_log->appendPlainText(QString::fromLocal8Bit(buf->c_data()));
    }
     
}

void WinnerWin::keyPressEvent(QKeyEvent *event)
{
    qDebug() << "WinnerWin::keyPressEvent"; 
    if( event->key() == Qt::Key_Escape )
    {
        if( ui.le_stock->hasFocus() )
        {
            m_list_hint_->close();
        }else if(ui.le_buytask_stock->hasFocus() )
        {
            m_bt_list_hint_->close();
        }
    }else
    {
        if( ui.le_stock->hasFocus() )
        {
            m_list_hint_->setFocus();
        }else if(ui.le_buytask_stock->hasFocus() )
        {
            m_bt_list_hint_->setFocus();
        }
    }
}


void WinnerWin::DoQueryCapital()
{ 
    ui.pte_capital->clear();
    std::unordered_map<std::string, T_PositionData> pos_map = app_->QueryPosition();
    T_Capital captital = app_->QueryCapital();
    ui.pte_capital->appendPlainText(QString::fromLocal8Bit("�ʽ�:\n"));
    ui.pte_capital->appendPlainText(QString::fromLocal8Bit(utility::FormatStr("���ʽ�:\t%.2f\n", captital.total).c_str())); 
    ui.pte_capital->appendPlainText(QString::fromLocal8Bit(utility::FormatStr("����:\t%.2f\n", captital.available).c_str()));
    ui.pte_capital->appendPlainText(QString::fromLocal8Bit(utility::FormatStr("���:\t%.2f\n", captital.remain).c_str()));

    //ui.pte_capital->appendPlainText(QString::fromLocal8Bit("�ʽ�:\n"));

    ui.pte_capital->appendPlainText(QString::fromLocal8Bit("��Ʊ: \t\t����\t����\t����ӯ��\tӯ������\n"));
    std::for_each(std::begin(pos_map), std::end(pos_map), [this]( std::unordered_map<std::string, T_PositionData>::reference entry)
    {  
        auto str = QString::fromLocal8Bit(
        utility::FormatStr("%s\t%s\t%d\t%d\t%.2f\t%.2f\n", entry.first.c_str(), app_->db_moudle().GetStockName(entry.first).c_str()
        , entry.second.total, entry.second.avaliable, entry.second.profit, entry.second.profit_percent).c_str()  );
        qDebug() << str << " -------\n";
        ui.pte_capital->appendPlainText(str);
         
    });
}

void WinnerWin::SlotAppendLog(char *p_chars)
{
    ui.pte_log->appendPlainText(QString::fromLocal8Bit(p_chars));
    delete p_chars; p_chars = nullptr;
}

void WinnerWin::SlotTabChanged(int /*index*/)
{
	assert(m_list_hint_ && m_bt_list_hint_);
	m_list_hint_->hide();
    m_bt_list_hint_->hide();
}

void WinnerWin::SlotTbvTasksContextMenu(QPoint p)
{
    QModelIndex pmindex = ui.tbview_tasks->indexAt(p);
    if( pmindex.isValid() )
    {
        tbv_tasks_popMenu_->exec(QCursor::pos());
    }
    ui.tbview_tasks->currentIndex();
}

void WinnerWin::SlotTbvTasksActionStart(bool)
{
    // todo:
    auto task_id = TbvTasksCurRowTaskId();

    /*auto iter = app_->task_infos().find(task_id);
    assert(iter != app_->task_infos().end());*/
     
    auto strategy_task = app_->FindStrategyTask(task_id); 
    assert(strategy_task); 
    strategy_task->SetOriginalState(TaskCurrentState::TORUN);

    app_->db_moudle().UpdateTaskInfo(strategy_task->task_info());
     
    app_->ticker_strand().PostTask([strategy_task, this]()
    {
        this->app_->stock_ticker().Register(strategy_task);
    });
}

void WinnerWin::SlotTbvTasksActionStop(bool)
{
    QStandardItemModel *model = static_cast<QStandardItemModel *>(ui.tbview_tasks->model());
    QModelIndex mod_index = ui.tbview_tasks->currentIndex();
    auto task_id = model->item(mod_index.row(), cst_tbview_tasks_rowindex_task_id)->text().toInt();
    auto strategy_task = app_->FindStrategyTask(task_id);
    assert(strategy_task);
    strategy_task->SetOriginalState(TaskCurrentState::STOP);
    app_->db_moudle().UpdateTaskInfo(strategy_task->task_info());

    auto p_strategy_task = strategy_task.get();
    app_->ticker_strand().PostTask([p_strategy_task, task_id, this]()
    {
        this->app_->stock_ticker().UnRegister(task_id);
        this->app_->Emit(p_strategy_task, static_cast<int>(TaskStatChangeType::CUR_STATE_CHANGE));
    });

    model->submit();
    update();

}

void WinnerWin::SlotTbvTasksActionDel(bool)
{
    QStandardItemModel *model = static_cast<QStandardItemModel *>(ui.tbview_tasks->model());
    QModelIndex mod_index = ui.tbview_tasks->currentIndex();
    auto task_id = model->item(mod_index.row(), cst_tbview_tasks_rowindex_task_id)->text().toInt();
    
    app_->DelTaskById(task_id);

    model->removeRow(mod_index.row());
    model->submit();
    update();
    return;
}

void WinnerWin::SlotTbvTasksActionDetail(bool)
{    
    auto task_id = TbvTasksCurRowTaskId();

    auto p_tskinfo = app_->FindTaskInfo(task_id);
    if( !p_tskinfo )
        return;

	is_open_hint_ = false;
     
    switch(p_tskinfo->type)
    {
    case TypeTask::BREAK_SELL: 
    case TypeTask::INFLECTION_SELL:
    case TypeTask::FOLLOW_SELL:
    case TypeTask::BATCHES_SELL:
        pre_close_price_ = 0.0;
        FillSellTaskWin(p_tskinfo->type, *p_tskinfo);
		ui.tabwid_holder->setCurrentIndex(cst_tab_index_sell_task);
        break;
    case TypeTask::BREAKUP_BUY:
    case TypeTask::INFLECTION_BUY:
    case TypeTask::BATCHES_BUY:
        buytask_pre_close_price_ = 0.0;
        FillBuyTaskWin(p_tskinfo->type, *p_tskinfo);
		ui.tabwid_holder->setCurrentIndex(cst_tab_index_buy_task);
        break;
    }
     

EXIT_PROC:

	is_open_hint_ = true;
}

void WinnerWin::RemoveByTaskId(int task_id)
{ 
    QStandardItemModel *model = static_cast<QStandardItemModel *>(ui.tbview_tasks->model());
    int i = 0;
    for( ; i < model->rowCount(); ++i )
    {
        if( task_id == model->item(i, cst_tbview_tasks_rowindex_task_id)->text().toInt() )
           break;
    }
    if( i < model->rowCount() )
    {
        model->removeRow(i);
        model->submit();
        update();
    }
    
}

void WinnerWin::DoLeStockEditingFinished()
{
    if( !ui.le_stock->isModified() )
        return;

    auto val = ui.le_stock->isModified();
    ui.le_stock->setModified(false);
    //ui.te_stock_match->hide();

#if 0
    auto ckval =  ui.le_stock->text();
    auto stock_code = ui.le_stock->text().trimmed();
    if( stock_code.length() != 6 )
        return;

    byte category = 9;
    int market = stock_code.at(0) == "6" ? 1 : 0;

    auto zqdm = stock_code.toLocal8Bit().data();

    //char  result[1024*1024] = {0};
    auto result = std::make_shared<Buffer>(1024*1024);
    char error[2*1024] = {0};
    short count = 1;
    app_->stock_ticker().GetSecurityBars(category, market, stock_code.toLocal8Bit().data(), 0, count, result->data(), error);
#endif
}

void WinnerWin::DoLeStockChanged(const QString &str)
{
    if( str.length() > 0 )
    {
        /*auto temp = ui.le_stock->text();
        ui.te_stock_match->append(temp);

        std::vector<T_StockCodeName> stockCodeNames;
        app_->db_moudle().GetStockCode(str.toLocal8Bit().data(), stockCodeNames);*/
    }
}

void WinnerWin::DoSpboxQuantityEditingFinished()
{
    
    //ui.spinBox_quantity->mo
}

void WinnerWin::closeEvent(QCloseEvent * event)
{
    app_->Stop();
}

void WinnerWin::DoTaskStatChangeSignal(StrategyTask* p_task, int val)
{
    assert(p_task);

    switch(static_cast<TaskStatChangeType>(val))
    {
    case TaskStatChangeType::CUR_PRICE_CHANGE:
        {
            for( int i = 0; i < ui.tbview_tasks->model()->rowCount(); ++i )
            {
                if( p_task->task_id() == static_cast<QStandardItemModel *>(ui.tbview_tasks->model())->item(i, cst_tbview_tasks_rowindex_task_id)->text().toUInt() ) 
                {
                    QStandardItemModel *model = static_cast<QStandardItemModel *>(ui.tbview_tasks->model());
                    model->item(i, cst_tbview_tasks_rowindex_cur_price)->setText(utility::FormatStr("%.2f", p_task->cur_price()).c_str());
                    return;
                }
            }
            break;
        }
    case TaskStatChangeType::CUR_STATE_CHANGE:
        {
            for( int i = 0; i < ui.tbview_tasks->model()->rowCount(); ++i )
            {
                if( p_task->task_id() == static_cast<QStandardItemModel *>(ui.tbview_tasks->model())->item(i, cst_tbview_tasks_rowindex_task_id)->text().toUInt() ) 
                {
                    QStandardItemModel *model = static_cast<QStandardItemModel *>(ui.tbview_tasks->model());
                    model->item(i, cst_tbview_tasks_rowindex_state)->setText(ToQString(p_task->cur_state()));
                    return;
                }
            }
            break;
        }
    }

}

void WinnerWin::DoQueryPosition()
{
    QString text = ui.le_stock->text();
    QString::SectionFlag flag = QString::SectionSkipEmpty;
    QString tgt_tag = text.section('/', 0, 0, flag);

    if( IsStrNum(tgt_tag.toLocal8Bit().data()) && tgt_tag.length() == 6 )
    {
        auto pos = app_->QueryPosition(tgt_tag.toLocal8Bit().data());
        if( pos )
        {
            ui.label_total_num->setText(QString("%1").arg(pos->avaliable));
            ui.spinBox_quantity->setValue(pos->avaliable);
        }
    }
}

int WinnerWin::TbvTasksCurRowTaskId()
{
    QStandardItemModel *model = static_cast<QStandardItemModel *>(ui.tbview_tasks->model());
    return model->item(ui.tbview_tasks->currentIndex().row(), cst_tbview_tasks_rowindex_task_id)->text().toInt();

}

void WinnerWin::FlushFromStationListWidget(QString str)
{
	if( !is_open_hint_ )
		return;
     
    HintList *p_list = nullptr;
    QLineEdit *p_edit = nullptr;
    QDoubleSpinBox *p_dbspinbox = nullptr;
    AssignHintListAndLineEdit(p_list, p_edit, p_dbspinbox);

    p_list->clear();
	 
    //qDebug() << "FlushFromStationListWidget " << str << "\n";

    std::vector<T_StockCodeName> stockCodeNames;
    app_->db_moudle().GetStockCode(str.toLocal8Bit().data(), stockCodeNames);
    std::for_each( std::begin(stockCodeNames), std::end(stockCodeNames), [p_list, this](T_StockCodeName& entry)
    { 
        QString tmp_str = QString("%1/%2").arg(entry.code.c_str()).arg(QString::fromLocal8Bit(entry.name.c_str()));
        p_list->addItem(tmp_str);
    });
    if( p_list->count() > 0 )
    {
        int x,y, w,h;
        p_edit->geometry().getRect(&x, &y, &w, &h);
    
        p_list->setGeometry( x + 4, y + 90, w, h + 40);
        p_list->setCurrentRow(0);
        p_list->ShowUI();
    }
} 

void WinnerWin::OnClickedListWidget(QModelIndex index)
{
    HintList *p_list = nullptr;
    QLineEdit *p_edit = nullptr;
    QDoubleSpinBox *p_dbspinbox = nullptr;
    AssignHintListAndLineEdit(p_list, p_edit, p_dbspinbox);

    QString text = p_list->item(index.row())->text();
    ChangeFromStationText(text);

   /* QString::SectionFlag flag = QString::SectionSkipEmpty;
    QString tgt_tag = text.section('/', 0, 0, flag);
     
    qDebug() << "OnClickedListWidget " << tgt_tag << "\n";

    p_edit->setText(tgt_tag);
    p_list->hide();
     
	T_StockPriceInfo *p_info = app_->GetStockPriceInfo(tgt_tag.toLocal8Bit().data());
	if( p_info )
    {
        p_dbspinbox->setValue(p_info->pre_close_price);
    }*/
}

void WinnerWin::ChangeFromStationText(QString text)
{
    HintList *p_list = nullptr;
    QLineEdit *p_edit = nullptr;
    QDoubleSpinBox *p_dbspinbox = nullptr;
    AssignHintListAndLineEdit(p_list, p_edit, p_dbspinbox);
     
    p_edit->setText(text);
    p_list->hide();

    //qDebug() << "ChangeFromStationText " << tgt_tag << "\n";
    QString::SectionFlag flag = QString::SectionSkipEmpty;
    QString tgt_tag = text.section('/', 0, 0, flag);
	T_StockPriceInfo *p_info = app_->GetStockPriceInfo(tgt_tag.toLocal8Bit().data());
	if( p_info )
    {
        p_dbspinbox->setValue(p_info->pre_close_price);
        if( ui.tabwid_holder->currentIndex() == cst_tab_index_sell_task )
            pre_close_price_ = p_info->pre_close_price;
        else
            buytask_pre_close_price_ = p_info->pre_close_price;
    }else
    {
        if( ui.tabwid_holder->currentIndex() == cst_tab_index_sell_task )
            pre_close_price_ = 0.0;
        else
            buytask_pre_close_price_ = 0.0;
    }
}

void WinnerWin::AssignHintListAndLineEdit(HintList *& p_list, QLineEdit *&p_edit, QDoubleSpinBox *&p_dbspinbox)
{
    if( ui.tabwid_holder->currentIndex() == cst_tab_index_sell_task )
    {
        p_list = m_list_hint_;
        p_edit = ui.le_stock;
        p_dbspinbox = ui.dbspbox_alert_price;
    }else if( ui.tabwid_holder->currentIndex() == cst_tab_index_buy_task )
    {   
        p_list = m_bt_list_hint_;
        p_edit = ui.le_buytask_stock;
        p_dbspinbox = ui.dbspbox_buytask_alert_price;
    }else
        assert(false);
}
