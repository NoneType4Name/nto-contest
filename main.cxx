#include "common.hxx"
#include "commercial.hxx"
#include "login.hxx"
#include "technical.hxx"
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
Technical *techincalWdg;
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
    sqlite3_open( "db.db", &database );
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
                            "INSERT OR IGNORE INTO clients (id, name) VALUES (1, 'Петров Василий Иванович');"
                            "CREATE TABLE if not EXISTS manufactories( id INTEGER primary key, name text );"
                            "INSERT OR IGNORE INTO manufactories( id, name ) VALUES( 1, 'Лесопильный цех' );"
                            "INSERT OR IGNORE INTO manufactories( id, name ) VALUES( 2, 'Сушильный комплекс' );"
                            "INSERT OR IGNORE INTO manufactories( id, name ) VALUES( 4, 'Цех строжки и обработки' );"
                            "INSERT OR IGNORE INTO manufactories( id, name ) VALUES( 8, 'Пеллетный цех' );"
                            "CREATE TABLE if NOT EXISTS workplaces( id INTEGER primary key, manufactoryID INTEGER, name text, description text );"
                            "INSERT OR IGNORE INTO workplaces( id, manufactoryID, name, description ) VALUES( 0, 1, 'Лесопильная линия №1', 'Используется для распиловки тонкомеров, например, реек' );"
                            "INSERT OR IGNORE INTO workplaces( id, manufactoryID, name, description ) VALUES( 1, 1, 'Лесопильная линия №2', 'Используется для распиловки среднего леса' );"
                            "INSERT OR IGNORE INTO workplaces( id, manufactoryID, name ) VALUES( 2, 2, 'Сушильная камера №1' );"
                            "INSERT OR IGNORE INTO workplaces( id, manufactoryID, name ) VALUES( 3, 2, 'Сушильная камера №2' );"
                            "INSERT OR IGNORE INTO workplaces( id, manufactoryID, name ) VALUES( 4, 2, 'Сушильная камера №3' );"
                            "INSERT OR IGNORE INTO workplaces( id, manufactoryID, name ) VALUES( 5, 2, 'Сушильная камера №4' );"
                            "INSERT OR IGNORE INTO workplaces( id, manufactoryID, name, description ) VALUES( 6, 4, 'Линия строжки №1', 'Используется для строжки тонкомеров' );"
                            "INSERT OR IGNORE INTO workplaces( id, manufactoryID, name, description ) VALUES( 7, 4, 'Линия строжки №2', 'Используется для строжки среднего леса' );"
                            "INSERT OR IGNORE INTO workplaces( id, manufactoryID, name, description ) VALUES( 8, 4, 'Линия строжки №3', 'Используется для строжки среднего леса' );"
                            "INSERT OR IGNORE INTO workplaces( id, manufactoryID, name ) VALUES( 9, 8, 'Дробилка' );"
                            "INSERT OR IGNORE INTO workplaces( id, manufactoryID, name ) VALUES( 10, 8, 'Сушилка' );"
                            "INSERT OR IGNORE INTO workplaces( id, manufactoryID, name ) VALUES( 11, 8, 'Гранулятор №1' );"
                            "INSERT OR IGNORE INTO workplaces( id, manufactoryID, name ) VALUES( 12, 8, 'Гранулятор №2' );"
                            "CREATE TABLE IF NOT EXISTS productTasks(id INTEGER PRIMARY KEY, orderID INTEGER, regDate INTEGER, startDate INTEGER, productID INTEGER, amount INTEGER, manufactoriesMask INTEGER, description TEXT);"
                            "INSERT OR IGNORE INTO productTasks (id, orderID, regDate, startDate, productID, amount, manufactoriesMask) VALUES (0, 1, 1733664438424, 1733750820000, 1, 1, 3);"
                            "INSERT OR IGNORE INTO productTasks (id, orderID, regDate, startDate, productID, amount, manufactoriesMask) VALUES (1, 2, 1733664438424, 1733750820000, 3, 1, 7);"
                            "INSERT OR IGNORE INTO productTasks (id, orderID, regDate, startDate, productID, amount, manufactoriesMask) VALUES (2, 3, 1733664438424, 1733750820000, 5, 1, 8);"
                            "CREATE TABLE IF NOT EXISTS prepareTasks(id INTEGER PRIMARY KEY, productTasksID INTEGER, regDate INTEGER, startDate INTEGER, workplaceID INTEGER, description TEXT, doned BOOL);",
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
    techincalWdg  = new Technical{ wnd };
    _wd.show();
    _wd.addWidget( LoginWdg );
    _wd.addWidget( commercialWdg );
    _wd.addWidget( techincalWdg );
    _wd.setCurrentWidget( techincalWdg );
    _wd.showMaximized();
    return a.exec();
}
