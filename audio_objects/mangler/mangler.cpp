#include "mangler.hpp"

Mangler::Mangler()
{
    SETN_IN     ( 2 );
    SETN_OUT    ( 2 );
}

void Mangler::userInitialize(qint64)
{

}

float** Mangler::userProcess(float**, qint64)
{
    return m_out;
}
