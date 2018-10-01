#include "sampler.hpp"

#include <cmath>


Sampler::Sampler() : m_loop(false), m_xfade(0), m_attack(0), m_release(0),
    m_start(0), m_length(0), m_rate(1.0), m_stream(false), m_next_buffer_ready(false)
{
    // building sin envelopes
    for ( int i = 0; i < ENV_RESOLUTION; ++ i )
    {
        auto env_value    = sin ((float)i/ENV_RESOLUTION*M_PI_2);
        m_attack_env[i]   = env_value;
        m_release_env[i]  = 1.f-env_value;
    }
}

void Sampler::componentComplete()
{
    if ( m_path.isEmpty() ) return;

    m_sfile         = new Soundfile(m_path);
    quint64 nch     = m_sfile->nchannels();
    quint64 srate   = m_sfile->sampleRate();

    SETN_OUT ( nch );

    if ( m_stream )
    {
        m_streamer = new SoundfileStreamer(m_sfile);

        m_streamer->setStartSample  ( m_start*srate );
        m_streamer->setBufferSize   ( BUFSTREAM_NSAMPLES_DEFAULT );
        m_streamer->setWrap         ( m_loop );

        // allocate buffers (in frames, interleaved)
        m_current_buffer  = new float[ BUFSTREAM_NSAMPLES_DEFAULT*nch ];
        m_next_buffer     = new float[ BUFSTREAM_NSAMPLES_DEFAULT*nch ];
        m_buffer_size     = BUFSTREAM_NSAMPLES_DEFAULT;

        // load first buffer
        m_streamer->moveToThread(&m_streamer_thread);
        QObject::connect(this, &Sampler::next, m_streamer, &SoundfileStreamer::next);
        QObject::connect(m_streamer, &SoundfileStreamer::bufferLoaded, this, &Sampler::onNextBufferReady);

        emit next(m_next_buffer);
    }
    else // single static buffer
    {
        quint64 len = floor(m_length*srate);
        m_current_buffer = new float[len*nch]();

        m_sfile->buffer(m_current_buffer, m_start*srate, len);
    }

}

void Sampler::setStream(bool stream)
{
    m_stream = stream;
}

void Sampler::setPath(QString path)
{
    m_path = path;
}

void Sampler::setLoop(bool loop)
{
    m_loop = loop;
}

void Sampler::setXfade(quint32 xfade)
{
    m_xfade = xfade;
}

void Sampler::setAttack(quint32 attack)
{
    m_attack        = attack;
    m_attack_end    = attack/1000.f*SAMPLERATE;
}

void Sampler::setRelease(quint32 release)
{
    m_release = release;
}

void Sampler::setStart(qreal start)
{
    m_start = start;
}

void Sampler::setLength(qreal length)
{
    m_length = length;
}

void Sampler::setRate(qreal rate)
{
    m_rate = rate;
}

qreal Sampler::fileLength() const
{
    return (qreal)m_sfile->nsamples()/(qreal)m_sfile->nchannels();
}

void Sampler::userInitialize(qint64)
{

}

void Sampler::onNextBufferReady()
{
    m_next_buffer_ready = true;
}

inline float lininterp(float x, float a, float b)
{
    return a + x * (b - a);
}

float** Sampler::userProcess(float**, qint64 le)
{
    auto bufdata        = m_current_buffer;
    auto bufnsamples    = m_buffer_size;
    auto playnsamples   = m_play_size;
    auto first          = m_first_play;
    auto spos           = m_file_phase;
    auto bpos           = m_buffer_phase;
    auto out            = m_out;
    auto nch            = m_num_outputs;
    auto loop           = m_loop;
    auto stream         = m_stream;

    auto attack         = m_attack_env;
    auto attack_end     = m_attack_end;
    auto attack_phase   = m_attack_phase;
    auto attack_inc     = m_attack_inc;

    auto xfade_point    = m_xfade_point;
    auto xfade_phase    = m_xfade_phase;
    auto xfade_inc      = m_xfade_inc;

    if ( loop && spos > attack_end ) m_first_play = false;

    for ( qint64 s = 0; s < le; ++s )
    {
        if ( stream && m_buffer_phase == bufnsamples )
        {
            //------------------ if streaming, swap buffers
            auto tmp            = m_current_buffer;
            m_current_buffer    = m_next_buffer;
            bufdata             = m_next_buffer;

            m_next_buffer       = tmp;
            m_next_buffer_ready = false;

            // start preping next buffer (asynchronous)
            emit next(m_next_buffer);
        }

        if ( first && spos < attack_end )
        {
            //          if first play && phase is in the 'attack zone'
            //          get interpolated data from envelope
            int y       = floor(attack_phase);
            float x     = (float) attack_phase-y;
            float e     = lininterp(x, attack[y], attack[y+1]);

            for ( int ch = 0; ch < nch; ++ch )
            {
                out[ch][s] = *bufdata*e;
                bufdata++;
            }

            spos++;
            attack_phase += attack_inc;
        }
        else if ( loop && spos >= xfade_point && spos < playnsamples )
        {
            //          if loop mode and if phase is in the 'crossfade zone'
            //          get interpolated data from envelope
            int y       = floor(xfade_phase);
            float x     = (float) xfade_phase-y;
            float xfu   = lininterp(x, attack[y], attack[y+1]);
            float xfd   = 1.f - xfu;

            for ( int ch = 0; ch < nch; ++ch )
            {
                float* rphs = bufdata-xfade_point*nch;
                out[ch][s]  = *bufdata*xfd + *rphs*xfu;

                bufdata++;
            }

        }
        else if ( spos == playnsamples )
        {
            if ( loop )
            {
                // if phase reaches end of 'crossfade zone'
                // main phase continues from end of 'up' crossfade
                // reset envelope phase
            }
            else
            {
                // if not looping, stop and set inactive
                m_active = false;
            }

        }

        else
        {
            // normal behaviour
        }


        // check streaming buffer

    }

}
