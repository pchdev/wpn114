#include "nodetree.hpp"

WPNNodeTree::WPNNodeTree() : m_root_node(nullptr)
{

}

WPNNodeTree::WPNNodeTree(WPNNode* root) : m_root_node(root)
{

}

void WPNNodeTree::setRoot(WPNNode *node)
{
    m_root_node = node;
}

int WPNNodeTree::columnCount(const QModelIndex& parent) const
{
    return 2;
}

int WPNNodeTree::rowCount(const QModelIndex& parent) const
{
    QString parent_path = parent.data().toString();
    auto node = m_root_node->subnode(parent_path);

    if ( node ) return node->nsubnodes();

    return 0;
}

QVariant WPNNodeTree::data(const QModelIndex& index, int role) const
{
    auto node = m_root_node->subnode(index.data().toString());
    return node->path();
}

QModelIndex WPNNodeTree::index(int row, int column, const QModelIndex& parent) const
{

}

QModelIndex WPNNodeTree::parent(const QModelIndex& index) const
{
    auto node = m_root_node->subnode(index.data().toString());

}
