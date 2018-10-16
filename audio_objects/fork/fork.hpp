#ifndef FORK_HPP
#define FORK_HPP

#include <src/audio/audio.hpp>

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

    public:
    Fork();

    virtual void preinitialize(StreamProperties properties) override;
    virtual void initialize(qint64) override {}
    virtual float** process(float** buf, qint64 nsamples) override;

    void setActive(bool active) override;

    StreamNode* target() { return m_target; }
    void setTarget(StreamNode* target);

    private:
    StreamNode* m_parent;
    StreamNode* m_target;
    ForkEndpoint* m_endpoint;
};

#endif // FORK_HPP
