#include "modmanager.h"
#include "ui_modmanager.h"

#include <QMessageBox>
#include <QSettings>

ModManager::ModManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModManager)
{
    ui->setupUi(this);

}

ModManager::~ModManager()
{
    delete ui;
}

void ModManager::AddRoot(QString name, QString version, QString author)
{
    QTreeWidgetItem *itm = new QTreeWidgetItem(ui->treeModList);
    itm->setText(0, name);
    itm->setText(1, version);
    itm->setText(2, author);
    ui->treeModList->addTopLevelItem(itm);
}

void ModManager::AddChild(QTreeWidgetItem *parent, QString name, QString version, QString author)
{

}

void ModManager::AddModDir()
{
    QDir moddir = QApplication::applicationDirPath();
    if (!moddir.exists("resourepack"))
    {
        QMessageBox::warning(this, "No Directory", "Directory 'resourepack' not found. Creating it. \nMake sure to throw in some mods in there and refresh!");
        moddir.mkdir("resourepack");
    }
    else
    {

        QSettings modini(moddir.filePath("respack.ini"), QSettings::IniFormat);
        QString packName = modini.value("packName", "My Mod").toString();
        QString packVersion = modini.value("packVersion", "v1.0").toString();
        QString packAuthor = modini.value("packAuthor", "A Pone").toString();

        AddRoot(packName, packVersion, packAuthor);
    }
}
