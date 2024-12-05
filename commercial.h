#ifndef COMMERCIAL_H
#define COMMERCIAL_H

#include <QWidget>

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

  private:
    Ui::Commercial *ui;
};

#endif // COMMERCIAL_H
