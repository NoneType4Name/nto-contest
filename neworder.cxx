#include "neworder.hxx"
#include "ui_neworder.h"

neworder::neworder( QWidget *parent ) : QWidget( parent ),
                                        ui( new Ui::neworder )
{
    ui->setupUi( this );
}

neworder::~neworder()
{
    delete ui;
}
