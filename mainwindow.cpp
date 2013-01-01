#include "mainwindow.hpp"
#include "ui_mainwindow.h"

MainWindow *instance = 0;

void new_client(char* str){
    QString name(str);
    instance->showConnectedNotification(name);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    instance = this;
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

void MainWindow::showConnectedNotification(QString &name)
{
    trayIcon->showMessage(tr("Mousoid"), name + tr(" connected"), QSystemTrayIcon::Information);
}

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
    serverState = Mousoid::WIRELESS_ON;
    ethernetLimitations = Mousoid::NO_LIMITATION;
    ui->setupUi(this);

    actionQuit = new QAction(QIcon::fromTheme("exit"), tr("Exit"), this);
    actionToggleWindow = new QAction(QIcon::fromTheme("window-close"), tr("Hide window"), this);
    ui->mainToolBar->addAction(actionQuit);
    ui->mainToolBar->addAction(actionToggleWindow);

    connect(actionToggleWindow, SIGNAL(triggered()), this, SLOT(toggleWindow()));
    connect(actionQuit, SIGNAL(triggered()), this, SLOT(quit()));
    connect(ui->buttonToggle, SIGNAL(clicked()), this, SLOT(toggleServer()));
    connect(ui->buttonApply, SIGNAL(clicked()), this, SLOT(applyChanges()));
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

void MainWindow::toggleServer()
{
    serverState ^= Mousoid::SERVER_ENABLED;
    if(serverState & Mousoid::SERVER_ENABLED){
        ui->buttonToggle->setText(tr("Stop server"));
        actionToggleServer->setText(tr("Stop server"));
    }else{
        ui->buttonToggle->setText(tr("Start server"));
        actionToggleServer->setText(tr("Start server"));
    }
    MousoidCore::changeServerState(serverState);
    MousoidCore::changeLimitations(ethernetLimitations);
    ui->buttonApply->setDisabled(true);
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
    ui->buttonApply->setDisabled(true);
    MousoidCore::changeServerState(serverState);
    MousoidCore::changeLimitations(ethernetLimitations);
}

void MainWindow::changeSettings()
{
    ui->buttonApply->setEnabled(true);
    if(ui->checkEthernet->isChecked()){
        serverState |= Mousoid::WIRELESS_ON;
    }
    else{
        serverState &= ~Mousoid::WIRELESS_ON;
    }
    if(ui->checkBluetooth->isChecked()){
        serverState |= Mousoid::BLUETOOTH_ON;
    }
    else{
        serverState &= ~Mousoid::BLUETOOTH_ON;
    }
    if(ui->radioAll->isChecked()){
        ethernetLimitations = Mousoid::NO_LIMITATION;
    }else if (ui->radioOne->isChecked()) {
        ethernetLimitations = Mousoid::ONLY_ONE_ALLOWED;
    }else if (ui->radioAllowed->isChecked()) {
        ethernetLimitations = Mousoid::ONLY_FROM_SET_ALLOWED;
    }else if (ui->radioBlocked->isChecked()) {
        ethernetLimitations = Mousoid::ONLY_FROM_SET_BLOCKED;
    }

}

void MainWindow::quit()
{
    /// @todo write settings
    MousoidCore::destroy();
    qApp->quit();
}

