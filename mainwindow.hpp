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
    
private slots:
    void toggleServer();
    void applyChanges();
    void changeSettings();
    void addClient(QString name);

private:
    Ui::MainWindow *ui;
    uchar serverState;
};

#endif // MAINWINDOW_HPP
