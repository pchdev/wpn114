#pragma once

#include <source/audio/audio.hpp>

struct chcoeff
{
    float ospl = 0;

    float x11 = 0;
    float x21 = 0;
    float y11 = 0;
    float y21 = 0;

    float x13 = 0;
    float x23 = 0;
    float y13 = 0;
    float y23 = 0;
};

class Filter : public StreamNode
{
    Q_OBJECT

    Q_PROPERTY  ( qreal hpf READ hpf WRITE setHpf )
    Q_PROPERTY  ( qreal lpf READ lpf WRITE setLpf )

    public:
    Filter();

    void initialize(qint64) override;
    float** process(float**, qint64) override;

    qreal hpf() const { return m_hpf; }
    qreal lpf() const { return m_lpf; }

    void setHpf(qreal hpf);
    void setLpf(qreal lpf);

    private:
    qreal m_hpf = 0;
    qreal m_lpf = 22000;
    qreal m_gain = 0;

    QVector<chcoeff> m_coeffs;

    bool b_hpf;
    bool b_lpf;
    float gain, gain1, gain3;
    float freq1, freq3;
    float a1, s1, q1;
    float a3, s3, q3;
    float w01, cosw01, sinw01, alpha1;
    float w03, cosw03, sinw03, alpha3;
    float b01, b11, b21, a01, a11, a21;
    float b03, b13, b23, a03, a13, a23;

    float ospl0, x11, x21, y11, y21;


};
