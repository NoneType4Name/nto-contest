#pragma once
#ifndef NEWORDER_H
#    define NEWORDER_H

#    include <QWidget>

namespace Ui
{
class neworder;
}

class neworder : public QWidget
{
    Q_OBJECT

  public:
    explicit neworder( QWidget *parent = nullptr );
    ~neworder();

  private:
    Ui::neworder *ui;
};

#endif // NEWORDER_H
