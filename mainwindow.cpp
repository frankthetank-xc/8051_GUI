#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QProcess>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    filemgr = new FileManager();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete filemgr;
}

void MainWindow::on_addButton_clicked()
{
    bool ok;
    QString fullpath = filemgr->getCFile(ok,this,workingDir());
    if(ok)
    {
        ui->sourceEdit->addItem(fullpath);
    }
}

void MainWindow::on_editButton_clicked()
{
    bool ok;
    // Avoid glitches with empty lists
    if(ui->sourceEdit->count() < 1)
        return;
    // Make sure an item is selected
    if(ui->sourceEdit->selectedItems().size() < 1)
        return;
    // Get selected item
    QListWidgetItem *selected = ui->sourceEdit->selectedItems().first();
    ui->sourceEdit->setCurrentItem(selected);
    // Prompt for a new item
    QString fullpath = filemgr->getCFile(ok,this,selected->text());
    if(ok)
    {
        ui->sourceEdit->currentItem()->setText(fullpath);
    }
}

void MainWindow::on_removeButton_clicked()
{
    // Avoid glitches with empty lists
    if(ui->sourceEdit->count() < 1)
        return;
    // Make sure an item is selected
    if(ui->sourceEdit->selectedItems().size() < 1)
        return;
    // Get selected item
    QListWidgetItem *selected = ui->sourceEdit->selectedItems().first();
    ui->sourceEdit->setCurrentItem(selected);
    // Remove that item
    if(ui->sourceEdit->currentRow() < ui->sourceEdit->count() && ui->sourceEdit->currentRow() >= 0)
        ui->sourceEdit->takeItem(ui->sourceEdit->currentRow());
}

void MainWindow::on_Compile_Button_clicked()
{
    // Make a new QProcess for sdcc
    QProcess process;
    QString program = "sdcc";
    // Compile a list of arguments
    QStringList arguments;
    arguments.append(ui->flagsEdit->text());
    for(int i = 0; i < ui->sourceEdit->count(); i++)
        arguments.append( ui->sourceEdit->item(i)->text());
    // Get the right working directory
    QString dir = workingDir();
    process.setWorkingDirectory(dir);
    // Move to the tab with the terminal
    ui->controlTabs->setCurrentIndex(ui->controlTabs->count() - 1);
    ui->terminalEdit->clear();
    // Run sdcc
    process.start(program, arguments);
    process.waitForFinished();
    // Grab  sdcc output and put it in the terminal box
    QString out(process.readAll());
    ui->terminalEdit->appendPlainText(out);
}

void MainWindow::on_dirButton_clicked()
{
    bool ok;
    QString fullpath = filemgr->getDir(ok, this);
    if(ok)
    {
        ui->dirEdit->setText(fullpath);
    }
}

void MainWindow::on_Download_Button_clicked()
{
    bool ok = false;
    // Get a hex file (For now just uses first in directory)
    QString fullpath = filemgr->getHex(ok,this,workingDir());
    if(ok)
    {
        ui->terminalEdit->appendPlainText(fullpath);
        // Connect to ec3 module
        QProcess ec2;
        QStringList args;
        args << "--port=USB" << "--mode=JTAG" << "--hex" << fullpath << "--eraseall";
        ec2.start("ec2writeflash",args);
        ec2.waitForFinished(10000);
        QString buf(ec2.readAllStandardOutput());
        ui->terminalEdit->appendPlainText(buf);
        // NOTE: ADD SUPPORT FOR AUTO DOWN-GRADING IF NECESSARY
    }
    else
    {
        ui->terminalEdit->appendPlainText("Could not find a hex file!");
    }
}

QString MainWindow::workingDir()
{
    if(ui->dirEdit->text().size() > 0)
        return ui->dirEdit->text();
    else
        return QDir::currentPath();
}

void MainWindow::on_saveButton_clicked()
{

}
