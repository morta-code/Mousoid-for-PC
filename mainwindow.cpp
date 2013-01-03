#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <QSettings>

// Global:

MainWindow *instance = 0;

void new_client(char* _name, char* _address){
    QString name(_name);
    QString address(_address);
    instance->addNewClient(name, address);
}


// Public members:

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    instance = this;
    serverState = Mousoid::WIRELESS_ON;
    showCloseNotification = true;

    questionIcon = QIcon::fromTheme("help-contents");
    allowedIcon = QIcon::fromTheme("dialog-ok");
    deniededIcon = QIcon::fromTheme("stop");
    quitIcon = QIcon::fromTheme("exit");
    hideIcon = QIcon::fromTheme("window-close");
    restoreIcon = QIcon::fromTheme("view-restore");

    MousoidCore::create();
    MousoidCore::funcForNewClient(new_client);
    initializeWindow();
    initializeSysTray();
    loadSettings();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addNewClient(QString &name, QString &address)
{
    QString a(name+" ("+address+")");
    if(ui->notifyCheckBox->isChecked()){
        trayIcon->showMessage(tr("Mousoid"), a + tr(" connected"), QSystemTrayIcon::Information);
    }

    QList<QListWidgetItem*> l = ui->listAllowed->findItems(address, Qt::MatchContains);
    if(l.isEmpty()){
        QListWidgetItem *item = new QListWidgetItem(a, ui->listAllowed);
        item->setIcon(questionIcon);
        ui->listAllowed->insertItem(0, item);
    } else {
        l.first()->setText(a);
    }
}


// Private members:

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible() && showCloseNotification) {
        if(QMessageBox::information(this, tr("Mousoid"),
                                 tr("The program will keep running in the "
                                    "system tray. To terminate the program, "
                                    "choose <b>Exit</b> in the context menu "
                                    "of the system tray entry."), tr("OK"), tr("OK, never show again"))
                == 1){
            showCloseNotification = false;
        }
        toggleWindow();
        event->ignore();
    }
}

void MainWindow::initializeWindow()
{
    ui->setupUi(this);

    actionQuit = new QAction(quitIcon, tr("Exit"), this);
    actionToggleWindow = new QAction(hideIcon, tr("Hide window"), this);
    ui->mainToolBar->addAction(actionQuit);
    ui->mainToolBar->addAction(actionToggleWindow);

    connect(actionToggleWindow, SIGNAL(triggered()), this, SLOT(toggleWindow()));
    connect(actionQuit, SIGNAL(triggered()), this, SLOT(quit()));
    connect(ui->buttonToggle, SIGNAL(clicked()), this, SLOT(toggleServer()));
}

void MainWindow::initializeSysTray()
{
    trayIconMenu = new QMenu(this);

    actionToggleServer = new QAction(tr("Start server"), this);
    connect(actionToggleServer, SIGNAL(triggered()), this, SLOT(toggleServer()));

    trayIconMenu->addAction(actionToggleWindow);
    trayIconMenu->addAction(actionToggleServer);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(actionQuit);

    trayIcon = new QSystemTrayIcon(QIcon(":/mousoid/icon.png"), this);
    QByteArray category = qgetenv("SNI_CATEGORY");
    if (!category.isEmpty()) {
        trayIcon->setProperty("_qt_sni_category", QString::fromLocal8Bit(category));
    }
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->show();
}

void MainWindow::saveSettings()
{
    QSettings settings("morta-code", "Mousoid");
    settings.beginGroup("Server");
    settings.setValue("limitation", ethernetLimitations);
    settings.setValue("hidden", ui->hiddenCheckBox->isChecked());
    QList<QListWidgetItem*> l = ui->listAllowed->findItems(" ", Qt::MatchContains);
    QString val_b;
    QString val_a;
    foreach (QListWidgetItem *i, l) {
        if(i->icon().name() == allowedIcon.name())
            val_a.append(i->text() + "%%%%");
        if(i->icon().name() == deniededIcon.name())
            val_b.append(i->text() + "%%%%");
    }
    settings.setValue("blocked", val_b);
    settings.setValue("allowed", val_a);
    settings.endGroup();

    settings.beginGroup("Window");
    settings.setValue("geometry", this->geometry());
    settings.setValue("closenotification", showCloseNotification);
    settings.setValue("systrayNotify", ui->notifyCheckBox->isChecked());
    settings.endGroup();
}

void MainWindow::loadSettings()
{
    QSettings settings("morta-code", "Mousoid");
    settings.beginGroup("Server");
    if(settings.value("hidden", false).toBool()){
        ui->hiddenCheckBox->setChecked(true);
    }

    ethernetLimitations = (settings.value("limitation", Mousoid::ONLY_FROM_SET_ALLOWED).toInt());
    switch (ethernetLimitations) {
    case Mousoid::NO_LIMITATION:
        ui->radioAll->setChecked(true);
        break;
    case Mousoid::ONLY_FROM_SET_ALLOWED:
        ui->radioAllowed->setChecked(true);
        break;
    case Mousoid::ONLY_FROM_SET_BLOCKED:
        ui->radioBlocked->setChecked(true);
        break;
    }
    QString val_b = settings.value("blocked", QString()).toString();
    QString val_a = settings.value("allowed", QString()).toString();

    foreach (QString str, val_a.split("%%%%")) {
        if(str.isEmpty())
            continue;
        QListWidgetItem *item = new QListWidgetItem(str, ui->listAllowed);
        item->setIcon(allowedIcon);
        ui->listAllowed->insertItem(0, item);
        str.remove(0, str.lastIndexOf('(')+1).chop(1);
        MousoidCore::addToAllowed(str);
    }
    foreach (QString str, val_b.split("%%%%")) {
        if(str.isEmpty())
            continue;
        QListWidgetItem *item = new QListWidgetItem(str, ui->listAllowed);
        item->setIcon(deniededIcon);
        ui->listAllowed->insertItem(0, item);
        str.remove(0, str.lastIndexOf('(')+1).chop(1);
        MousoidCore::addToBlocked(str);
    }

    settings.endGroup();

    settings.beginGroup("Window");
    setGeometry(settings.value("geometry", QRect(10, 10, 490, 345)).toRect());
    showCloseNotification = settings.value("closenotification", true).toBool();
    ui->notifyCheckBox->setChecked(settings.value("systrayNotify", true).toBool());
    settings.endGroup();
}


// Slots:

void MainWindow::toggleServer()
{
    serverState ^= Mousoid::SERVER_ENABLED;
    if(serverState & Mousoid::SERVER_ENABLED){
        ui->buttonToggle->setText(tr("&Stop server"));
        actionToggleServer->setText(tr("&Stop server"));
    }else{
        ui->buttonToggle->setText(tr("&Start server"));
        actionToggleServer->setText(tr("&Start server"));
    }
    MousoidCore::changeLimitations(ethernetLimitations);
    MousoidCore::changeServerState(serverState);
}

void MainWindow::toggleWindow()
{
    if(this->isHidden()){
        actionToggleWindow->setText(tr("Hide window"));
        actionToggleWindow->setIcon(hideIcon);
        showNormal();
    }
    else{
        actionToggleWindow->setText(tr("Restore window"));
        actionToggleWindow->setIcon(restoreIcon);
        hide();
    }
}

void MainWindow::applyChanges()
{
    MousoidCore::changeLimitations(ethernetLimitations);
    MousoidCore::changeServerState(serverState);
}

void MainWindow::onSettingsChanged()
{
    if(ui->radioAll->isChecked()){
        ethernetLimitations = Mousoid::NO_LIMITATION;
    }else if (ui->radioAllowed->isChecked()) {
        ethernetLimitations = Mousoid::ONLY_FROM_SET_ALLOWED;
    }else if (ui->radioBlocked->isChecked()) {
        ethernetLimitations = Mousoid::ONLY_FROM_SET_BLOCKED;
    }
    applyChanges();
}

void MainWindow::quit()
{
    saveSettings();
    MousoidCore::destroy();
    qApp->quit();
}

void MainWindow::onListWidgetSelectionChanged()
{
    bool b = ui->listAllowed->selectedItems().length() > 0;
    ui->pushButton_deny->setEnabled(b);
    ui->pushButton_allow->setEnabled(b);
    ui->pushButton_remove->setEnabled(b);
}

void MainWindow::allowSelected()
{
    QListWidgetItem *item = ui->listAllowed->currentItem();
    QString addr = item->text();
    addr.remove(0, addr.lastIndexOf('(')+1).chop(1);
    MousoidCore::addToAllowed(addr);
    item->setIcon(allowedIcon);
}

void MainWindow::denySelected()
{
    QListWidgetItem *item = ui->listAllowed->currentItem();
    QString addr = item->text();
    addr.remove(0, addr.lastIndexOf('(')+1).chop(1);
    MousoidCore::addToBlocked(addr);
    item->setIcon(deniededIcon);
}

void MainWindow::removeSelected()
{
    QListWidgetItem *item = ui->listAllowed->currentItem();
    QString addr = item->text();
    addr.remove(0, addr.lastIndexOf('(')+1).chop(1);
    MousoidCore::removeFromSets(addr);
    ui->listAllowed->removeItemWidget(item);
    delete item;
}

void MainWindow::hideServer()
{
    serverState ^= Mousoid::HIDDEN_MODE;
    MousoidCore::changeServerState(serverState);
}

