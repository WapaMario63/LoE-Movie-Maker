#include "chat_widget.h"
#include "ui_chat_widget.h"
#include "receiveChatMessage.h"
#include "message.h"
#include "widget.h"
#include "utils.h"
#include "serialize.h"
#include "settings.h"
#include <QSpinBox>
#include <QComboBox>
#include <QString>

#if defined _WIN32 || defined WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

chat_widget::chat_widget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::chat_widget),
  chatCmdPeer(new Player())
{
  ui->setupUi(this);

    connect(ui->sendChatCmdLine, SIGNAL(clicked()), this, SLOT(chatCmdLine()));

    QTimer* timer = new QTimer;
    connect(timer, SIGNAL(timeout()), this, SLOT(refreshPlayerList()));
    timer->start(ui->spbRefreshInterval->value());

}
chat_widget::~chat_widget()
{
  delete ui;
}

// MessageBox Handler
void chat_widget::MsgBox(MsgBoxType type, QString title, QString text)
{
    // 0 = Warning, 1 = Question, 2 = information
    if (type == MsgBoxType::Warning) QMessageBox::warning(this,title,text);
    else if (type == MsgBoxType::Question) QMessageBox::question(this,title,text);
    else if (type == MsgBoxType::Information) QMessageBox::information(this,title,text);
}

void chat_widget::logChatMessage(QString msg)
{
    ui->chatLog->appendPlainText(msg);
    ui->chatLog->repaint();
}
/*void chat_widget::playerList(QString msg)
{
    ui->playerList->appendPlainText(msg);
    ui->playerList->repaint();
}*/

void chat_widget::startChat()
{
    ui->retranslateUi(this);
    logChatMessage("[SERVER] Loaded the Legends of Equestria chat.");
    logChatMessage(tr("[SERVER] Version of [LoEWCT]: ")+LOEWCT_VERSION);
    logChatMessage("[SERVER] All Messages from players and commands they do will be listed here. \n[SERVER](Commands will not work on here, you will only see the command's arguments)");
}
void chat_widget::chatCmdLine()
{
    if (!win.enableGameServer)
    {
        logChatMessage("[SERVER] NOT A GAME SERVER! Enable the Game server in the server settings and restart.");
        return;
    }

    QString str = ui->chatCmdLine->text();

    if (str == "/clear")
    {
        ui->chatLog->clear();
        return;
    }
    else
    {
        for (int i=0; i<win.udpPlayers.size(); i++)
          {
            sendChatMessage(win.udpPlayers[i], "<span color=\"cyan\">"+str+"</span>", "[SERVER]", ChatSystem);
          }
        cwin.logChatMessage("<SERVER> "+str);
    }
}

void chat_widget::refreshPlayerList()
{
    ui->playerList->clear();
    for (int i=0; i<win.udpPlayers.size(); i++)
    {
        ui->playerList->addItem(QString().setNum(win.udpPlayers[i]->pony.id)
                          // +" ("+QString().setNum(win.udpPlayers[i]->pony.netviewId)+")"
                             +" <"+win.udpPlayers[i]->name
                             +"> "+win.udpPlayers[i]->pony.name
                             +" | "+win.udpPlayers[i]->IP
                             +":"+QString().setNum(win.udpPlayers[i]->port)
                             +" | "+QString().setNum((int)timestampNow()-win.udpPlayers[i]->lastPingTime)+"s");
    }
}

//************** GUI functions ***************//
void chat_widget::on_pushButton_clicked()
{
    //QTimer *refreshLoop = new QTimer();
    //refreshLoop->setSingleShot(true);

    //while (this)
    //{
        //refreshLoop->start(5000);
        ui->playerList->clear();
          for (int i=0; i<win.udpPlayers.size(); i++)
            {
              ui->playerList->addItem(QString().setNum(win.udpPlayers[i]->pony.id)
                                   // +" ("+QString().setNum(win.udpPlayers[i]->pony.netviewId)+")"
                                    +" <"+win.udpPlayers[i]->name
                                    +"> "+win.udpPlayers[i]->pony.name
                                    +" | "+win.udpPlayers[i]->IP
                                    +":"+QString().setNum(win.udpPlayers[i]->port)
                                    +" | "+QString().setNum((int)timestampNow()-win.udpPlayers[i]->lastPingTime)+"s");
            }
     //}
}

void chat_widget::on_playerList_itemDoubleClicked(QListWidgetItem *item)
{
    QString str = item->text();
    QStringList args = str.split(' ');

    win.externCmdLine("setPlayer "+args[0]);
    cwin.logChatMessage("[SERVER] Set player to "+args[0]+args[1]);
}

void chat_widget::on_pushButton_2_clicked()
{
    swin.show();
}

void chat_widget::on_pushButton_4_clicked()
{
    ui->pushButton_4->setDisabled(true);
    ui->pushButton_4->setCursor(Qt::WaitCursor);
    win.logMessage("[INFO] Restarting server...");
    for (int i=0; i<win.udpPlayers.size(); i++)
    {
        sendMessage(win.udpPlayers[i], MsgDisconnect, "Server is Restarting");
        Player::disconnectPlayerCleanup(win.udpPlayers[i]);
    }
    ui->chatLog->clear();
    win.stopServer();
    win.startServer();
    startChat();
    ui->pushButton_4->setDisabled(false);
    ui->pushButton_4->setCursor(Qt::ArrowCursor);
}

void chat_widget::on_pushButton_5_clicked()
{
    win.stopServer();
    for (int i=0; i<win.udpPlayers.size(); i++)
    {
        sendMessage(win.udpPlayers[i], MsgDisconnect, "Server has stopped/shutdown.");
        Player::disconnectPlayerCleanup(win.udpPlayers[i]);
    }
    ui->pushButton_3->setDisabled(false);
    ui->pushButton_4->setDisabled(true);
    ui->pushButton_5->setDisabled(true);
    ui->chatLog->clear();
}

void chat_widget::on_pushButton_3_clicked()
{
    win.startServer();
    startChat();
    ui->pushButton_3->setDisabled(true);
    ui->pushButton_4->setDisabled(false);
    ui->pushButton_5->setDisabled(false);
}

void chat_widget::on_kickPlyrBtn_clicked()
{
    if (win.cmdPeer->IP=="") MsgBox(MsgBoxType::Warning, "Error", "No Player Set!");
    else win.externCmdLine("kick "+ui->kickReason->text());
}

void chat_widget::on_sendBoxMsgBtn_clicked()
{
    if (win.cmdPeer->IP=="") MsgBox(MsgBoxType::Warning, "Error", "No Player Set!");
    else win.externCmdLine("error "+ui->msgBox->text());
}

void chat_widget::on_moveToMapBtn_clicked()
{
    if (win.cmdPeer->IP=="") MsgBox(MsgBoxType::Warning, "Error", "No Player Set!");
    else win.externCmdLine("load "+ui->mapSelect->currentText());
}

void chat_widget::on_tpPlyrBtn_clicked()
{
    if (win.cmdPeer->IP=="") MsgBox(MsgBoxType::Warning, "Error", "No Player Set!");
    else win.externCmdLine("tp "+ui->pID1->text()+" "+ui->pID2->text());
}

void chat_widget::on_posTpBtn_clicked()
{
    if (win.cmdPeer->IP=="") MsgBox(MsgBoxType::Warning, "Error", "No Player Set!");
    else win.externCmdLine("move "+ui->posXbox->text()+" "+ui->posYbox->text()+" "+ui->posZbox->text());
}

void chat_widget::on_getPosBtn_clicked()
{
    if (win.cmdPeer->IP=="") MsgBox(MsgBoxType::Warning, "Error", "No Player Set!");
    else
    {
        ui->plyrInfoBox->clear();
        ui->plyrInfoBox->insert("x: "+QString().setNum(win.cmdPeer->pony.pos.x)+" y: "+QString().setNum(win.cmdPeer->pony.pos.y)+" z: "+QString().setNum(win.cmdPeer->pony.pos.z));
    }
}
void chat_widget::on_getRotBtn_clicked()
{
    if (win.cmdPeer->IP=="") MsgBox(MsgBoxType::Warning, "Error", "No Player Set!");
    else
    {
        ui->plyrInfoBox->clear();
        ui->plyrInfoBox->insert("x: "+QString().setNum(win.cmdPeer->pony.rot.x)+" y: "+QString().setNum(win.cmdPeer->pony.rot.y)+" z: "+QString().setNum(win.cmdPeer->pony.rot.z)+" w: "+QString().setNum(win.cmdPeer->pony.rot.w));
    }
}

void chat_widget::on_getPDBtn_clicked()
{
    if (win.cmdPeer->IP=="") MsgBox(MsgBoxType::Warning, "Error", "No Player Set!");
    else
    {
        ui->plyrInfoBox->clear();
        ui->plyrInfoBox->insert(win.cmdPeer->pony.ponyData.toBase64());
    }
}
void chat_widget::on_chatClrBtn_clicked()
{
    ui->chatLog->clear();
}

void chat_widget::on_banPlrBtn_clicked()
{
    if (win.cmdPeer->IP=="") MsgBox(MsgBoxType::Warning, "Error", "No Player Set!");
    else
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::warning(this,"Player Ban", "Are you sure you want to ban this player?", QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes)
        {
            win.externCmdLine("ban "+ui->banreason->text());
        }
    }
}

void chat_widget::on_PromoModBtn_clicked()
{
    win.externCmdLine("mod");
}

void chat_widget::on_undoModBtn_clicked()
{
    win.externCmdLine("demod");
}
