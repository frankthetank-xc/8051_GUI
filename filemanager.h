#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QString>
#include <QWidget>

struct workspace_t{
    QString workingDir;
    QString flags;
    QStringList sources;
    QString filename;
};


class FileManager
{
public:
    FileManager();

    QString getCFile(bool &ok, QWidget *prnt, QString def );
    QString getDir(bool &ok, QWidget *parent, QString def);
    QString getHex(bool &ok, QString dir);

    workspace_t loadWorkspace(bool &ok, QWidget *parent, QString dir);
    workspace_t loadWorkspaceDirect(bool &ok, const QString &fullpath);

    int saveWorkspace(workspace_t toSave, QWidget *parent, QString dir, QString &saveFileName);
    int saveWorkspaceDirect(workspace_t toSave, QString &fullpath);
};

#endif // FILEMANAGER_H
