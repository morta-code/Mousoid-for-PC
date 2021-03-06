#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QSystemTrayIcon>
#include <QMainWindow>
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

    void addNewClient(QString &name, QString &address);

public slots:
    void toggleServer();
    void toggleWindow();
    void applyChanges();
    void onSettingsChanged();
    void quit();
    void onListWidgetSelectionChanged();
    void allowSelected();
    void denySelected();
    void removeSelected();
    void hideServer();

private:
    void closeEvent(QCloseEvent *event);
    void initializeWindow();
    void initializeSysTray();
    void saveSettings();
    void loadSettings();

    Ui::MainWindow *ui;
    uchar serverState;
    uchar ethernetLimitations;
    bool showCloseNotification;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    QAction *actionToggleWindow;
    QAction *actionToggleServer;
    QAction *actionQuit;

    QIcon questionIcon;
    QIcon allowedIcon;
    QIcon deniededIcon;
    QIcon quitIcon;
    QIcon hideIcon;
    QIcon restoreIcon;

};

#endif // MAINWINDOW_HPP
