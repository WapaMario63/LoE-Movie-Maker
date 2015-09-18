#ifndef SETTINGS_WIDGET_H
#define SETTINGS_WIDGET_H

#include <QTabWidget>
#include <QWidget>

namespace Ui {
class settings_widget;
}

class settings_widget : public QTabWidget
{
    Q_OBJECT

public:
    explicit settings_widget(QWidget *parent = 0);
    ~settings_widget();

    int anHeroDelay;

    void mainSettings();

private slots:

    void on_spinBox_valueChanged(int arg1);
    void on_lineEdit_textEdited(const QString &arg1);
    void on_lineEdit_2_textEdited(const QString &arg1);
    void on_lineEdit_3_textEdited(const QString &arg1);
    void on_lineEdit_4_textEdited(const QString &arg1);
    void on_spinBox_2_valueChanged(int arg1);
    void on_spinBox_3_valueChanged(int arg1);
    void on_spinBox_4_valueChanged(int arg1);
    void on_spinBox_5_valueChanged(int arg1);
    void on_spinBox_6_valueChanged(int arg1);

    void on_spinBox_7_valueChanged(int arg1);

    void on_spinBox_8_valueChanged(int arg1);

    void on_checkBox_stateChanged(int arg1);

    void on_checkBox_2_stateChanged(int arg1);

    void on_checkBox_3_stateChanged(int arg1);

    void on_checkBox_4_stateChanged(int arg1);

    void on_checkBox_5_stateChanged(int arg1);

    void on_checkBox_6_stateChanged(int arg1);

    //void on_cbxAutoRefreshPlayerList_stateChanged(int arg1);

    //void on_spbPlayerRefreshInterval_valueChanged(int arg1);

    void on_spbAnheroDelay_valueChanged(int arg1);

    void on_spbMaxDups_valueChanged(int arg1);

private:
    Ui::settings_widget *ui;
};
extern settings_widget swin;

#endif // SETTINGS_WIDGET_H
