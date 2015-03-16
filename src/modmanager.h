#ifndef MODMANAGER_H
#define MODMANAGER_H

#include <QWidget>
#include <QtCore>
#include <QtGui>

class QTreeWidgetItem;

namespace Ui {
class ModManager;
}

class ModManager : public QWidget
{
    Q_OBJECT

public:
    explicit ModManager(QWidget *parent = 0);
    ~ModManager();

    void AddRoot(QString name, QString version, QString author);
    void AddChild(QTreeWidgetItem *parent, QString name, QString version, QString author);

    void AddModDir();

private:
    Ui::ModManager *ui;
};

#endif // MODMANAGER_H
