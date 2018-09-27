#include "stereopanner.hpp"
#include <math.h>

StereoPanner::StereoPanner() : StreamNode(), m_position(0.5)
{
    SETN_IN     ( 1 );
    SETN_OUT    ( 2 );
}

float** StereoPanner::userProcess(float** buf, qint64 bsize)
{
    qreal position  = (m_position+1.f)/2.f;
    float** out     = m_out;

    for ( quint16 s = 0; s < bsize; ++s )
    {
        out[0][s] = buf[0][s]*sqrt(1.f-position);
        out[1][s] = buf[0][s]*sqrt(position);
    }

    return out;
}


