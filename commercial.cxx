#include "commercial.hxx"
#include "ui_commercial.h"
#include "order.hxx"
#include "ui_order.h"
#include "common.hxx"
#include <cstdlib>
#include <format>
#include <qlocale.h>
#include <QDatetime>
#include <qobject.h>

Commercial::Commercial( QWidget *parent ) : QWidget( parent ),
                                            ui( new Ui::Commercial )
{
    ui->setupUi( this );
    sqlite3_exec( database, "SELECT * FROM orders", []( void *data, int argc, char **argv, char **szColName ) -> int
                  { static_cast<Commercial*>(data)->addOrder(std::atoi(argv[0]), static_cast<orderStatus>(std::atoi(argv[1])), std::atoll((argv[2])),std::atoll(argv[3]), std::atoll(argv[4]), std::atoll(argv[5]), std::atoll(argv[6]), argv[7] );
                    return 0; }, this, 0 );
    updateClientList( "" );
    sqlite3_exec( database, "SELECT name FROM products", []( void *data, int argc, char **argv, char **azColName ) -> int
                  { 
                    static_cast<QComboBox *>(data)->addItem(argv[0]);
                    return 0; }, ui->productBlockSelect, 0 );
    ui->productBlockSelect->setCurrentIndex( 0 );
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

void Commercial::addOrder( uint64_t id, orderStatus status, uint64_t clientID, uint64_t productID, uint64_t amount, uint64_t regDate, uint64_t expDate, std::string desc )
{
    ui->ordersList->addItem( "" );
    auto e{ new Order( ui->ordersList ) };
    e->id = id;
    e->ui->id->setText( QString::number( id ) );
    e->ui->status->setText( translateOrderStatus( status ).c_str() );
    e->ui->weight->setText( QString::number( amount ) );
    e->ui->date->setText( QDateTime().addSecs( regDate ).toString( "dd-MM-yyyy" ) );

    sqlite3_exec( database, std::format( "SELECT name FROM clients WHERE id={}", clientID ).c_str(), []( void *data, int argc, char **argv, char **azColName ) -> int
                  { 
                    static_cast<QLabel *>(data)->setText(*argv);
                    return 0; }, e->ui->clientName, 0 );
    sqlite3_exec( database, std::format( "SELECT name FROM products WHERE id={}", productID ).c_str(), []( void *data, int argc, char **argv, char **azColName ) -> int
                  { 
                    static_cast<QLabel *>(data)->setText(*argv);
                    return 0; }, e->ui->productName, 0 );
    e->ui->id->setText( QString::number( id ) );
    ui->ordersList->setItemWidget( ui->ordersList->item( ui->ordersList->count() ), e );
}

void Commercial::on_submitPushButton_clicked()
{
    if( !ui->clientBlockExistName->currentIndex() && ui->clientBlockNameEdit->text().length() ) // create client
    {
        bool unique;
        auto nm{ toCamelCase( ui->clientBlockNameEdit->text() ).toStdString() };
        sqlite3_exec( database, std::format( "SELECT * FROM clients WHERE name = '{}';", nm ).c_str(), []( void *data, int argc, char **argv, char **azColName ) -> int
                      { 
                    *static_cast<bool*>(data) = !static_cast<bool>(argc);
                    return 0; }, &unique, 0 );
        if( unique )
            sqlite3_exec( database, std::format( "INSERT INTO clients (id, name) VALUES ((SELECT MAX(id) FROM clients)+1, '{}');", nm ).c_str(), 0, 0, 0 );
        else
            ui->clientBlockExistName->setCurrentText( ui->clientBlockNameEdit->text() );
        ui->clientBlockNameEdit->setEnabled( 0 );
    }

    if( ui->clientBlockNameEdit->text().length() )
    {
        uint64_t pID;
        sqlite3_exec( database, std::format( "SELECT id FROM product WHERE name = '{}';", ui->productBlockSelect->currentText().toStdString() ).c_str(), []( void *data, int argc, char **argv, char **azColName ) -> int
                      { 
                    *static_cast<uint64_t*>(data) = std::atoi(argv[0]);
                    return 0; }, &pID, 0 );
        sqlite3_exec( database, std::format( "INSERT INTO orders (id, status, productID, amount, regDate, description) VALUES ((SELECT MAX(id) FROM orders)+1, {}, {}, {}, {}, '{}');", orderStatus::draft, pID ).c_str(), []( void *data, int argc, char **argv, char **azColName ) -> int
                      { 
                    *static_cast<bool*>(data) = !static_cast<bool>(argc);
                    return 0; }, &unique, 0 );
    }

    updateClientList( ui->clientBlockExistName->currentText() );
}

void Commercial::on_clientBlockExistName_activated( int index )
{
    if( index )
    {
        ui->clientBlockNameEdit->setText( ui->clientBlockExistName->currentText() );
        ui->clientBlockNameEdit->setEnabled( 0 );
    }
    else
    {
        ui->clientBlockNameEdit->setText( "" );
        ui->clientBlockNameEdit->setEnabled( 1 );
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
