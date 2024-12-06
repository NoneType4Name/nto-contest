#include "commercial.hxx"
#include "ui_commercial.h"

Commercial::Commercial( QWidget *parent ) : QWidget( parent ),
                                            ui( new Ui::Commercial )
{
    ui->setupUi( this );
    ui->ordersList;
}

Commercial::~Commercial()
{
    delete ui;
}
