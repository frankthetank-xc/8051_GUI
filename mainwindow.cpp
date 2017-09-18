#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QProcess>
#include <QTextStream>

// Initialization Routine
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    filemgr = new FileManager();
    getDefaultSettings();
    ui->nameEdit->setText("Default");
    ui->terminalEdit->clear();
}

// Destructor
MainWindow::~MainWindow()
{
    saveDefaults();
    delete ui;
    delete filemgr;
}

// Routine for adding source files
void MainWindow::on_addButton_clicked()
{
    bool ok;
    // Get a C file
    QString fullpath = filemgr->getCFile(ok,this,workingDir());
    if(ok)
    {
        // Add to list
        ui->sourceEdit->addItem(fullpath);
    }
}

// Routine for editing what files are selected
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

// Routine to remove items from the list
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
    QStringList arguments = ui->flagsEdit->text().split(' ',QString::SkipEmptyParts);
    for(int i = 0; i < ui->sourceEdit->count(); i++)
        arguments.append( ui->sourceEdit->item(i)->text());
    // Get the right working directory
    QString dir = workingDir();
    process.setWorkingDirectory(dir);
    // Move to the tab with the terminal
    ui->controlTabs->setCurrentIndex(ui->controlTabs->count() - 1);
    // ui->terminalEdit->clear();
    // Run sdcc
    process.start(program, arguments);
    process.waitForFinished();
    // Grab  sdcc output and put it in the terminal box
    QString out(process.readAllStandardOutput());
    QString err(process.readAllStandardError());
    ui->terminalEdit->appendPlainText(out);
    ui->terminalEdit->appendPlainText(err);
}

void MainWindow::on_dirButton_clicked()
{
    bool ok;
    QString fullpath = filemgr->getDir(ok, this, workingDir());
    if(ok)
    {
        ui->dirEdit->setText(fullpath);
    }
}

void MainWindow::on_Download_Button_clicked()
{
    bool ok = false;
    // Get a hex file (For now just uses first in directory)
    QString fullpath = filemgr->getHex(ok,workingDir());
    if(ok)
    {
        ui->controlTabs->setCurrentIndex(ui->controlTabs->count() - 1);
        ui->terminalEdit->appendPlainText("Attempting to download to 8051...");
        ui->terminalEdit->appendPlainText(fullpath);
        printf("Connecting...\r\n");
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
    workspace_t params;
    QString filename;
    getParams(params);
    if(filemgr->saveWorkspace(params,this,workingDir(),filename) == 0)
        ui->nameEdit->setText(filename);
}

void MainWindow::on_loadButton_clicked()
{
    bool ok = false;
    workspace_t params = filemgr->loadWorkspace(ok,this,workingDir());
    if(ok)
    {
        configSettings(params);
    }
}

int MainWindow::getDefaultSettings()
{
    // Get path to where default settings should be
    defaultFullpath = QCoreApplication::applicationDirPath();
    defaultFullpath.append("/default.gws");
    QFile def(defaultFullpath);
    // If the file doesn't exist, don't bother reading in settings
    bool readIn = def.exists();
    if(def.open(QIODevice::ReadWrite))
    {
        // If the file does not exist, give it default values
        if(!readIn)
        {
            QTextStream out(&def);
            out << QCoreApplication::applicationDirPath();
            out << "\n" << "--debug --verbose --nogcse --out-fmt-ihx \n";
        }
        bool ok;
        workspace_t params = filemgr->loadWorkspaceDirect(ok,defaultFullpath);
        if(ok)
            configSettings(params);
        def.close();
    }
    return 0;
}

void MainWindow::configSettings(const workspace_t &params)
{
    ui->dirEdit->setText(params.workingDir);
    ui->flagsEdit->setText(params.flags);
    ui->sourceEdit->clear();
    if(params.filename.size() > 0)
        ui->nameEdit->setText(params.filename);
    foreach(const QString &str, params.sources)
        ui->sourceEdit->addItem(str);
}

void MainWindow::saveDefaults()
{
    workspace_t params;
    getParams(params);
    filemgr->saveWorkspaceDirect(params, defaultFullpath);
}

void MainWindow::getParams(workspace_t &params)
{
    params.workingDir = ui->dirEdit->text();
    params.flags = ui->flagsEdit->text();
    params.sources.clear();
    for(int i = 0; i < ui->sourceEdit->count(); i++)
        params.sources.append(ui->sourceEdit->item(i)->text());
}
