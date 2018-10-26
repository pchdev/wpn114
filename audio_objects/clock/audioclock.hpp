#ifndef AUDIOCLOCK_HPP
#define AUDIOCLOCK_HPP

#include <src/audio/audio.hpp>
#include <math.h>

#include <QQmlProperty>
#include <QMetaProperty>

class TimeNode : public QObject, public QQmlParserStatus
{
    Q_OBJECT

    Q_CLASSINFO     ( "DefaultProperty", "subnodes" )
    Q_INTERFACES    ( QQmlParserStatus )

    Q_PROPERTY  ( TimeNode* parentNode READ parentNode WRITE setParentNode NOTIFY parentNodeChanged )
    Q_PROPERTY  ( WorldStream* source READ source WRITE setSource NOTIFY sourceChanged )
    Q_PROPERTY  ( qreal date READ date WRITE setDate )
    Q_PROPERTY  ( qreal duration READ duration WRITE setDuration )
    Q_PROPERTY  ( QQmlListProperty<TimeNode> subnodes READ subnodes )
    Q_PROPERTY  ( TimeNode* after READ follow WRITE setFollow NOTIFY afterChanged )
    Q_PROPERTY  ( bool condition READ condition WRITE setCondition NOTIFY conditionChanged )
    Q_PROPERTY  ( bool running READ running )

    public:
    TimeNode();
    TimeNode(const TimeNode& copy);

    virtual void componentComplete() override;
    virtual void classBegin() override {}

    Q_INVOKABLE qreal sec(qreal s) { return s*1000.0; }
    Q_INVOKABLE qreal min(qreal m)
    {
        quint64 s = floor(m);
        return (s*60.0+((m-s)*100.0))*1000.0;
    }

    QQmlListProperty<TimeNode>  subnodes();
    const QVector<TimeNode*>&   getSubnodes() const { return m_subnodes; }

    virtual void appendSubnode  ( TimeNode* );
    virtual int subnodesCount   ( ) const;
    virtual TimeNode* subnode   ( int ) const;
    virtual void clearSubnodes  ( );

    TimeNode* parentNode  ( ) const { return m_parent_node; }
    TimeNode* follow      ( ) const { return m_follow; }
    WorldStream* source   ( ) const { return m_source; }
    bool condition        ( ) const { return m_condition; }
    bool running          ( ) const { return m_running; }
    qreal date            ( ) const;
    qreal duration        ( ) const;

    void setDate        ( qreal date );
    void setDuration    ( qreal duration );
    void setCondition   ( bool condition );
    void setSource      ( WorldStream* source );
    void setFollow      ( TimeNode* follow );
    void setParentNode  ( TimeNode* node );

    Q_INVOKABLE void reset ( );
    Q_INVOKABLE void playFrom ( quint64 ms );
    Q_INVOKABLE quint64 absoluteDate() const;

    Q_INVOKABLE void suspend ( );
    Q_INVOKABLE void resume  ( );

    signals:
    void afterChanged  ( );
    void sourceChanged ( );
    void parentNodeChanged ( );
    void start    ( );
    void end      ( );

    void conditionChanged  ( );

    protected slots:
    virtual void onTick   ( qint64 sz );
    virtual void onBegin  ( );
    virtual void onStop   ( );

    protected:
    static void appendSubnode  ( QQmlListProperty<TimeNode>*, TimeNode* );
    static int subnodesCount   ( QQmlListProperty<TimeNode>* );
    static TimeNode* subnode   ( QQmlListProperty<TimeNode>*, int );
    static void clearSubnodes  ( QQmlListProperty<TimeNode>* );

    QVector<TimeNode*> m_subnodes;
    TimeNode* m_follow = nullptr;
    TimeNode* m_parent_node = nullptr;

    bool m_suspended = false;
    bool m_running = false;
    bool m_condition = true;
    bool m_infinite = false;

    WorldStream* m_source = nullptr;
    qreal m_date = 0;
    qreal m_duration = 0;
    quint64 m_clock = 0;
};

class Automation : public TimeNode
{
    Q_OBJECT

    Q_PROPERTY  ( qreal from READ from WRITE setFrom )
    Q_PROPERTY  ( qreal to READ to WRITE setTo )
    Q_PROPERTY  ( QObject* target READ target WRITE setTarget )
    Q_PROPERTY  ( QString property READ property WRITE setProperty )

    public:
    Automation();

    virtual void componentComplete() override;

    qreal from  () const { return m_from; }
    qreal to    () const { return m_to; }

    QObject* target   ( ) const { return m_target; }
    QString property  ( ) const { return m_property_str; }

    void setFrom      ( qreal from );
    void setTo        ( qreal to );
    void setTarget    ( QObject* target );
    void setProperty  ( QString property );

    public slots:
    virtual void onTick   ( qint64 sz ) override;
    virtual void onBegin  ( );
    virtual void onStop   ( );

    void onFollowBegin ( );

    private:
    qreal m_phase;
    qreal m_from = 0;
    qreal m_to = 1;
    QObject* m_target;
    QString m_property_str;
    QMetaProperty m_property;

};

class Loop : public TimeNode
{
    Q_OBJECT

    Q_PROPERTY  ( int times READ times WRITE setTimes )
    Q_PROPERTY  ( TimeNode* pattern READ pattern WRITE setPattern )

    public:
    Loop();

    virtual void componentComplete() override;

    quint64 times       () const { return m_times; }
    TimeNode* pattern   () const { return m_pattern; }

    void setTimes   ( quint64 times );
    void setPattern ( TimeNode* pattern );

    virtual void appendSubnode  ( TimeNode* ) override;
    virtual int subnodesCount   ( ) const override;
    virtual TimeNode* subnode   ( int ) const override;
    virtual void clearSubnodes  ( ) override;

    signals:
    void loop(int count);

    public slots:
    virtual void onTick   ( qint64 sz ) override;
    void onPatternStop    ( );
    void onSourceChanged  ( );

    private:
    TimeNode* m_pattern = nullptr;
    quint64 m_times = 0;
    quint64 m_count = 0;
    qreal m_pattern_clock = 0;

};

#endif // AUDIOCLOCK_HPP
