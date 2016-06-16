#ifndef SET_IE_H
#define SET_IE_H

#include <QDialog>

namespace Ui {
    class Set_IE;
}

class Set_IE : public QDialog
{
    Q_OBJECT

public:
    explicit Set_IE(QWidget *parent = 0);
    ~Set_IE();
    QString mo;
public slots:
    void my_ok();
    void set(QString);

private:
    Ui::Set_IE *ui;
};

#endif // SET_IE_H
