#include "filemanager.h"
#include <QFileDialog>
#include <QObject>
#include <QDirIterator>

FileManager::FileManager()
{

}

// Function to get a filepath to a C file for the compiler
QString FileManager::getCFile(bool &ok, QWidget *prnt, QString def)
{
    QString fullpath = "";
    fullpath = QFileDialog::getOpenFileName(prnt, QString("Open C Source File"),
                                            def,
                                            QString("C Files (*.c *.h);;All files (*)"));
    if(QFile::exists(fullpath))
        ok = true;
    else
        ok = false;
    return fullpath;
}

// Function to get a working directory
QString FileManager::getDir(bool &ok, QWidget *parent)
{
    QString fullpath;
    fullpath = QFileDialog::getExistingDirectory(parent, QString("Select Working Directory"));

    if(QDir(fullpath).exists())
        ok = true;
    else
        ok = false;

    return fullpath;
}

QString FileManager::getHex(bool &ok, QWidget *parent, QString dir)
{
    QDirIterator itr(dir);
    QString fullpath;
    QString buf;
    bool found = false;
    while(itr.hasNext() && !found)
    {
        buf = itr.next();
        if(buf.indexOf(".ihx") != -1)
        {
            fullpath = itr.filePath();
            found = true;
        }
    }
    ok = found;
    return fullpath;
}
