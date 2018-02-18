#include "fields.h"
#include <math.h>

#define AUDIO_BUFFER_SIZE 2 << 17

Fields::Fields() : m_path(""), m_xfade(65536),
    m_spos(0), m_epos(0)
{
    SET_OFFSET  ( 0 );
}

Fields::~Fields()
{
    if ( m_buf ) delete m_buf;
}

void Fields::setNumInputs(const quint16) {}
void Fields::classBegin() {}
void Fields::componentComplete()
{
    // build sin xfade envelope
    for ( int i = 0; i < ENVSIZE; ++ i )
        m_env[i] = sin ( (float) i/ENVSIZE*M_PI_2 );

    // full memory buffering is convenient for the crossfade
    // but implementing both streaming & fullmem would be a good choice, for longer files
    // DiskFields audio object to implement then...

    m_buf = new sndbuf ( m_path.toStdString(), 0);

    m_xfade_point   = m_buf->nsamples - m_xfade;
    m_env_incr      = (float) ENVSIZE / m_xfade;

    SETN_OUT ( m_buf->nchannels );
    INITIALIZE_AUDIO_OUTPUTS;

    emit bufferReady( );
}

inline float lininterp(float x, float a, float b)
{
    return a + x * (b - a);
}

float** Fields::process(const quint16 nsamples)
{
    auto spos           = m_spos;
    auto epos           = m_epos;
    auto env            = m_env;
    auto eicr           = m_env_incr;
    auto bufnsamples    = m_buf->nsamples;
    auto nch            = m_buf->nchannels;
    auto bufdata        = m_buf->data;
    auto xfp            = m_xfade_point;
    auto xfl            = m_xfade;
    auto out            = OUT;

    // get buffer back in position
    bufdata += spos*nch;

    for ( int s = 0; s < nsamples; ++s )
    {
        if ( spos >= xfp && spos < nsamples )
        {
            //              if phase is in the crossfade zone
            //              get interpolated data from envelope
            int y           = floor( epos );
            float x         = (float) epos - y;
            float xfu       = lininterp( x, env[y], env[y+1] );
            float xfd       = 1.f-xfu;

            for ( int ch = 0; ch < nch; ++ ch )
            {
                //  normal phase * xfade 'down' mixed w/
                // 'reset' phase * xfade 'up
                float* rphs     = bufdata-xfp*nch;
                out[ch][s]      = *bufdata * xfd + *rphs * xfu;

                bufdata++;
            }

            spos++;
            epos += eicr;
        }
        else if ( spos  == bufnsamples )
        {
            // if phase reaches end of crossfade
            // main phase continues from end of 'up' crossfade
            // reset envelope phase
            bufdata     = &m_buf->data[xfl*nch];
            epos        = 0;

            for ( int ch = 0; ch < nch; ++ch )
                  out[ch][s]  = *bufdata++;

            spos        = xfl+1;
        }
        else
        {
            // normal behaviour
            for ( int ch = 0; ch < nch; ++ch )
                  out[ch][s]  = *bufdata++;

            spos++;
        }
    }

    m_spos = spos;
    m_epos = epos;

    return m_outputs;
}

uint32_t Fields::xfade() const
{
    return m_xfade;
}

QString Fields::path() const
{
    return m_path;
}

void Fields::setXfade(const uint32_t xfadelen)
{
    m_xfade = xfadelen;
}

void Fields::setPath(const QString path)
{
    m_path = path;
}
