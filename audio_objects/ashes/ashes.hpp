#ifndef ASHES_H
#define ASHES_H

#include <source/audio/audio.hpp>

#define PINK_MAX_RANDOM_ROWS    ( 30 )
#define PINK_RANDOM_BITS        ( 24 )
#define PINK_RANDOM_SHIFT       ( (sizeof(long)*8)-PINK_RANDOM_BITS )

class Ashes : public StreamNode
{
    Q_OBJECT

    public:
    Ashes();

    virtual void initialize(qint64) override;
    virtual float** process(float**, qint64) override;

    private:
    qint64 m_rows[PINK_MAX_RANDOM_ROWS];
    qint64 m_running_sum = 0;
    qint32 m_index = 0;
    qint32 m_index_mask;
    float m_scalar;


};

#endif // ASHES_H
