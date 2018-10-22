#ifndef NETEXPLORER_HPP
#define NETEXPLORER_HPP

#include <qzeroconf.h>
#include <QObject>

class NetExplorer : public QObject
{
    Q_OBJECT

    Q_PROPERTY  ( QString target READ target WRITE setTarget )

    public:
    NetExplorer();

    QString target() const { return m_target; }
    void setTarget(QString target);

    public slots:
    void onServiceAdded(QZeroConfService);
    void onServiceUpdated(QZeroConfService);
    void onServiceRemoved(QZeroConfService);

    signals:
    void newHost(QString host);
    void hostLost(QString host);

    private:
    QStringList m_hostlist;
    QString m_target;
    QZeroConf m_zeroconf;
};

#endif // NETEXPLORER_HPP
