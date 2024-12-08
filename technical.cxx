#include "common.hxx"
#include "technical.hxx"
#include "ui_technical.h"
#include <QDateTime>
#include <format>
#include <qdatetime.h>
#include <string>

Technical::Technical( QWidget *parent ) : QWidget( parent ),
                                          ui( new Ui::Technical )
{
    ui->setupUi( this );
    ui->table->horizontalHeader()->setStretchLastSection( 1 );
    ui->table->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch );
    ui->splitter->setStretchFactor( 0, 3 );
    ui->splitter->setStretchFactor( 1, 1 );
    ui->StatusLabel->hide();
    reloadTasksTable();
}

Technical::~Technical()
{
    delete ui;
}

void Technical::reloadTasksTable()
{
    ui->table->setRowCount( 1 );
    auto i = sqlite3_exec( database, "SELECT * FROM prepareTasks;", []( void *data, int argc, char **argv, char **szColName ) -> int
                           {
                        auto _ui{ static_cast<Ui::Technical *>( data ) };
                        _ui->table->insertRow( _ui->table->rowCount() );
                        _ui->table->setItem( _ui->table->rowCount() - 1, 0, new QTableWidgetItem{argv[ 0 ]} );
                        _ui->table->setItem( _ui->table->rowCount() - 1, 1, new QTableWidgetItem { QDateTime::fromMSecsSinceEpoch( std::stoull( argv[ 2 ] ) ).toString( "dd-MM-yyyy" ) } );
                        _ui->table->setItem( _ui->table->rowCount() - 1, 2, new QTableWidgetItem { QDateTime::fromMSecsSinceEpoch( std::stoull( argv[ 3 ] ) ).toString( "dd-MM-yyyy" ) } );
                        _ui->table->setItem( _ui->table->rowCount() - 1, 3 , new QTableWidgetItem { argv[ 5 ]});
                        return 0; }, ui, 0 );
}

void Technical::on_pushButton_clicked()
{
    ui->widget->show();
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
    int i = sqlite3_exec( database, std::format( "SELECT startdate FROM productTasks WHERE id = {};", ui->ProductionTaskComboBox->currentText().toStdString() ).c_str(), []( void *data, int argc, char **argv, char **szColName ) -> int
                          { 
                    *static_cast<QDateTime *>( data ) = QDateTime::fromMSecsSinceEpoch( std::stoull( argv[ 0 ] ) );
                    return 0; }, &date, 0 );

    if( ui->dateOfCreateDateEdit->dateTime() > date )
    {
        ui->StatusLabel->show();
        ui->StatusLabel->setText( "Дата неверна" );
        return;
    }
    // insert in bd, hide widget, sort and second button.
    ui->StatusLabel->hide();
}
