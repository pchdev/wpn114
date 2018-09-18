#pragma once

#include <QObject>
#include "node.hpp"

class WPNFolderNode : public WPNNode
{
    Q_OBJECT
    Q_PROPERTY  ( QString folderPath READ folderPath WRITE setFolderPath )
    Q_PROPERTY  ( QString extensions READ extensions WRITE setExtensions )
    Q_PROPERTY  ( bool recursive READ recursive WRITE setRecursive )

    public:
    WPNFolderNode();

    QString folderPath() const { return m_folder_path; }
    QString extensions() const { return m_extensions; }
    bool recursive() const { return m_recursive; }

    void setFolderPath(QString path);
    void setExtensions(QString ext);
    void setRecursive(bool rec);

    private:
    bool m_recursive;
    QString m_folder_path;
    QString m_extensions;

};
