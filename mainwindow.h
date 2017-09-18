#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "filemanager.h"

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


    void on_addButton_clicked();

    void on_editButton_clicked();

    void on_removeButton_clicked();

    void on_Compile_Button_clicked();

    void on_dirButton_clicked();

    void on_Download_Button_clicked();

    void on_saveButton_clicked();

    void on_loadButton_clicked();

private:
    Ui::MainWindow *ui;
    FileManager * filemgr;
    QString defaultFullpath;

    QString workingDir();
    int     getDefaultSettings();
    void configSettings(const workspace_t &params);
    void saveDefaults();
    void getParams(workspace_t &params);
};

#endif // MAINWINDOW_H
