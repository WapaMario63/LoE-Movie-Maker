#include "NpcManager.h"
#include "ui_NpcManager.h"

#include <QtMath>
#include <QFile>
#include <QTextStream>
#include "demo.h" // temporary
#include "widget.h"
#include "form.h"

NpcManager::NpcManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NpcManager)
{
    ui->setupUi(this);
}

NpcManager::~NpcManager()
{
    delete ui;
}

void NpcManager::on_cbxGenNPCself_stateChanged(int arg1)
{
    if (arg1 == 0)
    {
        ui->cbxGenNpcAccs->setEnabled(true);
        ui->cbxGenNpcPonyData->setEnabled(true);
        ui->cbxGenNpcPosRot->setEnabled(true);
        //ui->cbxGenNpcMeters->setEnabled(true);
    }
    else if (arg1 == 2)
    {
        ui->cbxGenNpcAccs->setEnabled(false);
        ui->cbxGenNpcPonyData->setEnabled(false);
        ui->cbxGenNpcPosRot->setEnabled(false);
        //ui->cbxGenNpcMeters->setEnabled(false);
    }
}

/*void NpcManager::on_cbxGenNpcMeters_stateChanged(int arg1)
{
    if (arg1 == 2)
    {
        ui->sbxNpcMeters->setEnabled(true);
        ui->cbxNpcMeterX->setEnabled(true);
        ui->cbxNpcMeterY->setEnabled(true);
        ui->cbxNpcMeterZ->setEnabled(true);
        ui->lblMeterCords->setEnabled(true);
        ui->lblMetersInThe->setEnabled(true);
    }
    else if (arg1 == 0)
    {
        ui->sbxNpcMeters->setEnabled(false);
        ui->cbxNpcMeterX->setEnabled(false);
        ui->cbxNpcMeterY->setEnabled(false);
        ui->cbxNpcMeterZ->setEnabled(false);
        ui->lblMeterCords->setEnabled(false);
        ui->lblMetersInThe->setEnabled(false);
    }
}*/

void NpcManager::on_cbxGenNpcPonyData_stateChanged(int arg1)
{
    if (arg1 == 2) ui->txtNpcPonyData->setEnabled(true);
    else if (arg1 == 0) ui->txtNpcPonyData->setEnabled(false);
}

void NpcManager::on_cbxGenNpcPosRot_stateChanged(int arg1)
{
    if (arg1 == 2)
    {
        ui->txtNpcXPos->setEnabled(true);
        ui->txtNpcYPos->setEnabled(true);
        ui->txtNpcZPos->setEnabled(true);
        ui->txtNpcXRot->setEnabled(true);
        ui->txtNpcYRot->setEnabled(true);
        ui->txtNpcZRot->setEnabled(true);
        ui->txtNpcWRot->setEnabled(true);
    }
    else if (arg1 == 0)
    {
        ui->txtNpcXPos->setEnabled(false);
        ui->txtNpcYPos->setEnabled(false);
        ui->txtNpcZPos->setEnabled(false);
        ui->txtNpcXRot->setEnabled(false);
        ui->txtNpcYRot->setEnabled(false);
        ui->txtNpcZRot->setEnabled(false);
        ui->txtNpcWRot->setEnabled(false);
    }
}

void NpcManager::on_cbxGenNpcAccs_stateChanged(int arg1)
{
    if (arg1 == 2) ui->txtItemIds->setEnabled(true);
    else if (arg1 == 0) ui->txtItemIds->setEnabled(false);
}

void NpcManager::on_btnGenerateNpc_clicked()
{
    win.logMessage("[INFO] Gathering inputed NPC data for generation...");
    if (win.cmdPeer->IP == "")
    {
        win.logMessage("[ERROR] No player selected!");

        lwin.MsgBox(Form::Warning, "Error", "No Player Set!");
        return;
    }

    Player* player = win.cmdPeer;
    QString PoneName;
    QString PoneData, PoneItems;
    float PosX, PosY, PosZ, RotX, RotY, RotZ, RotW;
    //float meters = 0;

    if (ui->txtNpcName->text().isEmpty())
    {
        PoneName = player->pony.name;
    }
    else
    {
        PoneName = ui->txtNpcName->text();
    }

    /*if (ui->cbxGenNpcMeters->isChecked())
    {
        meters = ui->sbxNpcMeters->text().toFloat();
    }
    else meters = 0;*/

    if (ui->cbxGenNpcPonyData->isChecked())
        PoneData = ui->txtNpcPonyData->text();
    else PoneData = player->pony.ponyData.toBase64();

    if (ui->cbxGenNpcPosRot->isChecked())
    {
        PosX = ui->txtNpcXPos->text().toFloat();
        PosY = ui->txtNpcYPos->text().toFloat();
        PosZ = ui->txtNpcZPos->text().toFloat();
        RotX = ui->txtNpcXRot->text().toFloat();
        RotY = ui->txtNpcYRot->text().toFloat();
        RotZ = ui->txtNpcZRot->text().toFloat();
        RotW = ui->txtNpcWRot->text().toFloat();
    }
    else
    {
        PosX = player->pony.pos.x;
        PosY = player->pony.pos.y;
        PosZ = player->pony.pos.z;
        RotX = player->pony.rot.x;
        RotY = player->pony.rot.y;
        RotZ = player->pony.rot.z;
        RotW = player->pony.rot.w;
    }

    if (ui->cbxGenNpcAccs->isChecked())
    {
        PoneItems = ui->txtItemIds->text();
    }
    else
    {
        PoneItems = ""; // Until I find a way to get a Pony's items, this will be empty.
    }

    if (ui->cbxGenNPCself->isChecked())
    {
        win.logMessage("[INFO] Generating NPC for the selected player.");
        // Use a Preset Npc generation function that is used for generating an NPC of a connected player
        Demo dem;
        dem.generateNpc(player, PoneName);
    }
    else // Proceed the more indepth Npc Generation algorithm
    {
        win.logMessage("[INFO] Generating NPC using gathered data...");

        win.logMessage("[INFO] Generating questId...");
        QString questId = QString().setNum(qFloor((qrand()*65535)+200));

        win.logMessage("[INFO] Creating NPC file...");
        QFile npcFile(QApplication::applicationDirPath()+"/data/npcs/"+questId+PoneName); // Npc File
        if (!npcFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            win.logMessage("[ERROR] Unable to create NPC file!");
        }

        win.logMessage("[INFO] Writing NPC File with gathered information...");
        try
        {
            QTextStream npcout(&npcFile);

            npcout << "# Metadata" << endl;
            npcout << "name " << PoneName << endl;
            npcout << "questId " << questId << endl;
            npcout << "questName " << PoneName << endl;
            npcout << "questDescr " << PoneName << endl;
            npcout << "scene " << player->pony.sceneName << endl;
            npcout << "pos " << PosX << " " << PosY << " " << PosZ << endl;
            npcout << "rot " << RotX << " " << RotY << " " << RotZ << " " << RotW << endl;
            npcout << "ponyData " << PoneData << endl;
            if (!PoneItems.isEmpty()) npcout << "wear " << PoneItems << endl;
            npcout << "# Actions" << endl;
            npcout << "say I am an NPC generated by the server. To edit me, go into /data/npcs/" << questId << PoneName << " and open it with notepad++." << endl;
            npcout << "end" << endl;
        }
        catch (...)
        {
            win.logMessage("[ERROR] Unable to write NPC file! Aborting!");
            return;
        }

        // Refresh the scene and keep the player on it's current pos
        sendLoadSceneRPC(player, player->pony.sceneName, UVector(player->pony.pos.x,player->pony.pos.y,player->pony.pos.z));

    }
}

void NpcManager::GenerateNPC()
{
    on_btnGenerateNpc_clicked();
}
