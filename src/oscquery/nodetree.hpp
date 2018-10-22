#ifndef NODETREE_HPP
#define NODETREE_HPP

#include <QAbstractItemModel>
#include "node.hpp"

class WPNNodeTree : public QAbstractItemModel
{
    Q_OBJECT

    public:
    WPNNodeTree();
    WPNNodeTree(WPNNode* node);

    void setRoot(WPNNode* node);

    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex& index) const;

    private:
    WPNNode* m_root_node;

};

#endif // NODETREE_HPP
