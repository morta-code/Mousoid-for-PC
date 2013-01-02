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
    ethernetLimitations = Mousoid::ONLY_FROM_SET_ALLOWED;
    showCloseNotification = true;
    /// @todo init gui signals and slots

    /// @todo name itt és ott
    MousoidCore::create();
    MousoidCore::funcForNewClient(new_client);

    initializeWindow();
    initializeSysTray();
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
        item->setIcon(QIcon::fromTheme("help-contents"));
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

    actionQuit = new QAction(QIcon::fromTheme("exit"), tr("Exit"), this);
    actionToggleWindow = new QAction(QIcon::fromTheme("window-close"), tr("Hide window"), this);
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

void MainWindow::refreshLimitations()
{
    // todo
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
    MousoidCore::changeServerState(serverState);
    MousoidCore::changeLimitations(ethernetLimitations);
}

void MainWindow::toggleWindow()
{
    if(this->isHidden()){
        actionToggleWindow->setText(tr("Hide window"));
        actionToggleWindow->setIcon(QIcon::fromTheme("window-close"));
        showNormal();
    }
    else{
        actionToggleWindow->setText(tr("Restore window"));
        actionToggleWindow->setIcon(QIcon::fromTheme("view-restore"));
        hide();
    }
}

void MainWindow::applyChanges()
{
    /// @todo getState getLimitations
    MousoidCore::changeServerState(serverState);
    MousoidCore::changeLimitations(ethernetLimitations);
}

void MainWindow::onSettingsChanged()
{
//    if(ui->checkEthernet->isChecked()){
//        serverState |= Mousoid::WIRELESS_ON;
//    }
//    else{
//        serverState &= ~Mousoid::WIRELESS_ON;
//    }
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
    /// @todo write settings
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
    item->setIcon(QIcon::fromTheme("dialog-ok"));
}

void MainWindow::denySelected()
{
    QListWidgetItem *item = ui->listAllowed->currentItem();
    QString addr = item->text();
    addr.remove(0, addr.lastIndexOf('(')+1).chop(1);
    MousoidCore::addToBlocked(addr);
    item->setIcon(QIcon::fromTheme("stop"));
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

