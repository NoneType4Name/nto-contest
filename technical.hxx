#pragma once
#include "ui_technical.h"
#ifndef TECHNICAL_HXX
#    define TECHNICAL_HXX

#    include <QWidget>

namespace Ui
{
class technical;
}

class Technical : public QWidget
{
    Q_OBJECT

  public:
    explicit Technical( QWidget *parent = nullptr );
    ~Technical();

  private slots:
    void on_pushButton_clicked();
    void on_ProductionTaskComboBox_currentTextChanged( const QString &arg1 );

    void on_TaskStatusLabel_clicked();

    void on_table_cellClicked( int row, int column );

    void on_comboBox_currentTextChanged( const QString &arg1 );

  private:
    Ui::Technical *ui;
    void reloadTasksTable();
};

#endif // TECHNICAL_HXX
