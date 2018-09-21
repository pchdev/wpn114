#include "folder.hpp"
#include "file.hpp"
#include <QtDebug>

WPNFolderNode::WPNFolderNode() : m_recursive(true)
{        
    m_attributes.type       = Type::List;
    m_attributes.access     = Access::READ;

    m_filters << "*.png" << "*.qml";
}

void WPNFolderNode::parseDirectory(QDir dir)
{
    for ( const auto& file : dir.entryList() )
    {
        WPNFileNode* node = new WPNFileNode;

        node->setName       ( file );
        node->setFilePath   ( dir.path()+"/"+file );
        node->setPath       ( m_attributes.path+"/"+file );
        addSubnode          ( node );
    }

    if ( m_recursive )
    {
        dir.setNameFilters(QStringList{});
        dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot );

        for ( const auto& d : dir.entryList() )
        {
            WPNFolderNode* folder = new WPNFolderNode;
            folder->setName ( d );
            folder->setPath ( m_attributes.path+"/"+d );
            folder->setRecursive(true);
            folder->setFilters(m_filters);
            folder->setFolderPath ( dir.path()+"/"+d );
            addSubnode(folder);
        }
    }
}

void WPNFolderNode::setFolderPath(QString path)
{
    m_folder_path = path;

    QDir dir(path);
    dir.setNameFilters(m_filters);
    parseDirectory(dir);
}

void WPNFolderNode::setExtensions(QString ext)
{
    m_extensions = ext;
}

void WPNFolderNode::setRecursive(bool rec)
{
    m_recursive = rec;
}

void WPNFolderNode::setFilters(QStringList filters)
{
    m_filters = filters;
}
