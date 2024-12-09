#include "commercial.hxx"
#include "neworder.hxx"
#include "login.hxx"
#include "ui_commercial.h"
#include "order.hxx"
#include "ui_order.h"
#include "common.hxx"
#include <cstdint>
#include <format>
#include <qdatetime.h>
#include <qlocale.h>
#include <QDatetime>
#include <qobject.h>
#include <string>
#include <vector>

Commercial::Commercial( QWidget *parent ) : QWidget( parent ),
                                            ui( new Ui::Commercial )
{
    ui->setupUi( this );
    ui->ordersList->addItem( "" );
    ui->ordersList->item( 0 )->setSizeHint( QSize( 0, 62 ) );
    ui->ordersList->setItemWidget( ui->ordersList->item( 0 ), new neworder( ui->ordersList ) );
    ui->ordersList->setCurrentRow( 0 );
    sqlite3_exec( database, "SELECT * FROM orders ORDER BY regDate DESC", []( void *data, int argc, char **argv, char **szColName ) -> int
                  { 
                    std::vector<char*>d(&argv[0], &argv[0]+argc);
                    static_cast<Commercial*>(data)->addOrder(std::stoull(argv[0]), static_cast<orderStatus>(std::stoull(argv[1])), (static_cast<orderStatus>(std::stoull(argv[1])) != orderStatus::draft) ? std::stoull((argv[2])):0, std::stoull(argv[3]), std::stoull(argv[4]), std::stoull(argv[5]), argv[7] );
                    return 0; }, this, 0 );
    updateClientList( "" );
    sqlite3_exec( database, "SELECT name FROM products ORDER BY id ASC", []( void *data, int argc, char **argv, char **azColName ) -> int
                  { 
                    static_cast<QComboBox *>(data)->addItem(argv[0]);
                    return 0; }, ui->productBlockSelect, 0 );
    ui->productBlockSelect->setCurrentIndex( 0 );
    ui->splitter->setStretchFactor( 0, 3 );
    ui->splitter->setStretchFactor( 1, 1 );
    clearFocus();
}

QString toCamelCase( const QString &s )
{
    QStringList parts = s.split( ' ', Qt::SkipEmptyParts );
    for( int i{ 0 }; i < parts.size(); ++i )
        parts[ i ].replace( 0, 1, parts[ i ][ 0 ].toUpper() );

    return parts.join( ' ' );
}

Commercial::~Commercial()
{
    delete ui;
}

void Commercial::addOrder( uint64_t id, orderStatus status, uint64_t clientID, uint64_t productID, uint64_t amount, uint64_t regDate, std::string desc )
{
    ui->ordersList->addItem( "" );
    ui->ordersList->item( ui->ordersList->count() - 1 )->setSizeHint( QSize( 0, 62 ) );
    auto e{ new Order( ui->ordersList ) };
    ui->ordersList->setItemWidget( ui->ordersList->item( ui->ordersList->count() - 1 ), e );
    e->id = id;
    e->ui->id->setText( QString::number( id ) );
    e->ui->status->setText( translateOrderStatus( status ).c_str() );
    e->ui->status->setStyleSheet( std::format( "color: rgb({});", colorizeOrderStatus( static_cast<orderStatus>( status ) ) ).c_str() );
    e->ui->weight->setText( QString::number( amount ) );
    e->ui->date->setText( QDateTime::fromMSecsSinceEpoch( regDate ).toString( "dd MM yyyy" ) );
    if( status != orderStatus::draft )
        sqlite3_exec( database, std::format( "SELECT name FROM clients WHERE id={}", clientID ).c_str(), []( void *data, int argc, char **argv, char **azColName ) -> int
                      { 
                    static_cast<QLabel *>(data)->setText(*argv);
                    return 0; }, e->ui->clientName, 0 );
    sqlite3_exec( database, std::format( "SELECT name FROM products WHERE id={}", productID ).c_str(), []( void *data, int argc, char **argv, char **azColName ) -> int
                  { 
                    static_cast<QLabel *>(data)->setText(*argv);
                    return 0; }, e->ui->productName, 0 );
    e->ui->id->setText( QString::number( id ) );
}

void Commercial::on_submitPushButton_clicked()
{
    if( !ui->clientBlockExistName->currentIndex() && ui->clientBlockNameEdit->text().length() ) // create client
    {
        bool unique{ 1 };
        auto nm{ toCamelCase( ui->clientBlockNameEdit->text() ).toStdString() };
        sqlite3_exec( database, std::format( "SELECT * FROM clients WHERE name = '{}';", nm ).c_str(), []( void *data, int argc, char **argv, char **azColName ) -> int
                      { 
                    *static_cast<bool*>(data) = !static_cast<bool>(argc);
                    return 0; }, &unique, 0 );
        if( unique )
            sqlite3_exec( database, std::format( "INSERT OR IGNORE INTO clients (name) VALUES ('{}');", nm ).c_str(), 0, 0, 0 );
        else
            ui->clientBlockExistName->setCurrentText( ui->clientBlockNameEdit->text() );
        ui->clientBlockNameEdit->setReadOnly( 1 );
    }

    if( ui->clientBlockNameEdit->text().length() )
        ui->orderStatus->setText( translateOrderStatus( orderStatus::conform ).c_str() );

    uint64_t pID; // product ID
    sqlite3_exec( database, std::format( "SELECT id FROM products WHERE name = '{}';", ui->productBlockSelect->currentText().toStdString() ).c_str(), []( void *data, int argc, char **argv, char **azColName ) -> int
                  { 

                    *static_cast<uint64_t*>(data) = std::stoull(argv[0]);
                    return 0; }, &pID, 0 );

    if( !ui->ordersList->currentRow() ) // editing
    {
        if( ui->clientBlockNameEdit->text().length() )
        {
            uint64_t cID;
            sqlite3_exec( database, std::format( "SELECT id FROM clients WHERE name = '{}';", ui->clientBlockNameEdit->text().toStdString() ).c_str(), []( void *data, int argc, char **argv, char **azColName ) -> int
                          {
                    *static_cast<uint64_t*>(data) = std::stoull(argv[0]);
                    return 0; }, &cID, 0 );
            sqlite3_exec( database, std::format( "INSERT OR IGNORE INTO orders (status, clientID, productID, amount, regDate, description) VALUES ( {}, {}, {}, {}, {}, '{}');", static_cast<uint64_t>( ui->clientBlockNameEdit->text().length() ? orderStatus::conform : orderStatus::draft ), cID, pID, ui->productBlockSpinBox->value(), QDateTime::currentDateTime().toMSecsSinceEpoch(), ui->textEdit->toPlainText().toStdString() ).c_str(), 0, 0, 0 );
        }
        else
            sqlite3_exec( database, std::format( "INSERT OR IGNORE INTO orders (status, productID, amount, regDate, description) VALUES ( {}, {}, {}, {}, '{}');", static_cast<uint64_t>( ui->clientBlockNameEdit->text().length() ? orderStatus::conform : orderStatus::draft ), pID, ui->productBlockSpinBox->value(), QDateTime::currentDateTime().toMSecsSinceEpoch(), ui->textEdit->toPlainText().toStdString() ).c_str(), 0, 0, 0 );
        updateOrdersList();
        ui->ordersList->setCurrentRow( 1 );
    }
    else // created new
    {
        if( ui->clientBlockNameEdit->text().length() )
        {
            uint64_t cID;
            sqlite3_exec( database, std::format( "SELECT id FROM clients WHERE name = '{}';", ui->clientBlockNameEdit->text().toStdString() ).c_str(), []( void *data, int argc, char **argv, char **azColName ) -> int
                          { 

                    *static_cast<uint64_t*>(data) = std::stoull(argv[0]);
                    return 0; }, &cID, 0 );

            sqlite3_exec( database, std::format( "UPDATE orders set status = {}, clientID = {}, productID = {}, amount = {}, description = '{}' WHERE id = {};", static_cast<uint64_t>( ui->clientBlockNameEdit->text().length() ? orderStatus::conform : orderStatus::draft ), cID, pID, ui->productBlockSpinBox->value(), ui->textEdit->toPlainText().toStdString(), static_cast<Order *>( ui->ordersList->itemWidget( ui->ordersList->currentItem() ) )->ui->id->text().toStdString() ).c_str(), 0, 0, 0 );
        }
        else
            sqlite3_exec( database, std::format( "UPDATE orders set status = {}, productID = {}, amount = {}, description = '{}' WHERE id = {};", static_cast<uint64_t>( ui->clientBlockNameEdit->text().length() ? orderStatus::conform : orderStatus::draft ), pID, ui->productBlockSpinBox->value(), ui->textEdit->toPlainText().toStdString(), static_cast<Order *>( ui->ordersList->itemWidget( ui->ordersList->currentItem() ) )->ui->id->text().toStdString() ).c_str(), 0, 0, 0 );
        updateOrdersList();
    }
    updateClientList( ui->clientBlockExistName->currentText() );
    ui->ordersList->setCurrentRow( 0 );
}

void Commercial::on_clientBlockExistName_activated( int index )
{
    if( index )
    {
        ui->clientBlockNameEdit->setText( ui->clientBlockExistName->currentText() );
        ui->clientBlockNameEdit->setReadOnly( 1 );
    }
    else
    {
        ui->clientBlockNameEdit->setText( "" );
        ui->clientBlockNameEdit->setReadOnly( 0 );
    }
}

void Commercial::updateClientList( QString text )
{
    ui->clientBlockExistName->clear();
    ui->clientBlockExistName->addItem( "Новый пользователь" );
    sqlite3_exec( database, "SELECT name FROM clients", []( void *data, int argc, char **argv, char **azColName ) -> int
                  { 
                    static_cast<QComboBox *>(data)->addItem(argv[0]);
                    return 0; }, ui->clientBlockExistName, 0 );
    ui->clientBlockExistName->setCurrentText( text );
}

void Commercial::updateOrdersList()
{
    ui->ordersList->clear();
    ui->ordersList->addItem( "" );
    ui->ordersList->item( 0 )->setSizeHint( QSize( 0, 62 ) );
    ui->ordersList->setItemWidget( ui->ordersList->item( 0 ), new neworder( ui->ordersList ) );
    sqlite3_exec( database, "SELECT * FROM orders ORDER BY regDate DESC;", []( void *data, int argc, char **argv, char **szColName ) -> int
                  { 
                    static_cast<Commercial*>(data)->addOrder(std::stoull(argv[0]), static_cast<orderStatus>(std::stoull(argv[1])), (static_cast<orderStatus>(std::stoull(argv[1])) != orderStatus::draft) ? std::stoull((argv[2])):0, std::stoull(argv[3]), std::stoull(argv[4]), std::stoull(argv[5]), argv[7] );
                    return 0; }, this, 0 );
}

void Commercial::on_ordersList_currentRowChanged( int currentRow )
{
    ui->submitPushButton->show();
    ui->productBlockSelect->setEnabled( 1 );
    ui->productBlockSpinBox->setEnabled( 1 );
    ui->clientBlockExistName->setEnabled( 1 );
    ui->clientBlockNameEdit->setReadOnly( 0 );
    ui->textEdit->setReadOnly( 0 );
    auto item{ static_cast<Order *>( ui->ordersList->itemWidget( ui->ordersList->item( currentRow ) ) ) };
    if( currentRow > 0 )
    {
        ui->clientBlockExistName->setCurrentText( item->ui->clientName->text() );
        ui->clientBlockNameEdit->setText( item->ui->clientName->text() );
        sqlite3_exec( database, std::format( "SELECT * FROM orders WHERE id = {}", static_cast<Order *>( ui->ordersList->itemWidget( ui->ordersList->currentItem() ) )->ui->id->text().toStdString() ).c_str(), []( void *data, int argc, char **argv, char **szColName ) -> int
                      { 
                        auto _ui{static_cast<Ui::Commercial*>(data)};
                        _ui->orderStatus->setText(translateOrderStatus(static_cast<orderStatus>(std::stoull(argv[1]))).c_str());
                        if(static_cast<orderStatus>(std::stoull(argv[1])) !=draft)
                        {
                        sqlite3_exec(database, std::format("SELECT name FROM clients WHERE id = {}", argv[2]).c_str(),[](void *data, int argc, char **argv, char **szColName)->int{
                                static_cast<Ui::Commercial*>(data)->clientBlockExistName->setCurrentText(argv[0]);
                                static_cast<Ui::Commercial*>(data)->clientBlockNameEdit->setText(argv[0]);
                                return 0;
                            }, _ui, 0);
                        }
                        else
                        {
                            static_cast<Ui::Commercial*>(data)->clientBlockExistName->setCurrentIndex(0);
                            static_cast<Ui::Commercial*>(data)->clientBlockNameEdit->clear();
                        }
                        _ui->orderStatus->setStyleSheet( std::format( "color: rgb({});", colorizeOrderStatus( static_cast<orderStatus>(std::stoull(argv[1])) ) ).c_str() );
                        _ui->productBlockSelect->setCurrentIndex(std::stoull(argv[3]));
                        _ui->productBlockSpinBox->setValue(std::stoull(argv[4]));
                        _ui->textEdit->setText(argv[7]);
                    return 0; }, ui, 0 );
        if( retranslateOrderStatus( item->ui->status->text().toStdString() ) == draft )
        {
            ui->editStatus->setText( "Изменение заказа" );
            ui->submitPushButton->setText( "Изменить" );
        }
        else
        {
            ui->editStatus->setText( "Просмотр заказа" );
            ui->submitPushButton->hide();
            ui->productBlockSelect->setDisabled( 1 );
            ui->productBlockSpinBox->setDisabled( 1 );
            ui->clientBlockExistName->setDisabled( 1 );
            ui->clientBlockNameEdit->setReadOnly( 1 );
            ui->textEdit->setReadOnly( 1 );
        }
    }
    if( !currentRow )
    {
        ui->submitPushButton->show();
        ui->productBlockSelect->setEnabled( 1 );
        ui->productBlockSpinBox->setEnabled( 1 );
        ui->clientBlockExistName->setEnabled( 1 );
        ui->clientBlockNameEdit->setReadOnly( 0 );
        ui->textEdit->setReadOnly( 0 );
        ui->editStatus->setText( "Создание заказа" );
        ui->submitPushButton->setText( "Создать" );
        ui->orderStatus->setText( translateOrderStatus( orderStatus::draft ).c_str() );
        ui->orderStatus->setStyleSheet( std::format( "color: rgb({});", colorizeOrderStatus( orderStatus::draft ) ).c_str() );
        ui->productBlockSelect->setCurrentIndex( 0 );
        ui->productBlockSpinBox->setValue( 1 );
        ui->textEdit->clear();
        ui->clientBlockExistName->setCurrentIndex( 0 );
        ui->clientBlockNameEdit->clear();
    }
}

void Commercial::on_exitPushButton_clicked()
{
    wnd->setCurrentWidget( loginWdg );
}
