#pragma once

#include <QObject>

class AudioStream : public QObject
{
    Q_OBJECT
    Q_PROPERTY  ( int sampleRate READ sampleRate WRITE setSampleRate NOTIFY sampleRateChanged )
    Q_PROPERTY  ( int blockSize READ blockSize WRITE setBlockSize NOTIFY blockSizeChanged )
    Q_PROPERTY  ( int numInputs READ numInputs WRITE setNumInputs NOTIFY numInputsChanged )
    Q_PROPERTY  ( int numOutputs READ numOutputs WRITE setNumOutputs NOTIFY numOutputsChanged )
    Q_PROPERTY  ( QString device READ device WRITE setDevice NOTIFY deviceChanged )
    Q_PROPERTY  ( qreal level READ level WRITE setLevel )
    Q_PROPERTY  ( bool mute READ mute WRITE setMute NOTIFY muteChanged )
    Q_PROPERTY  ( bool active READ active WRITE setActive NOTIFY activeChanged )

    public:

    Q_INVOKABLE void start  ();
    Q_INVOKABLE void stop   ();

    uint32_t sampleRate     () const { return m_sample_rate; }
    uint16_t blockSize      () const { return m_block_size; }
    uint16_t numInputs      () const { return m_num_inputs; }
    uint16_t numOutputs     () const { return m_num_outputs; }
    QString device          () const { return m_device; }
    qreal level             () const { return m_level; }

    void setSampleRate      ( uint32_t sample_rate );
    void setBlockSize       ( uint16_t block_size );
    void setNumInputs       ( uint16_t num_inputs );
    void setNumOutputs      ( uint16_t num_outputs );
    void setDevice          ( QString device );
    void setLevel           ( qreal level );

    private:
    uint32_t m_sample_rate;
    uint16_t m_block_size;
    uint16_t m_num_inputs;
    uint16_t m_num_outputs;
    QString m_device;
    qreal m_level;
};
