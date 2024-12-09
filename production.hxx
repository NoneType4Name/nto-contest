#ifndef PRODUCTION_HXX
#define PRODUCTION_HXX

#include <QWidget>

namespace Ui
{
class production;
}

class production : public QWidget
{
    Q_OBJECT

  public:
    explicit production( QWidget *parent = nullptr );
    ~production();

  private slots:
    void on_exitPushButton_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_orderComboBox_currentTextChanged( const QString &arg1 );

  private:
    Ui::production *ui;
};

#endif // PRODUCTION_HXX
