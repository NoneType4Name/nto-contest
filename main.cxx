#include "common.hxx"
#include "commercial.hxx"
#include "login.hxx"
#include <qnamespace.h>
#include <qstackedwidget.h>
#include <sqlite3.h>
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QStackedWidget>
#include <qlogging.h>

sqlite3 *database;
QStackedWidget *wnd;
Commercial *commercialWdg;
Login *LoginWdg;
struct _
{

    ~_()
    {
        sqlite3_close( database );
    }
} _;

void setCurrentWidgetCommercial()
{
    wnd->setCurrentWidget( commercialWdg );
}

int main( int argc, char *argv[] )
{
    int rc{ sqlite3_open( "db.db", &database ) };
    if( rc )
    {
        // cout << "Can't open database: " << sqlite3_errmsg( db ) << "\n";
        return -1;
    }
    else
    {
        // cout << "Open database successfully\n\n";
    }
    sqlite3_exec( database, "create table if not exists clients(id INTEGER primary key, name text);"
                            "create table if not exists products(id INTEGER primary key, name text, description longtext);"
                            "create table if not exists orders(id INTEGER primary key, status INTEGER, clientID INTEGER, productID INTEGER, amount INTEGER, regDate INTEGER, expDate INTEGER, description LONGTEXT);"
                            "INSERT OR IGNORE INTO products (id, name) VALUES (0, 'Cырые пиломатериалы');"
                            "INSERT OR IGNORE INTO products (id, name) VALUES (1, 'Cухие пиломатериалы');"
                            "INSERT OR IGNORE INTO products (id, name) VALUES (2, 'Cтроганные доски');"
                            "INSERT OR IGNORE INTO products (id, name) VALUES (3, 'Рейки');"
                            "INSERT OR IGNORE INTO products (id, name) VALUES (4, 'Брус');"
                            "INSERT OR IGNORE INTO products (id, name) VALUES (5, 'Пеллеты');"
                            "INSERT OR IGNORE INTO clients (id, name) VALUES (0, 'Иванов Иван Иванович');"
                            "INSERT OR IGNORE INTO clients (id, name) VALUES (1, 'Петров Василий Иванович');",

                  0, 0, 0 );

    QApplication a( argc, argv );
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for( const QString &locale : uiLanguages )
    {
        const QString baseName = "nto-contest_" + QLocale( locale ).name();
        if( translator.load( ":/i18n/" + baseName ) )
        {
            a.installTranslator( &translator );
            break;
        }
    }
    QStackedWidget _wd;
    _wd.resize( 800, 600 );
    wnd           = &_wd;
    LoginWdg      = new Login{ wnd };
    commercialWdg = new Commercial{ wnd };
    _wd.show();
    _wd.addWidget( LoginWdg );
    _wd.addWidget( commercialWdg );
    _wd.setCurrentWidget( LoginWdg );
    _wd.showMaximized();
    return a.exec();
}
