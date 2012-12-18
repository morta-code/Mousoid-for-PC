#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    serverState = 0;
    ui->setupUi(this);

    ui->mainToolBar->addAction(ui->menuMousoid->addAction(QIcon::fromTheme(""), tr("Close to systray"), this, SLOT(close()), QKeySequence("ALT+F4")));
    ui->mainToolBar->addAction(ui->menuMousoid->addAction(QIcon::fromTheme("exit"), tr("Exit"), qApp, SLOT(quit()), QKeySequence("CTRL+q")));

    connect(ui->startstopButton, SIGNAL(clicked()), this, SLOT(toggleServer()));
    connect(ui->applyButton, SIGNAL(clicked()), this, SLOT(applyChanges()));

    connect(ui->multipleCheck, SIGNAL(toggled(bool)), this, SLOT(changeSettings()));
    connect(ui->wirelessRadio, SIGNAL(toggled(bool)), this, SLOT(changeSettings()));
    connect(ui->bluetotthRadio, SIGNAL(toggled(bool)), this, SLOT(changeSettings()));
    connect(ui->bothRadio, SIGNAL(toggled(bool)), this, SLOT(changeSettings()));

    MousoidCore::create();
    connect(MousoidCore::self(), SIGNAL(clientConnected(QString&)), this, SLOT(addClient(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::toggleServer()
{
    serverState = serverState ^ Mousoid::SERVER_ENABLED;
    if(serverState & Mousoid::SERVER_ENABLED){
        ui->startstopButton->setText(tr("Stop"));
    }else{
        ui->startstopButton->setText(tr("Start"));
    }
    MousoidCore::self()->changeServer(serverState);
}

void MainWindow::applyChanges()
{
    ui->applyButton->setDisabled(true);
    MousoidCore::self()->changeServer(serverState);
}

void MainWindow::changeSettings()
{
    ui->applyButton->setEnabled(true);
    if(ui->multipleCheck->isChecked()){
        serverState |= Mousoid::MORE_ALLOWED;
    } else {
        serverState &= ~Mousoid::MORE_ALLOWED;
        if(ui->bothRadio->isChecked()){
            ui->wirelessRadio->setChecked(true);
            return;
        }
    }

    if(ui->wirelessRadio->isChecked())
        serverState |= Mousoid::WIRELESS_ON;
    else
        serverState &= Mousoid::WIRELESS_ON;


    if(ui->bluetotthRadio->isChecked())
        serverState |= Mousoid::BLUETOOTH_ON;
    else
        serverState &= Mousoid::BLUETOOTH_ON;


    if(ui->bothRadio->isChecked()){
        serverState |= Mousoid::WIRELESS_ON;
        serverState |= Mousoid::BLUETOOTH_ON;
    }

}

void MainWindow::addClient(QString name)
{
    /// @todo
}

