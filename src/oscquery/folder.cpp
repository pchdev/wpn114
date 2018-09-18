#include "folder.hpp"
#include <QDir>
#include "file.hpp"
#include <QtDebug>

WPNFolderNode::WPNFolderNode()
{
    m_attributes.type       = Type::None;
    m_attributes.access     = Access::READ;
}

void WPNFolderNode::setFolderPath(QString path)
{
    m_folder_path = path;

    QDir dir(path);
    dir.setNameFilters(QStringList{"*.qml"});

    QStringList files = dir.entryList();

    for ( const auto& file : files )
    {
        WPNFileNode* node = new WPNFileNode;

        node->setName       ( file );
        node->setFilePath   ( path+"/"+file );
        node->setPath       ( m_attributes.path+"/"+file );
        addSubnode          ( node );
    }
}

void WPNFolderNode::setExtensions(QString ext)
{
    m_extensions = ext;
}

void WPNFolderNode::setRecursive(bool rec)
{
    m_recursive = rec;
}
