#include "folder.hpp"
#include "file.hpp"
#include <QtDebug>
#include <QStandardPaths>
#include <src/oscquery/client.hpp>

WPNFolderNode::WPNFolderNode() : m_recursive(true)
{        
    m_attributes.type       = Type::List;
    m_attributes.access     = Access::READ;

    m_filters << "*.png" << "*.qml";
}

void WPNFolderNode::parseDirectory(QDir dir)
{
    setValue(dir.entryList());

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

            folder->setName         ( d );
            folder->setPath         ( m_attributes.path+"/"+d );
            folder->setRecursive    ( true );
            folder->setFilters      ( m_filters );
            folder->setFolderPath   ( dir.path()+"/"+d );
            addSubnode              ( folder );
        }
    }
}

void WPNFolderNode::setFolderPath(QString path)
{
    m_folder_path = path;

    QDir dir            ( path );
    dir.setNameFilters  ( m_filters );
    parseDirectory      ( dir );
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

//---------------------------------------------------------------------------------------------------------

WPNFolderMirror::WPNFolderMirror() : m_recursive(true), m_current_download(nullptr)
{
    // whenever value is received, update downloads
    QObject::connect(this, SIGNAL(valueReceived(QVariant)), this, SLOT(onFileListChanged(QVariant)));
}

WPNFolderMirror::~WPNFolderMirror()
{
    for ( const auto& child : m_children_folders )
        delete child;
}

void WPNFolderMirror::setDestination(QString destination)
{
    m_destination = destination;
    m_abs_path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).append(destination);
    if ( !QDir(m_abs_path).exists()) QDir().mkdir(m_abs_path);
}

QUrl WPNFolderMirror::toUrl(QString file)
{
    WPNQueryClient* device = qobject_cast<WPNQueryClient*>(m_device);
    QString root = device->hostAddr().prepend("http://").append(":").append(QString::number(device->port()));
    root.append(file.prepend(m_destination)+"/");

    return QUrl(root);
}

void WPNFolderMirror::onFileListChanged(QVariant list)
{
    m_downloads = list.toStringList();

    for ( const auto& file : m_downloads )
        m_queue.enqueue(toUrl(file));

    next();
}

void WPNFolderMirror::parseChildren()
{
    // if child is a list, create another folder mirror
    for ( const auto& child : m_children )
    {
        if ( child->type() == Type::List )
        {
            auto mirror = new WPNFolderMirror;

            mirror->setRecursive    ( true );
            mirror->setDestination  ( m_destination+"/"+child->name() );
            mirror->setValue        ( child->value() );

            m_children_folders.push_back(mirror);
        }
    }
}

void WPNFolderMirror::next()
{
    if ( m_queue.isEmpty() )
    {
        parseChildren();
        emit mirrorComplete();
        return;
    }

    QString path = m_downloads.first().prepend("/").prepend(m_abs_path);
    m_output.setFileName(path);

    if ( !m_output.open(QIODevice::WriteOnly | QIODevice::Text ))
    {
        qDebug() << "error opening file for writing";
        qDebug() << path;
        qDebug() << m_output.errorString();
        return;
    }

    qDebug() << "[FOLDER-MIRROR] Donwloading file:" << m_downloads.first();

    QUrl url = m_queue.dequeue();
    QNetworkRequest req(url);

    m_current_download = m_netaccess.get(req);

    QObject::connect(m_current_download, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    QObject::connect(m_current_download, SIGNAL(finished()), this, SLOT(onComplete()));
}

void WPNFolderMirror::onReadyRead()
{
    m_output.write(m_current_download->readAll());
}

void WPNFolderMirror::onComplete()
{
    m_output.close();
    QObject::disconnect(m_current_download, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    QObject::disconnect(m_current_download, SIGNAL(finished()), this, SLOT(onComplete()));

    qDebug() << "[FOLDER-MIRROR] File download complete:" << m_downloads.first();

    m_current_download->deleteLater();
    m_downloads.removeFirst();

    next();
}
