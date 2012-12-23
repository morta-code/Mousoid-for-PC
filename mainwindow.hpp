#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
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

    void addClient(QString &name){
        ;
    }

public slots:
    void toggleServer();
    void applyChanges();
    void changeSettings();
    void quit();

private:    
    Ui::MainWindow *ui;
    uchar serverState;
    uchar ethernetLimitations;
};

#endif // MAINWINDOW_HPP
