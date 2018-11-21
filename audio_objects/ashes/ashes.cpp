#include "ashes.hpp"

Ashes::Ashes()
{
    SETN_OUT  ( 1 );
    SETTYPE   ( StreamType::Generator );
}

inline quint64 draw()
{
    static quint64 randseed = 22222;
    randseed = (randseed*19631465) + 907633515;
    return randseed;
}

void Ashes::initialize(qint64 nsamples)
{
    qint32 nrows = 16;
    m_index = 0;
    m_index_mask = ( 1 << nrows ) - 1;
    qint64 pmax = (nrows+1) * ( 1 << PINK_RANDOM_BITS-1 );
    m_scalar = 1.0f/pmax;

    for ( int i = 0; i < nrows; ++i )
          m_rows[i] = 0;
}

float** Ashes::process(float**, qint64 nsamples)
{    
    auto out            = m_out;
    auto index          = m_index;
    auto index_mask     = m_index_mask;
    auto rsum           = m_running_sum;
    auto scalar         = m_scalar;

    for ( qint64 s = 0; s < nsamples; ++s )
    {
        qint64 nrandom, sum;
        index = index+1 & index_mask;

        if ( index )
        {
            int nzeros = 0;
            int n = index;

            while ((n & 1) == 0)
            {
                n >>= 1;
                nzeros++;
            }

            rsum -= m_rows[nzeros];
            nrandom = ((qint64)draw())>> PINK_RANDOM_SHIFT;
            rsum += nrandom;
            m_rows[nzeros] = nrandom;
        }

        nrandom = ((qint64)draw()) >> PINK_RANDOM_SHIFT;
        sum = rsum + nrandom;

        out[0][s] = scalar*sum;
    }

    m_index = index;
    m_running_sum = rsum;

    return out;
}
