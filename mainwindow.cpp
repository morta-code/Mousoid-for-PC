#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <QDebug>

void fakeaddclient(QString& str){
    qDebug() << "Client connected" << str;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    serverState = Mousoid::WIRELESS_ON;
    ethernetLimitations = Mousoid::NO_LIMITATION;
    ui->setupUi(this);

    ui->mainToolBar->addAction(ui->menuMousoid->addAction(QIcon::fromTheme("exit"), tr("Exit"), this, SLOT(quit()), QKeySequence("CTRL+q")));
    ui->mainToolBar->addAction(ui->menuMousoid->addAction(QIcon::fromTheme(""), tr("Close to systray"), this, SLOT(close()), QKeySequence("ALT+F4")));

    connect(ui->buttonToggle, SIGNAL(clicked()), this, SLOT(toggleServer()));
    connect(ui->buttonApply, SIGNAL(clicked()), this, SLOT(applyChanges()));

    /// @todo init gui signals and slots

    /// @todo name itt és ott
    MousoidCore::create();
    qDebug() << "Server created";
    MousoidCore::funcForNewClient(fakeaddclient);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::toggleServer()
{
    serverState ^= Mousoid::SERVER_ENABLED;
    if(serverState & Mousoid::SERVER_ENABLED){
        ui->buttonToggle->setText(tr("Stop server"));
    }else{
        ui->buttonToggle->setText(tr("Start server"));
    }
    MousoidCore::changeServerState(serverState);
}

void MainWindow::applyChanges()
{
    ui->buttonApply->setDisabled(true);
    MousoidCore::changeServerState(serverState);
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
