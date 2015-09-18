#ifndef DEMO_H
#define DEMO_H

#include <QWidget>
#include <QObject>
#include "sync.h"
#include "character.h"

namespace Ui {
class Demo;
}

//class Player;

class Demo : public QWidget
{
    Q_OBJECT

public:
    explicit Demo(QWidget *parent = 0);
    ~Demo();

    void generateNpc(Player* player, QString filename);
    void recordDemo(Player* player, QString filename);
    void stopDemo(bool stop);

    // vars
    bool isRecording;
    bool isSyncOurs;
    QTimer* demoSyncTimer;
    Sync orgSync;
public slots:
    //void switchSyncs();
    void logDemo(QString msg);
    void demoSync();
    void demoSyncRecord(QString filename);
    //QString writeToDemoFile();
private slots:
    void on_btnRecord_clicked();

    void on_btnSaveRec_clicked();

    void on_btnStopRec_clicked();

private:
    Ui::Demo *ui;
    bool stopIt;
};

extern Demo dem;

#endif // DEMO_H
