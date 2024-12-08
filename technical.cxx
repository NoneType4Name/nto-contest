#include "common.hxx"
#include "technical.hxx"
#include "ui_technical.h"
#include <QDateTime>
#include <format>
#include <qcolor.h>
#include <qcombobox.h>
#include <qdatetime.h>
#include <qnamespace.h>
#include <string>

Technical::Technical( QWidget *parent ) : QWidget( parent ),
                                          ui( new Ui::Technical )
{
    ui->setupUi( this );
    ui->table->horizontalHeader()->setStretchLastSection( 1 );
    ui->table->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch );
    ui->splitter->setStretchFactor( 0, 3 );
    ui->splitter->setStretchFactor( 1, 1 );
    sqlite3_exec( database, "SELECT name FROM manufactories;", []( void *data, int argc, char **argv, char **szColName ) -> int
                  {auto _cb{static_cast<QComboBox*>(data)};
                  _cb->addItem(argv[0]);
                     return 0; }, ui->comboBox, 0 );
    ui->StatusLabel->hide();
    ui->widget->hide();
    reloadTasksTable();
}

Technical::~Technical()
{
    delete ui;
}

void Technical::reloadTasksTable() // todo: collarize
{
    ui->table->setRowCount( 0 );
    if( ui->comboBox->currentIndex() )
        sqlite3_exec( database, std::format( "SELECT * FROM prepareTasks AS task WHERE task.workplaceID in (SELECT id FROM workplaces WHERE manufactoryID = (SELECT id FROM manufactories WHERE name = '{}'));", ui->comboBox->currentText().toStdString() ).c_str(), []( void *data, int argc, char **argv, char **szColName ) -> int
                      {
                        auto _ui{ static_cast<Ui::Technical *>( data ) };
                        QDateTime msProdStart;
                        sqlite3_exec( database, std::format( "SELECT startdate FROM productTasks WHERE id = {};", argv[1] ).c_str(), []( void *data, int argc, char **argv, char **szColName ) -> int
                          { 
                    *static_cast<QDateTime *>( data ) = QDateTime::fromMSecsSinceEpoch( std::stoull( argv[ 0 ] ) );
                    return 0; }, &msProdStart, 0 );
                        _ui->table->insertRow( _ui->table->rowCount() );
                        _ui->table->setItem( _ui->table->rowCount() - 1, 0, new QTableWidgetItem{ argv[ 0 ] } );
                        _ui->table->setItem( _ui->table->rowCount() - 1, 1, new QTableWidgetItem{ QDateTime::fromMSecsSinceEpoch( std::stoull( argv[ 2 ] ) ).toString( "dd-MM-yyyy" ) } );
                        _ui->table->setItem( _ui->table->rowCount() - 1, 2, new QTableWidgetItem{ QDateTime::fromMSecsSinceEpoch( std::stoull( argv[ 3 ] ) ).toString( "dd-MM-yyyy" ) } );
                        _ui->table->setItem( _ui->table->rowCount() - 1, 3, new QTableWidgetItem{ argv[ 5 ] } );
                        if( msProdStart.date()  == QDateTime::fromMSecsSinceEpoch( std::stoull( argv[ 2 ] ) ).date() )
                        {
                            _ui->table->item( _ui->table->rowCount() - 1, 0 )->setBackground( Qt::red );
                            _ui->table->item( _ui->table->rowCount() - 1, 1 )->setBackground( Qt::red );
                            _ui->table->item( _ui->table->rowCount() - 1, 2 )->setBackground( Qt::red );
                            _ui->table->item( _ui->table->rowCount() - 1, 3 )->setBackground( Qt::red );
                        }
                        else if( msProdStart.date() == QDateTime::fromMSecsSinceEpoch( std::stoull( argv[ 2 ] ) ).addDays( 1 ).date() )
                        {
                            _ui->table->item( _ui->table->rowCount() - 1, 0 )->setBackground( Qt::yellow );
                            _ui->table->item( _ui->table->rowCount() - 1, 1 )->setBackground( Qt::yellow );
                            _ui->table->item( _ui->table->rowCount() - 1, 2 )->setBackground( Qt::yellow );
                            _ui->table->item( _ui->table->rowCount() - 1, 3 )->setBackground( Qt::yellow );
                        }
                        return 0; }, ui, 0 );
    else
        sqlite3_exec( database, "SELECT * FROM prepareTasks WHERE doned = 0;", []( void *data, int argc, char **argv, char **szColName ) -> int
                      {
                        auto _ui{ static_cast<Ui::Technical *>( data ) };
                        QDateTime msProdStart;
                       sqlite3_exec( database, std::format( "SELECT startdate FROM productTasks WHERE id = {};", argv[1] ).c_str(), []( void *data, int argc, char **argv, char **szColName ) -> int
                          { 
                    *static_cast<QDateTime *>( data ) = QDateTime::fromMSecsSinceEpoch( std::stoull( argv[ 0 ] ) );
                    return 0; }, &msProdStart, 0 );
                        _ui->table->insertRow( _ui->table->rowCount() );
                        _ui->table->setItem( _ui->table->rowCount() - 1, 0, new QTableWidgetItem{ argv[ 0 ] } );
                        _ui->table->setItem( _ui->table->rowCount() - 1, 1, new QTableWidgetItem{ QDateTime::fromMSecsSinceEpoch( std::stoull( argv[ 2 ] ) ).toString( "dd-MM-yyyy" ) } );
                        _ui->table->setItem( _ui->table->rowCount() - 1, 2, new QTableWidgetItem{ QDateTime::fromMSecsSinceEpoch( std::stoull( argv[ 3 ] ) ).toString( "dd-MM-yyyy" ) } );
                        _ui->table->setItem( _ui->table->rowCount() - 1, 3, new QTableWidgetItem{ argv[ 5 ] } );
                        if(  msProdStart.date()  == QDateTime::fromMSecsSinceEpoch( std::stoull( argv[ 2 ] ) ).date() )
                        {
                            _ui->table->item( _ui->table->rowCount() - 1, 0 )->setBackground( Qt::red );
                            _ui->table->item( _ui->table->rowCount() - 1, 1 )->setBackground( Qt::red );
                            _ui->table->item( _ui->table->rowCount() - 1, 2 )->setBackground( Qt::red );
                            _ui->table->item( _ui->table->rowCount() - 1, 3 )->setBackground( Qt::red );
                        }
                        else if( msProdStart.date() == QDateTime::fromMSecsSinceEpoch( std::stoull( argv[ 2 ] ) ).addDays( 1 ).date() )
                        {
                            _ui->table->item( _ui->table->rowCount() - 1, 0 )->setBackground( Qt::yellow );
                            _ui->table->item( _ui->table->rowCount() - 1, 1 )->setBackground( Qt::yellow );
                            _ui->table->item( _ui->table->rowCount() - 1, 2 )->setBackground( Qt::yellow );
                            _ui->table->item( _ui->table->rowCount() - 1, 3 )->setBackground( Qt::yellow );
                        }
                        auto d=( msProdStart ).toString();
                        auto d1=QDateTime::fromMSecsSinceEpoch( std::stoull( argv[ 2 ] ) ).addDays( 1 ).toString();
                        return 0; }, ui, 0 );
}

void Technical::on_pushButton_clicked()
{
    if( ui->widget->isHidden() || !ui->ProductionTaskComboBox->isEnabled() )
    {
        ui->widget->show();
        ui->dateOfCreateDateEdit->setReadOnly( 0 );
        ui->ProductionTaskComboBox->setDisabled( 0 );
        ui->workspaceComboBox->setDisabled( 0 );
        ui->DescriptionPlainTextEdit->setReadOnly( 0 );
        ui->dateOfCreateDateEdit->setDateTime( QDateTime::currentDateTime() );
        ui->ProductionTaskComboBox->clear();
        sqlite3_exec( database, "SELECT id, description, manufactoriesMask FROM productTasks;", []( void *data, int argc, char **argv, char **szColName ) -> int // product tasks
                      { auto _ui{static_cast<Ui::Technical*>(data)};
                  _ui->ProductionTaskComboBox->addItem(argv[0]);
                  _ui->ProductionTaskDescriptionLabel->setText(argv[1]);

                  return 0; }, ui, 0 );

        ui->ProductionTaskDescriptionLabel->clear();
        ui->workspaceComboBox->clear();
        ui->DescriptionPlainTextEdit->clear();
        ui->TaskDoneCheckBox->setCheckState( Qt::CheckState::Unchecked );
    }
    else
    {
        ui->widget->hide();
    }
}

void Technical::on_ProductionTaskComboBox_currentTextChanged( const QString &arg )
{
    ui->workspaceComboBox->clear();
    sqlite3_exec( database, std::format( "SELECT manufactoriesMask, description FROM productTasks WHERE id = {}", arg.toStdString() ).c_str(), []( void *data, int argc, char **argv, char **szColName ) -> int
                  { 
                    auto _ui{ static_cast<Ui::Technical *>( data ) };
                    auto mnfcMask{ std::stoull( argv[ 0 ] ) };
                    if( mnfcMask & manufactBits::first )
                        sqlite3_exec( database, std::format( "SELECT name FROM workplaces WHERE manufactoryID = {};", 1 ).c_str(), []( void *data, int argc, char **argv, char **szColName ) -> int
                                    {
                                                    static_cast<Ui::Technical*>(data)->workspaceComboBox->addItem(argv[0]);
                                                    return 0; }, _ui, 0 );
                    if( mnfcMask & manufactBits::second )
                        sqlite3_exec( database, std::format( "SELECT name FROM workplaces WHERE manufactoryID = {};", 2 ).c_str(), []( void *data, int argc, char **argv, char **szColName ) -> int
                                    {
                                                    static_cast<Ui::Technical*>(data)->workspaceComboBox->addItem(argv[0]);
                                                    return 0; }, _ui, 0 );
                    if( mnfcMask & manufactBits::third )
                        sqlite3_exec( database, std::format( "SELECT name FROM workplaces WHERE manufactoryID = {};", 4 ).c_str(), []( void *data, int argc, char **argv, char **szColName ) -> int
                                    {
                                                    static_cast<Ui::Technical*>(data)->workspaceComboBox->addItem(argv[0]);
                                                    return 0; }, _ui, 0 );
                    if( mnfcMask & manufactBits::fourth )
                        sqlite3_exec( database, std::format( "SELECT name FROM workplaces WHERE manufactoryID = {};", 8 ).c_str(), []( void *data, int argc, char **argv, char **szColName ) -> int
                                    {
                                                    static_cast<Ui::Technical*>(data)->workspaceComboBox->addItem(argv[0]);
                                                    return 0; }, _ui, 0 );
                    _ui->ProductionTaskDescriptionLabel->setText( argv[ 1 ] );
                    return 0; }, ui, 0 );
}

void Technical::on_TaskStatusLabel_clicked()
{
    if( ui->ProductionTaskComboBox->currentIndex() == -1 || ui->workspaceComboBox->currentIndex() == -1 )
    {
        ui->StatusLabel->show();
        ui->StatusLabel->setText( "Не все поля заполнены" );
        return;
    }
    QDateTime date;
    sqlite3_exec( database, std::format( "SELECT startdate FROM productTasks WHERE id = {};", ui->ProductionTaskComboBox->currentText().toStdString() ).c_str(), []( void *data, int argc, char **argv, char **szColName ) -> int
                  { 
                    *static_cast<QDateTime *>( data ) = QDateTime::fromMSecsSinceEpoch( std::stoull( argv[ 0 ] ) );
                    return 0; }, &date, 0 );

    if( ui->dateOfCreateDateEdit->dateTime() > date )
    {
        ui->StatusLabel->show();
        ui->StatusLabel->setText( "Дата неверна" );
        return;
    }
    // todo: second button.
    uint64_t wID;
    sqlite3_exec( database, std::format( "SELECT id FROM workplaces WHERE name = '{}'", ui->workspaceComboBox->currentText().toStdString() ).c_str(), []( void *data, int argc, char **argv, char **szColName ) -> int
                  { *static_cast<uint64_t*>(data) = std::stoull(argv[0]);
                    return 0; }, &wID, 0 );
    if( ui->ProductionTaskComboBox->isEnabled() )
        sqlite3_exec( database, std::format( "INSERT INTO prepareTasks (productTasksID, regDate, startDate, workplaceID, description, doned) VALUES ({}, {}, {}, {}, '{}', {})", ui->ProductionTaskComboBox->currentText().toStdString(), QDateTime::currentMSecsSinceEpoch(), ui->dateOfCreateDateEdit->dateTime().toMSecsSinceEpoch(), wID, ui->DescriptionPlainTextEdit->toPlainText().toStdString(), ui->TaskDoneCheckBox->checkState() == Qt::Checked ).c_str(), 0, 0, 0 );
    else
        sqlite3_exec( database, std::format( "UPDATE prepareTasks SET doned = {} WHERE id = {};", ui->TaskDoneCheckBox->checkState() == Qt::Checked, ui->table->item( ui->table->currentRow(), 0 )->text().toStdString() ).c_str(), 0, 0, 0 );
    ui->StatusLabel->hide();
    ui->widget->hide();
    reloadTasksTable();
}

void Technical::on_table_cellClicked( int row, int column )
{
    ui->widget->show();
    ui->StatusLabel->hide();
    ui->workspaceComboBox->clear();
    ui->ProductionTaskComboBox->clear();
    sqlite3_exec( database, std::format( "SELECT productTasksID, startDate, workplaceID, description, doned FROM prepareTasks WHERE id = {};", ui->table->item( row, 0 )->text().toStdString() ).c_str(), []( void *data, int argc, char **argv, char **szColName ) -> int
                  { 
                    auto _ui{ static_cast<Ui::Technical *>( data ) };
                    _ui->dateOfCreateDateEdit->setDateTime( QDateTime::fromMSecsSinceEpoch(std::stoull(argv[1])) );
                    sqlite3_exec( database, std::format("SELECT id, description FROM productTasks WHERE id = {};", argv[0]).c_str(), []( void *data, int argc, char **argv, char **szColName ) -> int // product tasks
                    {
                        auto _ui{static_cast<Ui::Technical*>(data)};
                        _ui->ProductionTaskComboBox->addItem(argv[0]);
                        _ui->ProductionTaskComboBox->setCurrentIndex(0);
                        _ui->ProductionTaskDescriptionLabel->setText(argv[1]);
                    return 0; }, _ui, 0 );
                    sqlite3_exec(database, std::format("SELECT name FROM workplaces WHERE id = {};", argv[2]).c_str(), []( void *data, int argc, char **argv, char **szColName ) -> int{
                        auto _ui{ static_cast<Ui::Technical *>( data ) };
                        _ui->workspaceComboBox->addItem(argv[0]);
                        _ui->workspaceComboBox->setCurrentIndex(0);
                         return 0;}, _ui, 0 );
                    _ui->DescriptionPlainTextEdit->setPlainText(argv[3]);
                    _ui->TaskDoneCheckBox->setCheckState(std::stoi(argv[4]) == 1 ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
                    _ui->dateOfCreateDateEdit->setReadOnly(1);
                    _ui->ProductionTaskComboBox->setDisabled(1);
                    _ui->workspaceComboBox->setDisabled(1);
                    _ui->DescriptionPlainTextEdit->setReadOnly(1);
                    return 0; }, ui, 0 );
}

void Technical::on_comboBox_currentTextChanged( const QString &arg1 )
{
    reloadTasksTable();
}
