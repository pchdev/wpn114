#ifndef FORK_HPP
#define FORK_HPP

#include <source/audio/audio.hpp>

class Fork;

class ForkEndpoint: public StreamNode
{
    Q_OBJECT

    public:
    ForkEndpoint(Fork& fork);

    virtual void initialize(qint64) override {}
    virtual float** process(float** buf, qint64 nsamples) override;

    private:
    Fork& m_fork;

};

class Fork : public StreamNode
{
    Q_OBJECT
    Q_PROPERTY  ( StreamNode* target READ target WRITE setTarget )
    Q_PROPERTY  ( bool prefader READ prefader WRITE setPrefader )

    public:
    Fork();
    ~Fork();

    virtual void preinitialize(StreamProperties properties) override;
    virtual void initialize(qint64) override {}
    virtual float** preprocess(float** buf, qint64 nsamples) override;
    virtual float** process(float** buf, qint64 nsamples) override {}

    void setActive(bool active) override;

    StreamNode* target  ( ) { return m_target; }
    bool prefader       ( ) { return m_prefader; }

    void setTarget      ( StreamNode* target );
    void setPrefader    ( bool prefader );

    public slots:
    void onSourceActiveChanged();

    private:
    bool m_active_default;
    bool m_prefader = false;
    StreamNode* m_parent;
    StreamNode* m_target;
    ForkEndpoint* m_endpoint;
};

#endif // FORK_HPP
