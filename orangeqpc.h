#ifndef ORANGEQPC_H
#define ORANGEQPC_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class OrangeQPC;
}
QT_END_NAMESPACE

class OrangeQPC : public QMainWindow
{
    Q_OBJECT

public:
    OrangeQPC(QWidget *parent = nullptr);
    ~OrangeQPC();

private slots:
    void on_BrowseButton_clicked();
    void on_CreateDiskButton_clicked();
    void on_BrowseQemuButton_clicked();
    void on_WithoutDVDRadioButton_toggled(bool checked);
    void on_UseDVDRadioButton_toggled(bool checked);
    void on_BrowseDVDButton_clicked();


    void on_RunButton_clicked();

private:
    Ui::OrangeQPC *ui;
};
#endif // ORANGEQPC_H
