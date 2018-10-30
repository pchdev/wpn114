#ifndef PEAKRMS_HPP
#define PEAKRMS_HPP

#include <src/audio/audio.hpp>

class PeakRMS : public StreamNode
{
    Q_OBJECT

    public:
    PeakRMS();

    virtual void initialize(qint64) override;
    virtual float** process(float**, qint64) override;

    virtual void componentComplete() override;

    signals:
    void rms  ( QVariant value );
    void peak ( QVariant value );
};

#endif // PEAKRMS_HPP
