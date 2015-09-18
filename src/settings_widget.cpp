#include "settings_widget.h"
#include "ui_settings_widget.h"
#include "widget.h"

#include <QFile>
#include <QUrl>
#include <QColor>
#include <QSettings>

QSettings loewctconfig(LOEWCTCONFIGFILEPATH, QSettings::IniFormat);
QSettings config(CONFIGFILEPATH, QSettings::IniFormat);

settings_widget::settings_widget(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::settings_widget)
{
    ui->setupUi(this);
}

settings_widget::~settings_widget()
{
    delete ui;
}

void settings_widget::on_spinBox_valueChanged(int arg1)
{
    loewctconfig.setValue("rollCoolDown", arg1);
}

void settings_widget::on_lineEdit_textEdited(const QString &arg1)
{
    loewctconfig.setValue("serverPrefix", arg1);
}

void settings_widget::on_lineEdit_2_textEdited(const QString &arg1)
{
    loewctconfig.setValue("motdMessage", arg1);
}

void settings_widget::on_lineEdit_3_textEdited(const QString &arg1)
{
    loewctconfig.setValue("rulesMessage", arg1);
}

void settings_widget::on_lineEdit_4_textEdited(const QString &arg1)
{
    loewctconfig.setValue("helpMessage", arg1);
}

void settings_widget::on_spinBox_2_valueChanged(int arg1)
{
    config.setValue("maxConnected", arg1);
}

void settings_widget::on_spinBox_3_valueChanged(int arg1)
{
    config.setValue("maxRegistered", arg1);
}

void settings_widget::on_spinBox_4_valueChanged(int arg1)
{
    config.setValue("syncInterval", arg1);
}

void settings_widget::on_spinBox_5_valueChanged(int arg1)
{
    config.setValue("pingTimeout", arg1);
}

void settings_widget::on_spinBox_6_valueChanged(int arg1)
{
    config.setValue("pingCheckInterval", arg1);
}
void settings_widget::on_spinBox_7_valueChanged(int arg1)
{
    config.setValue("loginPort", arg1);
}

void settings_widget::on_spinBox_8_valueChanged(int arg1)
{
    config.setValue("gamePort", arg1);
}

void settings_widget::on_checkBox_stateChanged(int arg1)
{
    if(arg1 == 2) loewctconfig.setValue("pvp", true);
    else loewctconfig.setValue("pvp", false);
}

void settings_widget::on_checkBox_2_stateChanged(int arg1)
{
    if (arg1 == 2) loewctconfig.setValue("playerMapTp", true);
    else loewctconfig.setValue("playerMapTp", false);
}

void settings_widget::on_checkBox_3_stateChanged(int arg1)
{
    if (arg1 == 2)
    {
        config.setValue("enableGameServer", true);
        //ui->checkBox_5->setCheckable(true);
        ui->checkBox_5->setVisible(true);
    }
    else
    {
        config.setValue("enableGameServer", false);
        //ui->checkBox_5->setCheckable(false);
        ui->checkBox_5->setVisible(false);
    }
}

void settings_widget::on_checkBox_4_stateChanged(int arg1)
{
    if (arg1 == 2) config.setValue("enableLoginServer", true);
    else config.setValue("enableLoginServer", false);
}

void settings_widget::on_checkBox_5_stateChanged(int arg1)
{
    if (arg1 == 2) config.setValue("enableMultiplayer", true);
    else config.setValue("enableMultiplayer", false);
}

void settings_widget::on_checkBox_6_stateChanged(int arg1)
{
    if (arg1 == 2) loewctconfig.setValue("playerJoinedMsg", true);
    else loewctconfig.setValue("playerJoinedMsg", false);
}

/*void settings_widget::on_cbxAutoRefreshPlayerList_stateChanged(int arg1)
{

    if (arg1 == 2)
    {
        loewctconfig.setValue("playerListRefresh", true);
        ui->lblRefreshPlayerListInt->setEnabled(true);
        ui->spbPlayerRefreshInterval->setEnabled(true);

        LoEWCTsettings::playerListRefresh = true;
    }
    else if (arg1 == 0)
    {
        loewctconfig.setValue("playerListRefresh", false);
        ui->lblRefreshPlayerListInt->setEnabled(false);
        ui->spbPlayerRefreshInterval->setEnabled(false);

        LoEWCTsettings::playerListRefresh = false;
    }
}

void settings_widget::on_spbPlayerRefreshInterval_valueChanged(int arg1)
{
    arg1 = LoEWCTsettings::playerListRefreshInterval;
    loewctconfig.setValue("playerListRefreshInterval", arg1);
}*/

void settings_widget::on_spbAnheroDelay_valueChanged(int arg1)
{
    loewctconfig.setValue("anheroDelay", arg1);
}

void settings_widget::on_spbMaxDups_valueChanged(int arg1)
{
    loewctconfig.setValue("maxPacketDups", arg1);
}
