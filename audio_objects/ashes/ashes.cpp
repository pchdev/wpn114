#include "ashes.hpp"

#define PINK_MAX_RANDOM_ROWS    ( 30 )
#define PINK_RANDOM_BITS        ( 24 )
#define PINK_RANDOM_SHIFT       ( (sizeof(long)*8)-PINK_RANDOM_BITS )

Ashes::Ashes()
{
    SETN_OUT    ( 1 );
    SET_OFFSET  ( 0 );
}

void Ashes::classBegin() {}

void Ashes::componentComplete()
{
    INITIALIZE_AUDIO_OUTPUTS;
}

float** Ashes::process(const quint16 nsamples)
{

}
