#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    serverState = 0;
    ui->setupUi(this);

    ui->mainToolBar->addAction(ui->menuMousoid->addAction(QIcon::fromTheme("exit"), tr("Exit"), qApp, SLOT(quit()), QKeySequence("CTRL+q")));
    ui->mainToolBar->addAction(ui->menuMousoid->addAction(QIcon::fromTheme(""), tr("Close to systray"), this, SLOT(close()), QKeySequence("ALT+F4")));

    connect(ui->buttonToggle, SIGNAL(clicked()), this, SLOT(toggleServer()));
    connect(ui->buttonApply, SIGNAL(clicked()), this, SLOT(applyChanges()));

    /// @todo init gui signals and slots

    /// @todo name itt és ott
    MousoidCore::create();
    connect(MousoidCore::self(), SIGNAL(clientConnected(QString&)), this, SLOT(addClient(QString&)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::toggleServer()
{
    serverState = serverState ^ Mousoid::SERVER_ENABLED;
    if(serverState & Mousoid::SERVER_ENABLED){
        ui->buttonToggle->setText(tr("Stop server"));
    }else{
        ui->buttonToggle->setText(tr("Start server"));
    }
    MousoidCore::self()->changeServerState(serverState);
}

void MainWindow::applyChanges()
{
    ui->buttonApply->setDisabled(true);
    MousoidCore::self()->changeServerState(serverState);
}

void MainWindow::changeSettings()
{
    ui->buttonApply->setEnabled(true);
    /// @todo
//    if(ui->multipleCheck->isChecked()){
//        serverState |= Mousoid::MORE_ALLOWED;
//    } else {
//        serverState &= ~Mousoid::MORE_ALLOWED;
//        if(ui->bothRadio->isChecked()){
//            ui->wirelessRadio->setChecked(true);
//            return;
//        }
//    }

//    if(ui->wirelessRadio->isChecked())
//        serverState |= Mousoid::WIRELESS_ON;
//    else
//        serverState &= Mousoid::WIRELESS_ON;


//    if(ui->bluetotthRadio->isChecked())
//        serverState |= Mousoid::BLUETOOTH_ON;
//    else
//        serverState &= Mousoid::BLUETOOTH_ON;


//    if(ui->bothRadio->isChecked()){
//        serverState |= Mousoid::WIRELESS_ON;
//        serverState |= Mousoid::BLUETOOTH_ON;
//    }

}

void MainWindow::addClient(QString &name)
{
    /// @todo
}

