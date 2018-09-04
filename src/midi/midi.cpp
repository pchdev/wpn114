#include "midi.hpp"
#include <QVariantList>
#include <QtDebug>

void ReceiveCallback(double delta, std::vector<uint8_t>* msg, void* userData )
{
    auto& msg_d         = *msg;
    MIDIHandler* hdl    = static_cast<MIDIHandler*>(userData);
    QByteArray* buf     = new QByteArray(reinterpret_cast<const char*>(msg->data()), msg->size());

    hdl->eventReceived(*buf);

    if ( msg_d[0] >= 0x80 && msg_d[0] <= 0x8f )
        hdl->noteOffReceived(msg_d[0]-0x80, msg_d[1], msg_d[2]);

    else if ( msg_d[0] >= 0x90 && msg_d[0] <= 0x9f )
        hdl->noteOnReceived(msg_d[0]-0x90, msg_d[1], msg_d[2]);

    else if ( msg_d[0] >= 0xb0 && msg_d[0] <= 0xbf )
        hdl->controlReceived(msg_d[0]-0xb0, msg_d[1], msg_d[2]);

    else if ( msg_d[0] >= 0xa0 && msg_d[0] <= 0xaf )
        hdl->aftertouchReceived(msg_d[0]-0xa0, msg_d[1], msg_d[2]);

    else if ( msg_d[0] >= 0xc0 && msg_d[0] <= 0xcf )
        hdl->programReceived(msg_d[0]-0xc0, msg_d[1]);

    else if ( msg_d[0] >= 0xe0 && msg_d[0] <= 0xef )
    {
        int value = (msg_d[2] << 7) | msg_d[1];
        hdl->pitchBendReceived(msg_d[0]-0xe0, value);
    }

    else if ( msg_d[0] >= 0xd0 && msg_d[0] <= 0xdf )
        hdl->channelPressureReceived(msg_d[0]-0xd0, msg_d[1]);
}

MIDIHandler::MIDIHandler() :
    m_out_port(-1), m_in_port(-1),
    m_rt_in(new RtMidiIn), m_rt_out(new RtMidiOut)
{

}

MIDIHandler::~MIDIHandler()
{
    m_rt_in->closePort();
    m_rt_out->closePort();

    delete m_rt_in;
    delete m_rt_out;
}

void MIDIHandler::classBegin() {}
void MIDIHandler::componentComplete() { }

void MIDIHandler::setInVirtualPort(QString name)
{
    if ( m_rt_in->isPortOpen() ) m_rt_in->closePort();

    m_rt_in->openVirtualPort(name.toStdString());
    m_rt_in->setCallback(ReceiveCallback, static_cast<void*>(this));

    m_in_device = name;
    m_in_port = outDevices().indexOf(name);

    emit inPortOpen();
}

void MIDIHandler::setOutVirtualPort(QString name)
{
    if ( m_rt_out->isPortOpen() ) m_rt_out->closePort();
    m_rt_out->openVirtualPort(name.toStdString());

    m_out_device = name;
    m_out_port = outDevices().indexOf(name);

    emit outPortOpen();
}

QStringList MIDIHandler::outDevices() const
{
    QStringList res;
    for ( int i = 0; i < m_rt_out->getPortCount(); ++i )
        res << QString::fromStdString(m_rt_out->getPortName(i));

    return res;
}

QStringList MIDIHandler::inDevices() const
{
    QStringList res;
    for ( int i = 0; i < m_rt_in->getPortCount(); ++i )
        res << QString::fromStdString(m_rt_in->getPortName(i));

    return res;
}

void MIDIHandler::setOutPort(int8_t port)
{
    if ( m_out_port != port )
    {
        emit outPortChanged(port);
        m_out_port = port;
        if ( m_rt_out->isPortOpen()) m_rt_out->closePort();
        m_rt_out->openPort(port);

        emit outPortOpen();
    }
}

void MIDIHandler::setInPort(int8_t port)
{
    if ( m_in_port != port )
    {
        emit inPortChanged(port);
        m_in_port = port;
        if ( m_rt_in->isPortOpen()) m_rt_in->closePort();
        m_rt_in->openPort(port);        
        m_rt_in->setCallback(ReceiveCallback, static_cast<void*>(this));

        emit inPortOpen();
    }
}

void MIDIHandler::setInDevice(QString device)
{
    if ( m_in_device != device )
    {
        emit inDeviceChanged(device);
        m_in_device = device;

        int i = 0;
        for ( const auto& indev : inDevices() )
        {
            if ( indev == device )
                setInPort(i);
            ++i;
        }
    }
}

void MIDIHandler::setOutDevice(QString device)
{
    if ( m_out_device != device )
    {
        emit outDeviceChanged(device);
        m_out_device = device;

        int i = 0;
        for ( const auto& outdev : outDevices() )
        {
            if ( outdev == device )
                setOutPort(i);
            ++i;
        }
    }
}

void MIDIHandler::noteOn(int channel, int index, int value)
{
    std::vector<uint8_t> msg = { static_cast<uint8_t>(0x90+channel),
                                 static_cast<uint8_t>(index),
                                 static_cast<uint8_t>(value) };

    m_rt_out->sendMessage ( &msg );
}

void MIDIHandler::noteOff(int channel, int index, int value)
{
    std::vector<uint8_t> msg = { static_cast<uint8_t>(0x80+channel),
                                 static_cast<uint8_t>(index),
                                 static_cast<uint8_t>(value) };

    m_rt_out->sendMessage ( &msg );
}

void MIDIHandler::control(int channel, int index, int value)
{
    std::vector<uint8_t> msg = { static_cast<uint8_t>(0xb0+channel),
                                 static_cast<uint8_t>(index),
                                 static_cast<uint8_t>(value) };

    m_rt_out->sendMessage ( &msg );
}

void MIDIHandler::program(int channel, int index)
{
    std::vector<uint8_t> msg = { static_cast<uint8_t>(0xc0+channel),
                                 static_cast<uint8_t>(index) };

    m_rt_out->sendMessage ( &msg );
}

void MIDIHandler::aftertouch(int channel, int index, int value)
{
    std::vector<uint8_t> msg = { static_cast<uint8_t>(0xa0+channel),
                                 static_cast<uint8_t>(index),
                                 static_cast<uint8_t>(value) };

    m_rt_out->sendMessage ( &msg );
}

void MIDIHandler::bend(int channel, int value)
{
    uint8_t msb = value >> 7;
    uint8_t lsb = value - ( msb << 7 );

    std::vector<uint8_t> msg = { static_cast<uint8_t>(0xe0+channel), lsb, msb };

    m_rt_out->sendMessage ( &msg );
}

void MIDIHandler::pressure(int channel, int value)
{
    std::vector<uint8_t> msg = { static_cast<uint8_t>(0xd0+channel),
                               static_cast<uint8_t>(value) };

    m_rt_out->sendMessage( &msg );
}

void MIDIHandler::sendRaw(QByteArray msg)
{
    std::vector<uint8_t> vec(msg.begin(), msg.end());
    m_rt_out->sendMessage(&vec);
}

void MIDIHandler::sendVariant(QVariantList list)
{
    QByteArray arr;
    for ( const QVariant& var : list )
    {
        if ( var.type() == QMetaType::QString )
            for ( const QChar& c : var.toString() )
                arr.append(c.toLatin1());
        else arr.append(var.toInt());
    }

    std::vector<uint8_t> vec(arr.begin(), arr.end());
    m_rt_out->sendMessage(&vec);
}
