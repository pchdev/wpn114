#include "mangler.hpp"

Mangler::Mangler()
{
    SETN_IN     ( 2 );
    SETN_OUT    ( 2 );
}

void Mangler::initialize(qint64)
{

}

float** Mangler::process(float**, qint64)
{
    return m_out;
}
