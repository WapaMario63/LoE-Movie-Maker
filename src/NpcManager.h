#ifndef NPCMANAGER_H
#define NPCMANAGER_H

#include <QWidget>
#include "character.h"

namespace Ui {
class NpcManager;
}

class NpcManager : public QWidget
{
    Q_OBJECT

public:
    explicit NpcManager(QWidget *parent = 0);
    ~NpcManager();

    void GenerateNPC();

private:
    Ui::NpcManager *ui;

private slots:
    void on_cbxGenNPCself_stateChanged(int arg1);

    void on_cbxGenNpcMeters_stateChanged(int arg1);

    void on_cbxGenNpcPonyData_stateChanged(int arg1);

    void on_cbxGenNpcPosRot_stateChanged(int arg1);

    void on_cbxGenNpcAccs_stateChanged(int arg1);

    void on_btnGenerateNpc_clicked();
};

#endif // NPCMANAGER_H
