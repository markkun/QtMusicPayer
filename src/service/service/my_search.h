#ifndef MY_SEARCH_H
#define MY_SEARCH_H

#include <QDialog>

namespace Ui {
    class My_Search;
}

class My_Search : public QDialog
{
    Q_OBJECT

public:
    explicit My_Search(QWidget *parent = 0);
    ~My_Search();
    QString search_filename;
public slots:
    void my_ok();
private:
    Ui::My_Search *ui;
};

#endif // MY_SEARCH_H
