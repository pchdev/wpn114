#include "rooms.hpp"

CircularSetup::CircularSetup()
{

}

void CircularSetup::setOffset(qint16 offset)
{
    m_offset = offset;
}

//---------------------------------------------------------------------------------------------------------

RoomNode::RoomNode()
{

}

void RoomNode::setNspeakers(quint16 nspeakers)
{
    m_nspeakers = nspeakers;
}

//---------------------------------------------------------------------------------------------------------

RoomSetup::RoomSetup()
{

}

//---------------------------------------------------------------------------------------------------------

RoomSource::RoomSource()
{

}

void RoomSource::setBias(QVariant bias)
{
    m_bias = bias;
}

void RoomSource::setDiffuse(QVariant diffuse)
{
    m_diffuse = diffuse;
}

void RoomSource::setPosition(QVariant position)
{
    m_position = position;
}

void RoomSource::userInitialize(qint64 nsamples)
{

}

float** RoomSource::userProcess(float**buf, qint64 nsamples)
{

}

//---------------------------------------------------------------------------------------------------------

Rooms::Rooms()
{

}

void Rooms::setSetup(RoomSetup* setup)
{
    m_setup = setup;
}

void Rooms::userInitialize(qint64 nsamples)
{

}

float** Rooms::process(float** buf, qint64 nsamples)
{
    for ( const auto& node : m_subnodes )
    {
        RoomSource* source = qobject_cast<RoomSource*>(node);
        if ( ! source ) continue;

        //...

    }

}

// no need to call userProcess, as process is already overriden
float** Rooms::userProcess(float** buf, qint64 nsamples) { }
