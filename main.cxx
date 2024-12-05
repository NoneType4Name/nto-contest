#include "commercial.h"
#include <sqlite3.h>
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <qlogging.h>

int main( int argc, char *argv[] )
{
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
    Commercial w;
    w.show();
    return a.exec();
}
