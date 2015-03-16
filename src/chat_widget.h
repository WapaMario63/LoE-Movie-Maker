#ifndef CHAT_WIDGET_H
#define CHAT_WIDGET_H

#include <QWidget>
#include "receiveChatMessage.h"
#include "widget.h"
//#include "ui_chat_widget.h"
#include "settings_widget.h"
#include <QPointer>
#include <QSpinBox>
#include <QComboBox>
#include <QMessageBox>

namespace Ui {
  class chat_widget;
}

class QSpinBox;

class chat_widget : public QWidget
{
  Q_OBJECT
public slots:
    void chatCmdLine();
    void refreshPlayerList();

public:
    Ui::chat_widget *ui;
    explicit chat_widget(QWidget *parent = 0);
    ~chat_widget();
    void logChatMessage(QString msg);
    void playerList(QString msg);
    void startChat();
    Player* chatCmdPeer;

private slots:

    void on_pushButton_clicked();

   // void on_playerList_itemDoubleClicked(QListWidgetItem *item);

    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_3_clicked();

    void on_kickPlyrBtn_clicked();

    void on_sendBoxMsgBtn_clicked();

    void on_moveToMapBtn_clicked();

    void on_tpPlyrBtn_clicked();

    void on_posTpBtn_clicked();

    void on_getPosBtn_clicked();

    void on_getRotBtn_clicked();

    void on_getPDBtn_clicked();

    void on_chatClrBtn_clicked();

    void on_banPlrBtn_clicked();

    void on_PromoModBtn_clicked();

    void on_undoModBtn_clicked();

private:
    enum MsgBoxType
    {
        Warning,
        Question,
        Information
    };
    void MsgBox(MsgBoxType type, QString title, QString text);
    void logPlayerCount(QString msg);
};

extern chat_widget cwin;

#endif // CHAT_WIDGET_H
