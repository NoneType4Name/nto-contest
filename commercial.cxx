#include "commercial.hxx"
#include "ui_commercial.h"

Commercial::Commercial( QWidget *parent ) : QWidget( parent ),
                                            ui( new Ui::Commercial )
{
    ui->setupUi( this );
}

Commercial::~Commercial()
{
    delete ui;
}
