#pragma once
#include "RtMidi.h"
#include <QObject>
#include <QQmlParserStatus>
#include <QVariant>

//-------------------------------------------------------------------------------------------------------
enum class MIDIStatus
//-------------------------------------------------------------------------------------------------------
{
    SYSEX                   = 0xf0,
    EOX                     = 0xf7,
    NOTE_OFF                = 0x80,
    NOTE_ON                 = 0x90,
    AFTERTOUCH              = 0xa0,
    CONTINUOUS_CONTROL      = 0xb0,
    PATCH_CHANGE            = 0xc0,
    CHANNEL_PRESSURE        = 0xd0,
    PITCH_BEND              = 0xe0
};

class MIDIHandler : public QObject, public QQmlParserStatus
{
    Q_OBJECT

    Q_PROPERTY  ( int outPort READ outPort WRITE setOutPort NOTIFY outPortChanged )
    Q_PROPERTY  ( int inPort READ inPort WRITE setInPort NOTIFY inPortChanged )
    Q_PROPERTY  ( QString outDevice READ outDevice WRITE setOutDevice NOTIFY outDeviceChanged )
    Q_PROPERTY  ( QString inDevice READ inDevice WRITE setInDevice NOTIFY inDeviceChanged )
    Q_PROPERTY  ( QString inVirtualPort READ inVirtualPort WRITE setInVirtualPort )
    Q_PROPERTY  ( QString outVirtualPort READ outVirtualPort WRITE setOutVirtualPort )

    Q_INTERFACES ( QQmlParserStatus )

    public:
    MIDIHandler     ( );
    ~MIDIHandler    ( ) override;

    virtual void classBegin             () override;
    virtual void componentComplete      () override;

    int outPort  ( ) const { return m_out_port; }
    int inPort   ( ) const { return m_in_port;  }

    QString outDevice       () const { return m_out_device; }
    QString inDevice        () const { return m_in_device; }
    QString inVirtualPort   () const { return m_virtual_port; }
    QString outVirtualPort  () const { return m_virtual_port; }

    void setOutPort         ( int8_t port );
    void setInPort          ( int8_t port );

    void setOutDevice       ( QString device );
    void setInDevice        ( QString device );

    void setInVirtualPort   ( QString name );
    void setOutVirtualPort  ( QString name );

    Q_INVOKABLE void noteOn         ( int channel, int index, int value );
    Q_INVOKABLE void noteOff        ( int channel, int index, int value );
    Q_INVOKABLE void control        ( int channel, int index, int value );
    Q_INVOKABLE void program        ( int channel, int index);
    Q_INVOKABLE void aftertouch     ( int channel, int index, int value );
    Q_INVOKABLE void bend           ( int channel, int value );
    Q_INVOKABLE void pressure       ( int channel, int value );
    Q_INVOKABLE void sendRaw        ( QByteArray msg );
    Q_INVOKABLE void sendVariant    ( QVariantList list );

    Q_INVOKABLE QStringList outDevices  () const;
    Q_INVOKABLE QStringList inDevices   () const;

    signals:
    void outPortChanged     ( int );
    void inPortChanged      ( int );
    void outDeviceChanged   ( QString );
    void inDeviceChanged    ( QString );
    void inPortOpen         ( );
    void outPortOpen        ( );

    void noteOnReceived             ( int channel, int index, int velocity );
    void noteOffReceived            ( int channel, int index, int velocity );
    void controlReceived            ( int channel, int index, int value );
    void channelPressureReceived    ( int channel, int value );
    void aftertouchReceived         ( int channel, int index, int value );
    void pitchBendReceived          ( int channel, int value );
    void programReceived            ( int channel, int index );
    void eventReceived              ( QByteArray event );

    private:
    RtMidiIn*   m_rt_in;
    RtMidiOut*  m_rt_out;
    int8_t      m_out_port;
    int8_t      m_in_port;
    QString     m_out_device;
    QString     m_in_device;
    QString     m_virtual_port;
};
