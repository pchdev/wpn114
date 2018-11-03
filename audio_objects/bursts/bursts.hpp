#ifndef BURSTS_HPP
#define BURSTS_HPP

#include <src/audio/audio.hpp>

class Bursts : public StreamNode
{
    Q_OBJECT

    Q_PROPERTY  ( int length READ length WRITE setLength )
    Q_PROPERTY  ( QVariantList channels READ channels WRITE setChannels )

    public:
    Bursts();

    void componentComplete  ( ) override;
    void initialize         ( qint64 ) override;
    float** process         ( float**, qint64 ) override;

    quint32 length          ( ) const { return m_length; }
    QVariantList channels   ( ) const;

    void setLength      ( quint32 length );
    void setChannels    ( QVariantList channels );

    private:
    quint32 m_length;
    QVector<quint16>  m_channels;
};

#endif // BURSTS_HPP
