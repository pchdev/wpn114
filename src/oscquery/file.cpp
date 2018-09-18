#include "file.hpp"

WPNFileNode::WPNFileNode() : WPNNode(), m_file(nullptr)
{
    m_attributes.type       = Type::File;
    m_attributes.access     = Access::READ;
}

void WPNFileNode::setFilePath(QString path)
{
    m_filepath = path;
    m_file = new QFile(path, this);

    m_file->open(QIODevice::ReadOnly | QIODevice::Text );
    m_data = m_file->readAll();
}

QByteArray WPNFileNode::data() const
{
    return m_data;
}
