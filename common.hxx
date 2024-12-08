#pragma once
#include <qcolor.h>
#include <string>
#include <string_view>
#ifndef COMMON_HXX
#    define COMMON_HXX
#    include <sqlite3.h>

extern sqlite3 *database;
void setCurrentWidgetCommercial();

enum orderStatus
{
    draft,
    conform,
    manufact,
    done
};

enum manufactBits
{
    first  = 1,
    second = 2,
    third  = 4,
    fourth = 8,
};

inline std::string translateOrderStatus( orderStatus stat )
{
    switch( stat )
    {
        case draft:
            return "Черновик";
            break;
        case conform:
            return "Согласован с клиентом";
            break;
        case manufact:
            return "Принят в производство";
            break;
        case done:
            return "Выполнен";
            break;
    }
}

inline orderStatus retranslateOrderStatus( std::string_view stat )
{
    if( stat == "Черновик" )
        return draft;
    else if( stat == "Согласован с клиентом" )
        return conform;
    else if( stat == "Принят в производство" )
        return manufact;
    else
        return done;
}

inline std::string colorizeOrderStatus( orderStatus stat )
{
    switch( stat )
    {
        case draft:
            return "185, 185, 185";
        case conform:
            return "255, 123, 0";
            break;
        case manufact:
            return "255, 255, 0";
            break;
        case done:
            return "0, 255, 0";
            break;
    }
}
#endif