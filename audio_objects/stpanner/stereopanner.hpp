#ifndef STEREOPANNER_HPP
#define STEREOPANNER_HPP

#include <src/audio/audio.hpp>

class StereoPanner : public StreamNode
{
    Q_OBJECT
    Q_PROPERTY ( qreal position READ position WRITE setPosition )

    public:
    StereoPanner();
    virtual void userInitialize(qint64) override {}
    virtual float** userProcess(float** buf, qint64 le) override;

    qreal position() const { return m_position; }
    void setPosition (qreal position ) { m_position = position; }

    private:
    qreal m_position;
};

#endif // STEREOPANNER_HPP
