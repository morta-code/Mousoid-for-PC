#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QSystemTrayIcon>
#include <QMainWindow>
#include <QDebug>
#include <QMessageBox>
#include <QCloseEvent>
#include <mousoidcore.hpp>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void showConnectedNotification(QString &name);

protected:
    void closeEvent(QCloseEvent *event);

public slots:
    void toggleServer();
    void toggleWindow();
    void applyChanges();
    void changeSettings();
    void quit();

private:
    void initializeWindow();
    void initializeSysTray();

    Ui::MainWindow *ui;
    uchar serverState;
    uchar ethernetLimitations;
    bool showCloseNotification;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    QAction *actionToggleWindow;
    QAction *actionToggleServer;
    QAction *actionQuit;
};

#endif // MAINWINDOW_HPP
