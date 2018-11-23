#include "peakrms.hpp"
#include <math.h>
#include <cmath>
#include <QtDebug>

PeakRMS::PeakRMS()
{
    SETTYPE   ( StreamType::Effect );
    qRegisterMetaType<QVector<float>>();
}

void PeakRMS::setSource(StreamNode* source)
{
    m_source = source;
}

void PeakRMS::setRefreshRate(qreal rate)
{
    m_refresh_rate = rate;
}

void PeakRMS::componentComplete()
{    
}

void PeakRMS::initialize(qint64 nsamples)
{
    m_block_size = SAMPLERATE/m_refresh_rate;
    StreamNode::allocateBuffer(m_block, m_num_inputs, m_block_size);
}

void PeakRMS::bufferComplete()
{
    auto nout   = m_num_outputs;
    auto pos    = m_pos;
    auto block  = m_block;
    auto bsize  = m_block_size;

    QVector<float> max;
    QVector<float> mean;
    QVariantList peakvl;
    QVariantList meanvl;

    max.fill   ( 0, nout );
    mean.fill  ( 0, nout );

    for ( quint16 ch = 0; ch < nout; ++ch )
    {
        for ( quint16 s = 0; s < bsize; ++s )
        {
            max  [ ch ] = qMax  ( max[ch], block[ch][s] );
            mean [ ch ] += pow  ( block[ch][s], 2 );
        }

        mean[ch] = sqrt( 1.0/bsize*mean[ch] );

        max  [ ch ]  = std::log10( max[ch]  ) *20.f;
        mean [ ch ]  = std::log10( mean[ch] ) *20.f;

        peakvl << max[ch];
        meanvl << mean[ch];
    }

    emit peak ( peakvl );
    emit rms  ( meanvl );

}

float** PeakRMS::process(float** in, qint64 nsamples)
{
    auto nout   = m_num_outputs;
    auto pos    = m_pos;
    auto block  = m_block;
    auto bsize  = m_block_size;

    qint32 rest = bsize-(pos+nsamples);

    if ( rest > 0 )
    {
        for ( quint16 ch = 0; ch < nout; ++ch )
            for ( quint32 s = 0; s < nsamples; ++s )
                m_block[ch][m_pos+s] = in[ch][s];

        m_pos += nsamples;
    }

    else
    {
        for ( quint16 ch = 0; ch < nout; ++ch )
            for ( quint32 s = 0; s < nsamples+rest; ++s )
                m_block[ch][m_pos+s] = in[ch][s];

        // dispatch buffer
        bufferComplete();

        // reset buffer and start filling again
        // with the rest of the current block
        StreamNode::resetBuffer(block, nout, bsize);

        for ( quint16 ch = 0; ch < nout; ++ch )
            for ( quint32 s = 0; s < -rest; ++s )
                m_block[ch][s] = in[ch][nsamples+rest+s];

        m_pos = -rest;
    }

    return in;
}
