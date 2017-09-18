#include "filemanager.h"
#include <QFileDialog>
#include <QObject>
#include <QDirIterator>
#include <QTextStream>

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
QString FileManager::getDir(bool &ok, QWidget *parent, QString def)
{
    QString fullpath;
    fullpath = QFileDialog::getExistingDirectory(parent,
                                                 QString("Select Working Directory"),
                                                 def);

    if(QDir(fullpath).exists())
        ok = true;
    else
        ok = false;

    return fullpath;
}

QString FileManager::getHex(bool &ok, QString dir)
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

workspace_t FileManager::loadWorkspace(bool &ok, QWidget *parent, QString dir)
{
    workspace_t returnValue;
    QString fullpath;
    ok = false;
    fullpath = QFileDialog::getOpenFileName(parent, QString("Open a workspace"),
                                            dir,
                                            "8051 Workspace Files (*.gws)");
    // Ensure a real .gws file was chosen
    if(!fullpath.isEmpty() && fullpath.indexOf(".gws") != -1)
    {
        QFile file(fullpath);
        if(file.open(QIODevice::ReadOnly))
        {
            QTextStream in(&file);
            returnValue.workingDir = in.readLine();
            returnValue.flags = in.readLine();
            returnValue.sources.clear();
            returnValue.filename = QFileInfo(file).fileName();
            while(!in.atEnd())
                returnValue.sources.append(in.readLine());
            ok = true;
            file.close();
        }
    }
    return returnValue;
}

workspace_t FileManager::loadWorkspaceDirect(bool &ok, const QString & fullpath)
{
    workspace_t returnValue;
    ok = false;
    // Ensure a real .gws file was chosen
    if(!fullpath.isEmpty() && fullpath.indexOf(".gws") != -1)
    {
        QFile file(fullpath);
        if(file.open(QIODevice::ReadOnly))
        {
            QTextStream in(&file);
            returnValue.workingDir = in.readLine();
            returnValue.flags = in.readLine();
            returnValue.sources.clear();
            returnValue.filename = QFileInfo(file).fileName();
            while(!in.atEnd())
                returnValue.sources.append(in.readLine());
            ok = true;
            file.close();
        }
    }
    return returnValue;
}

int FileManager::saveWorkspace(workspace_t toSave, QWidget *parent, QString dir,
                                    QString &saveFileName)
{
    int returnVal = -1;
    QString fullpath;
    fullpath = QFileDialog::getSaveFileName(parent, QString("Select save destination"),
                                            dir,
                                            "8051 Workspace Files (*.gws)");
    // Ensure a real file was chosen
    if(!fullpath.isEmpty())
    {
        // Add correct filetype
        if(fullpath.indexOf(".gws") == -1)
            fullpath.append(".gws");
        // Attempt to open file for streaming
        QFile file(fullpath);
        if(file.open(QIODevice::WriteOnly))
        {
            QTextStream out(&file);
            out << toSave.workingDir << "\n" << toSave.flags << "\n";
            foreach(const QString & src, toSave.sources)
                out << src << "\n";
            returnVal = 0;
            saveFileName = QFileInfo(file).fileName();
            file.close();
        }
    }
    return returnVal;
}

int FileManager::saveWorkspaceDirect(workspace_t toSave, QString &fullpath)
{
    int returnVal = -1;
    // Ensure a real file was chosen
    if(!fullpath.isEmpty())
    {
        // Add correct filetype
        if(fullpath.indexOf(".gws") == -1)
            fullpath.append(".gws");
        // Attempt to open file for streaming
        QFile file(fullpath);
        if(file.open(QIODevice::WriteOnly))
        {
            QTextStream out(&file);
            out << toSave.workingDir << "\n" << toSave.flags << "\n";
            foreach(const QString & src, toSave.sources)
                out << src << "\n";
            returnVal = 0;
            file.close();
        }
    }
    return returnVal;
}
