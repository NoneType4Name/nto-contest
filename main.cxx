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
#include <sqlite3.h>

sqlite3 *db;
QStackedWidget *wnd;
Commercial *commercialWdg;
Login *LoginWdg;
struct _
{

    ~_()
    {
        sqlite3_close( db );
    }
} _;

void setCurrentWidgetCommercial()
{
    wnd->setCurrentWidget( commercialWdg );
}

int main( int argc, char *argv[] )
{
    int rc{ sqlite3_open( "db.db", &db ) };
    if( rc )
    {
        // cout << "Can't open database: " << sqlite3_errmsg( db ) << "\n";
        return -1;
    }
    else
    {
        // cout << "Open database successfully\n\n";
    }
    sqlite3_exec( db, "create table if not exists clients(id int primary key, name text);", 0, 0, 0 );
    sqlite3_exec( db, "create table if not exists products(id int primary key, name text, description longtext);", 0, 0, 0 );
    sqlite3_exec( db, "create table if not exists orders(id int primary key, status int, clientID int, productID int, amount int, regDate int, expDate int, description longtext);", 0, 0, 0 );

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
    wnd           = &_wd;
    LoginWdg      = new Login{ wnd };
    commercialWdg = new Commercial{ wnd };
    _wd.show();
    _wd.addWidget( LoginWdg );
    _wd.addWidget( commercialWdg );
    _wd.setCurrentWidget( LoginWdg );
    _wd.setWindowState( Qt::WindowFullScreen );
    return a.exec();
}
