#include "peakrms.hpp"
#include <math.h>
#include <cmath>
#include <QtDebug>

PeakRMS::PeakRMS()
{
    qRegisterMetaType<QVector<float>>();
}

void PeakRMS::componentComplete()
{
    auto parent = qobject_cast<StreamNode*>(QObject::parent());

    SETN_IN   ( 2 );
    SETN_OUT  ( 2 );
}

void PeakRMS::initialize(qint64 nsamples)
{

}

float** PeakRMS::process(float** in, qint64 nsamples)
{
    auto nout = m_num_outputs;

    QVector<float> max;
    QVector<float> mean;
    QVariantList peakvl;
    QVariantList meanvl;

    max.fill(0, nout);
    mean.fill(0, nout);


    for ( quint16 ch = 0; ch < nout; ++ch )
    {
        for ( quint16 s = 0; s < nsamples; ++s )
        {
            max[ch]   = qMax(max[ch], in[ch][s]);
            mean[ch] += pow(in[ch][s],2);
        }

        mean[ch] = sqrt(1.0/nsamples*mean[ch]);

        max[ch]  = std::log10(max[ch])*20.f;
        mean[ch] = std::log10(mean[ch])*20.f;

        peakvl << max[ch];
        meanvl << mean[ch];
    }

    emit peak ( peakvl );
    emit rms  ( meanvl );

    return in;
}


