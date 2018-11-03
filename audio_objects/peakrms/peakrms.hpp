#ifndef PEAKRMS_HPP
#define PEAKRMS_HPP

#include <src/audio/audio.hpp>

class PeakRMS : public StreamNode
{
    Q_OBJECT

    Q_PROPERTY  ( StreamNode* source READ source WRITE setSource )
    Q_PROPERTY  ( qreal refreshRate READ refreshRate WRITE setRefreshRate )

    public:
    PeakRMS();

    virtual void initialize(qint64) override;
    virtual float** process(float**, qint64) override;
    virtual void componentComplete() override;

    qreal refreshRate   ( ) const { return m_refresh_rate; }
    StreamNode* source  ( ) { return m_source; }

    void setRefreshRate ( qreal rate );
    void setSource      ( StreamNode* source );

    signals:
    void rms  ( QVariant value );
    void peak ( QVariant value );

    protected:
    void bufferComplete();

    private:
    StreamNode* m_source = nullptr;
    qreal m_refresh_rate = 20;
    quint32 m_block_size = 0;
    float** m_block = nullptr;
    quint32 m_pos = 0;
};

#endif // PEAKRMS_HPP
