#ifndef FORM_H
#define FORM_H

#include <QWidget>

#include "character.h"
#include <message.h>
#include <Utils/Thread.h>

class QListWidgetItem;
class QFile;


namespace Ui {
class Form;
}

namespace ServerVersion
{
    extern bool isBABSCon; // Use BABScon14 Configurations
    extern bool isAugust; // Use August14 Configurations
    extern bool isJanuray; // Use January15 Server System and Configurations (EARLY ALPHA).
}

class Form : public QWidget
{
    Q_OBJECT
public:
    enum MsgBoxType
    {
        Critical,
        Warning,
        Info,
        Question
    };
public:
    explicit Form(QWidget *parent = 0);
    ~Form();

    /// TODO: Make a better way to find clients other than being this specific and cringe inducing
    //Global Vars
    QString clientName, clientExePath, clientDataPath, clientAssemblyPath; // BABSCon Build
    QString clientNameA, clientExePathA, clientDataPathA, clientAssemblyPathA; // August Build
    QString clientNameO, clientExePathO, clientDataPathO, clientAssemblyPathO; // Ohaycon Build
    QString clientNameJ, clientExePathJ, clientDataPathJ, clientAssemblyPathJ; // January Build
    int anHeroDelay;

    Ui::Form *ui;
    void externCmdLine(QString str);
    void logStatusMsg(QString str);
    void launchClient();
    void searchClientPath();

    void MsgBox(MsgBoxType type, QString title, QString text);

public slots:
    void sendCmdLine();
    void refreshPlayerList();

private slots:
    void on_btnSettings_clicked();
    void on_btnOpenGame_clicked();
    void on_lineEdit_returnPressed();
    void on_btnMapTp_clicked();
    void on_btnPlayerTp_clicked();
    void on_btnPosTp_clicked();
    void on_btnGetPos_clicked();
    void on_btnGetRot_clicked();
    void on_btnGetPonyData_clicked();
    void on_btnSetPlayer_clicked();
    void on_listPlayers_itemClicked(QListWidgetItem *item);
    void on_pushButton_clicked();
    void on_sendInputCmd_clicked();
    void on_btnStartStopServer_clicked();
    void on_pushButton_2_clicked();
    void on_btnAbout_clicked();
    void on_btnAboutQt_clicked();
    void on_btnWebsiteLink_clicked();
    void on_btnNPCManager_clicked();
public: // Vars
    QString commandString;
};
extern Form lwin;

#endif // FORM_H
