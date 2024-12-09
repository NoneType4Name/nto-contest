#pragma once
#ifndef LOGIN_H
#    define LOGIN_H

#    include <QWidget>

namespace Ui
{
class login;
}

class Login : public QWidget
{
    Q_OBJECT

  public:
    explicit Login( QWidget *parent = nullptr );
    ~Login();

  private slots:
    void on_firstCaseLayoutPushButton_clicked();

    void on_thirdCasePushButton_clicked();

    void on_secondCasePushButton_clicked();

  private:
    Ui::login *ui;
};

#endif // LOGIN_H
