#pragma once
#include <cstdint>
#ifndef ORDER_H
#    define ORDER_H

#    include "commercial.hxx"
#    include <QWidget>

namespace Ui
{
class Order;
}

class Order : public QWidget
{
    Q_OBJECT

  public:
    explicit Order( QWidget *parent = nullptr );
    ~Order();
    uint64_t id;

  private:
    friend Commercial;
    Ui::Order *ui;
};

#endif // ORDER_H
