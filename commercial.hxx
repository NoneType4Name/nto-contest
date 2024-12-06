#pragma once
#ifndef COMMERCIAL_H
#    define COMMERCIAL_H

#    include <QWidget>
#    include <cstdint>
#    include <string>
#    include "common.hxx"

namespace Ui
{
class Commercial;
}

class Commercial : public QWidget
{
    Q_OBJECT

  public:
    explicit Commercial( QWidget *parent = nullptr );
    ~Commercial();
    void addOrder( uint64_t id, orderStatus status, uint64_t clientID, uint64_t productID, uint64_t amount, uint64_t regDate, std::string desc );

  private slots:
    void on_submitPushButton_clicked();
    void on_clientBlockExistName_activated( int index );

  private:
    void updateClientList( QString text );
    Ui::Commercial *ui;
};

#endif // COMMERCIAL_H
