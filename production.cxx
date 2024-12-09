#include "production.hxx"
#include "common.hxx"
#include "login.hxx"
#include "ui_production.h"
#include <cstdint>
#include <qdatetime.h>
#include <vector>

production::production( QWidget *parent ) : QWidget( parent ),
                                            ui( new Ui::production )
{
    ui->setupUi( this );
    ui->label_3->hide();
    ui->widget_2->hide();
}

production::~production()
{
    delete ui;
}

void production::on_exitPushButton_clicked()
{
    ui->widget_2->hide();
    wnd->setCurrentWidget( loginWdg );
}

void production::on_pushButton_clicked()
{
    if( ui->widget_2->isHidden() )
    {
        ui->widget_2->show();
        ui->dateEdit->setDate( QDateTime::currentDateTime().date() );
        ui->orderComboBox->clear();
        ui->manufactoriesListWidget->clear();
        sqlite3_exec( database, "SELECT id, productID FROM orders WHERE status = 1", []( void *data, int argc, char **argv, char **szColName ) -> int
                      { 
                    auto _ui{static_cast<Ui::production*>(data)};
                    _ui->orderComboBox->addItem(argv[0]);
                    return 0; }, ui, 0 );
        sqlite3_exec( database, "SELECT name FROM manufactories;", []( void *data, int argc, char **argv, char **szColName ) -> int
                      { 
                    auto _ui{static_cast<Ui::production*>(data)};
                    _ui->manufactoriesListWidget->addItem(argv[0]);
                    return 0; }, ui, 0 );
        ui->amountSpinBox->setValue( 1 );
        ui->descriptionPlainTextEdit->clear();
    }
    else
    {
        ui->widget_2->hide();
    }
}

void production::on_pushButton_2_clicked()
{
    if( ui->orderComboBox->currentIndex() == -1 || ui->manufactoriesListWidget->selectedItems().empty() )
    {
        ui->label_3->show();
        ui->label_3->setText( "Не все поля заполнены" );
        return;
    }
    std::vector<uint64_t> mIDs;
    mIDs.reserve( ui->manufactoriesListWidget->selectedItems().count() );
    for( auto &name : ui->manufactoriesListWidget->selectedItems() )
        sqlite3_exec( database, std::format( "SELECT id FROM manufactories WHERE name = '{}'", name->text().toStdString() ).c_str(), []( void *data, int argc, char **argv, char **szColName ) -> int
                      { static_cast<std::vector<uint64_t>*>(data)->emplace_back(std::stoull(argv[0]));
                    return 0; }, &mIDs, 0 );
    uint32_t mnMask{ 0 };
    for( auto manufact : mIDs )
        mnMask |= manufact;
    uint64_t pID; // product ID
    sqlite3_exec( database, std::format( "SELECT id FROM products WHERE name = '{}';", ui->label_2->text().toStdString() ).c_str(), []( void *data, int argc, char **argv, char **azColName ) -> int
                  { 

                    *static_cast<uint64_t*>(data) = std::stoull(argv[0]);
                    return 0; }, &pID, 0 );

    sqlite3_exec( database, std::format( "INSERT INTO productTasks (orderID, regDate, startDate, productID, amount, manufactoriesMask, description) VALUES ({}, {}, {}, {}, {}, {}, '{}')", ui->orderComboBox->currentText().toStdString(), QDateTime::currentMSecsSinceEpoch(), ui->dateEdit->dateTime().toMSecsSinceEpoch(), pID, ui->amountSpinBox->value(), mnMask, ui->descriptionPlainTextEdit->toPlainText().toStdString() ).c_str(), 0, 0, 0 );
    sqlite3_exec( database, std::format( "UPDATE orders SET status = {} WHERE id = {};", static_cast<uint64_t>( orderStatus::manufact ), ui->orderComboBox->currentText().toStdString() ).c_str(), 0, 0, 0 );
    ui->label_3->hide();
    ui->widget_2->hide();
}

void production::on_orderComboBox_currentTextChanged( const QString &arg1 )
{
    sqlite3_exec( database, std::format( "SELECT name FROM products WHERE id = (SELECT productID FROM orders WHERE id = {});", arg1.toStdString() ).c_str(), []( void *data, int argc, char **argv, char **szColName ) -> int
                  {
    auto _ui{static_cast<Ui::production*>(data)};
    _ui->label_2->setText(argv[0]);
    return 0; }, ui, 0 );
    sqlite3_exec( database, std::format( "SELECT amount FROM orders WHERE id = {};", arg1.toStdString() ).c_str(), []( void *data, int argc, char **argv, char **szColName ) -> int
                  {
    auto _ui{static_cast<Ui::production*>(data)};
    _ui->amountSpinBox->setMaximum(std::stoull(argv[0]));
    return 0; }, ui, 0 );
}
