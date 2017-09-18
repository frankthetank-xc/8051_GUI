#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QString>
#include <QWidget>


class FileManager
{
public:
    FileManager();

    QString getCFile(bool &ok, QWidget *prnt, QString def );
    QString getDir(bool &ok, QWidget *parent);
    QString getHex(bool &ok, QWidget *parent, QString dir);
};

#endif // FILEMANAGER_H
