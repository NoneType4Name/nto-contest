#include "login.hxx"
#include "ui_login.h"
#include "common.hxx"
#include "technical.hxx"
#include "production.hxx"
#include "commercial.hxx"

Login::Login( QWidget *parent ) : QWidget( parent ),
                                  ui( new Ui::login )
{
    ui->setupUi( this );
    clearFocus();
}

Login::~Login()
{
    delete ui;
}

void Login::on_firstCaseLayoutPushButton_clicked()
{
    wnd->setCurrentWidget( commercialWdg );
}

void Login::on_thirdCasePushButton_clicked()
{
    wnd->setCurrentWidget( techincalWdg );
}

void Login::on_secondCasePushButton_clicked()
{
    wnd->setCurrentWidget( productionWdg );
}
