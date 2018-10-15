#include "peakrms.hpp"
#include <math.h>

PeakRMS::PeakRMS()
{
    auto parent = qobject_cast<StreamNode*>(QObject::parent());

    SETN_IN   ( parent->numOutputs() );
    SETN_OUT  ( parent->numOutputs() );
}

void PeakRMS::initialize(qint64 nsamples)
{

}

float** PeakRMS::process(float** in, qint64 nsamples)
{
    auto nout = m_num_outputs;
    QVector<float> max;
    QVector<float> trms;

    max.fill(0, nout);
    trms.fill(0, nout);

    for ( quint16 ch = 0; ch < nout; ++ch )
        for ( quint16 s = 0; s < nsamples; ++s )
        {
            max[ch] = qMax(max[ch], in[ch][s]);
            trms[ch] = sqrt(1.0/nsamples*in[ch][s]);
        }

    emit peak ( max );
    emit rms  ( trms);

    return in;
}


