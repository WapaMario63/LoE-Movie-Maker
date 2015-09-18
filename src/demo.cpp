#include "demo.h"
#include "ui_demo.h"
#include "widget.h"
#include "message.h"
#include "form.h"
#include "sync.h"
#include "scene.h"

#include <QtMath>
#include <QTextStream>
#include <QFileDialog>

Demo::Demo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Demo)
{
    ui->setupUi(this);
    stopIt = false;
    isSyncOurs = false;
    demoSyncTimer = new QTimer();
    isRecording = false;
}

Demo::~Demo()
{
    delete ui;
}

void Demo::logDemo(QString msg)
{
    ui->logDemRec->appendPlainText(msg);
    ui->logDemRec->repaint();
}

void Demo::generateNpc(Player *Owner, QString filename)
{
    QString questId = QString().setNum(qFloor((qrand()*65535)+200));

    //QString npcname, npcScene, npcPonyData; // Data
    //QString posX, posY, posZ; // Position
    //QString rotX, rotY, rotZ, rotW; // Rotation
    QFile npcFile(QApplication::applicationDirPath()+"/data/npcs/"+questId+filename); // Npc File

    if (!npcFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        win.logMessage("[ERROR] Unable to generate NPC file!");
    }

    try
    {
        QTextStream npcout(&npcFile);
        npcout << "# Metadata" << endl;
        npcout << "name " << Owner->pony.name << endl;
        npcout << "questId " << questId << endl;
        npcout << "questName " << Owner->pony.name << endl;
        npcout << "questDescr " << Owner->pony.name << endl;
        npcout << "scene " << Owner->pony.sceneName << endl;
        npcout << "pos " << Owner->pony.pos.x << " " << Owner->pony.pos.y << " " << Owner->pony.pos.z << endl;
        npcout << "rot " << Owner->pony.rot.x << " " << Owner->pony.rot.y << " " << Owner->pony.rot.z << " " << Owner->pony.rot.w << endl;
        npcout << "ponyData " << Owner->pony.ponyData.toBase64() << endl;
        npcout << "# Actions" << endl;
        npcout << "say I am an NPC generated by the server. To edit me, go into /data/npcs/" << questId << filename << " and open it with notepad++." << endl;
        npcout << "end" << endl;
    }
    catch (...)
    {
        win.logMessage("[ERROR] Unable to generate NPC file!");
    }
}

void Demo::stopDemo(bool stop)
{
    if (stop) { stopIt = true; }
}

/*void Demo::switchSyncs()
{
    // This is used so we can seperately sync our player when recording our demo
    // When stopDemo is called, we connect back the original sync timer.

    // We first disconnect the old Sync timer
    disconnect(orgSync.syncTimer, SIGNAL(timeout()), Sync(), SLOT(Sync::doSync()));
    // We then connect our own sync timer so it runs on demoSync
    connect(demoSyncTimer, SIGNAL(timeout()), this, SLOT(demoSync()));
    // We have now taken control of the Sync timer, so it is now ours
    isSyncOurs = true;
}*/

void Demo::demoSync()
{
    // We know we can call Sync's doSync() function, but this is completely ours now.
    for (int i=0; i<win.scenes.size(); i++)
    {
        if (win.scenes[i].players.size()<2) continue;
        for (int j=0; j<win.scenes[i].players.size(); j++)
            for (int k=0; k<win.scenes[i].players.size(); k++)
            {
                if (j==k) continue;
                orgSync.sendSyncMessage(win.scenes[i].players[j], win.scenes[i].players[k]);
            }
    }
}

void Demo::demoSyncRecord(QString filename) // cmdPeer only
{
    int tick = 0;

    logDemo("Creating file...");
    QFile demofile(filename);
    if (!demofile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        sendChatMessage(win.cmdPeer, "Could not write demo file "+filename+".", "[LoEWCT Demo Manager]", ChatSystem);
        win.logMessage("[ERROR] Could not write demo file "+filename+".");
        logDemo("Could not write demo file "+filename+"!");
        return;
    }

    QTextStream out(&demofile);
    stopIt = false; // If true, we stop writing and save the file.

    // Generate NPC file of the player
    logDemo("Generating NPC for Set Player for debug purposes.");
    generateNpc(win.cmdPeer, win.cmdPeer->pony.name);

    // Start Writing the demo file
    // Write the positions and rotations
    logDemo("Now Recording.");

    // Only write if the timer is up and if the Sync is ours and if we haven't stopped it
    if (stopIt)
    {
        logDemo("Dang, we failed to conquer our Sync timer!");
        logDemo("Did something stop the process?");
        logDemo("Unable to record because Sync::doSync() is still used and not Demo::demoSync().");
    }
    while (!stopIt)
    {
        tick += 1; // Every position is a tick, we incease this on each loop
        // Start Writing the info in a line of each file, each loop is a new line.
        out << tick << ";" << win.cmdPeer->pony.pos.x << " " << win.cmdPeer->pony.pos.y << " " << win.cmdPeer->pony.pos.z << ";"
            << win.cmdPeer->pony.rot.x << " " << win.cmdPeer->pony.rot.y << " " << win.cmdPeer->pony.rot.z << " " << win.cmdPeer->pony.rot.w << endl;

        // Log what we are writing.
        logDemo("Tick: "+QString().setNum(tick)+
                " | Pos: "+QString().setNum(win.cmdPeer->pony.pos.x)+
                " "+QString().setNum(win.cmdPeer->pony.pos.y)+
                " "+QString().setNum(win.cmdPeer->pony.pos.z)+
                " | Rot: "+QString().setNum(win.cmdPeer->pony.rot.x)+
                " "+QString().setNum(win.cmdPeer->pony.rot.y)+
                " "+QString().setNum(win.cmdPeer->pony.rot.z)+
                " "+QString().setNum(win.cmdPeer->pony.rot.w));
        // Count how many ticks we are recording in the UI
        ui->lblTicksRecorded->setText(QString().setNum(tick));
        if (stopIt) break; // Just incase.
    }

    // BUG: after 4000+ ticks, the program freezes, but not the console
    // The demo is still recording though, but only in the position it began recording. I wish I knew how to make this not happen...
}

/*void Demo::recordDemo(Player *player, QString filename)
{
    //demoSyncRecord(player, filename);
    // These float array variables are here to fix an array compile error within the loops
    // The error is: "invalid types 'float[int]' for array subscript"
    // Thanks compiler for your bullshit, because it worked fine with other things.
    float posx[1] = { player->pony.pos.x };
    float posy[1] = { player->pony.pos.y };
    float posz[1] = { player->pony.pos.z };
    float rotx[1] = { player->pony.rot.x };
    float roty[1] = { player->pony.rot.y };
    float rotz[1] = { player->pony.rot.z };
    float rotw[1] = { player->pony.rot.w };*/

    //connect(synctimer, SIGNAL(timeout()), this, SLOT(demoSync());
    /*while (synctimer)
    {
        synctimer->start(250);
        tick += 1;
        out << tick << ";" << player->pony.pos.x << " " << player->pony.pos.y << " " << player->pony.pos.z << ";"
            << player->pony.rot.x << " " << player->pony.rot.y << " " << player->pony.rot.z << " " << player->pony.rot.w << endl;

        logDemo("Tick: "+QString().setNum(tick)+
                " | Pos: "+QString().setNum(player->pony.pos.x)+
                " "+QString().setNum(player->pony.pos.y)+
                " "+QString().setNum(player->pony.pos.z)+
                " | Rot: "+QString().setNum(player->pony.rot.x)+
                " "+QString().setNum(player->pony.rot.y)+
                " "+QString().setNum(player->pony.rot.z)+
                " "+QString().setNum(player->pony.rot.w));
        ui->lblTicksRecorded->setText(QString().setNum(tick));

        if (stopIt) { break; return; }
    }*/

    /*for (int x=0; x<player->pony.pos.x; x++)
    {
        for (int y=0; y<player->pony.pos.y; y++)
        {
            for (int z=0; z<player->pony.pos.z; z++)
            {
                for (int rx=0; rx<player->pony.rot.x; rx++)
                {
                    for (int ry=0; ry<player->pony.rot.y; ry++)
                    {
                        for (int rz=0; rz<player->pony.rot.z; rz++)
                        {
                            for (int rw=0; rw<player->pony.rot.w; rw++)
                            {
                                tick += 1;
                                out << tick << ";" << posx[x] << " " << posy[y] << " " << posz[z] << ";"
                                    << rotx[rx] << " " << roty[ry] << " " << rotz[rz] << " " << rotw[rw] << endl;

                                logDemo("Tick: "+QString().setNum(tick)+
                                        " | Pos: "+QString().setNum(posx[x])+
                                        " "+QString().setNum(posy[y])+
                                        " "+QString().setNum(posz[z])+
                                        " | Rot: "+QString().setNum(rotx[rx])+
                                        " "+QString().setNum(roty[ry])+
                                        " "+QString().setNum(rotz[rz])+
                                        " "+QString().setNum(rotw[rw]));
                                ui->lblTicksRecorded->setText(QString().setNum(tick));
                                if (stopIt) { break; return; }
                            }
                            if (stopIt) { break; return; }
                        }
                        if (stopIt) { break; return; }
                    }
                    if (stopIt) { break; return; }
                }
                if (stopIt) { break; return; }
            }
            if (stopIt) { break; return; }
        }
        if (stopIt) { break; return; }
    }
}*/

void Demo::on_btnRecord_clicked()
{
    if (win.cmdPeer->IP=="") lwin.MsgBox(Form::MsgBoxType::Warning, "Error", "No Player Set!");
    else
    {
        ui->btnStopRec->setEnabled(true);
        ui->btnRecord->setEnabled(false);
        demoSyncRecord(ui->txtDemoSavePath->text());
    }
}

void Demo::on_btnSaveRec_clicked()
{
    // fileName = QFileDialog::getOpenFileName(this,
    //tr("Open Image"), "/home/jana", tr("Image Files (*.png *.jpg *.bmp)"));
    QString demofile;
    QFileDialog saveDialog;
    saveDialog.setFileMode(QFileDialog::AnyFile);
    demofile = saveDialog.getSaveFileName(this, tr("Save LoE Demo"), QApplication::applicationDirPath(), tr("LoE Demo Files (*.loedemo)"));
    ui->txtDemoSavePath->setText(demofile);
}

void Demo::on_btnStopRec_clicked()
{
    ui->btnStopRec->setEnabled(false);
    ui->btnRecord->setEnabled(true);
    stopDemo(true);
}
