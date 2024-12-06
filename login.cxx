#include "login.hxx"
#include "ui_login.h"
#include "common.hxx"

Login::Login( QWidget *parent ) : QWidget( parent ),
                                  ui( new Ui::login )
{
    ui->setupUi( this );
}

Login::~Login()
{
    delete ui;
}

void Login::on_firstCaseLayoutPushButton_clicked()
{
    setCurrentWidgetCommercial();
}
