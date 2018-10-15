#ifndef FORK_HPP
#define FORK_HPP

#include <src/audio/audio.hpp>

class Fork : public StreamNode
{
    Q_OBJECT
    Q_PROPERTY  ( StreamNode* target READ target WRITE setTarget )

    public:
    Fork();

    virtual void initialize(qint64) override;
    virtual float** process(float** buf, qint64 nsamples) override;

    StreamNode* target() { return m_target; }
    void setTarget(StreamNode* target);

    private:
    StreamNode* m_target;
};

#endif // FORK_HPP
