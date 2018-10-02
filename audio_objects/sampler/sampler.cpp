#include "sampler.hpp"

#include <cmath>

#define BUFSR m_soundfile->sampleRate()

StreamSampler::StreamSampler()
{
    m_soundfile             = nullptr;
    m_streamer              = nullptr;
    m_current_buffer        = nullptr;
    m_next_buffer           = nullptr;
    m_next_buffer_ready     = false;
    m_first_play            = false;
    m_loop                  = false;
    m_buffer_size           = 0;
    m_play_size             = 0;
    m_phase                 = 0;
    m_stream_phase          = 0;
    m_attack_phase          = 0;
    m_release_phase         = 0;
    m_xfade_phase           = 0;
    m_attack_end            = 0;
    m_xfade_point           = 0;
    m_xfade_length          = 0;
    m_attack_inc            = 0;
    m_release_inc           = 0;
    m_xfade_inc             = 0;

    m_xfade                 = 0;
    m_attack                = 0;
    m_release               = 0;
    m_start                 = 0.f;
    m_end                   = 0.f;
    m_length                = 0.f;
    m_rate                  = 1.f;

    // building sin envelopes
    for ( int i = 0; i < ENV_RESOLUTION; ++ i )
    {
        auto env_value    = sin ((float)i/ENV_RESOLUTION*M_PI_2);
        m_attack_env[i]   = env_value;
        m_release_env[i]  = 1.f-env_value;
    }
}

StreamSampler::~StreamSampler()
{
    delete m_streamer;
    delete m_soundfile;
    delete m_current_buffer;
    delete m_next_buffer;
}

inline quint64 ms_to_samples(quint64 x, quint64 sr)
{
    return x/1000.f*sr;
}

void StreamSampler::setLoop(bool loop)
{
    m_loop = loop;

    if ( m_streamer ) m_streamer->setWrap(loop);
}

void StreamSampler::setXfade(quint32 xfade)
{
    m_xfade = xfade;
    m_xfade_length = ms_to_samples(xfade, SAMPLERATE);
}

void StreamSampler::setAttack(quint32 attack)
{
    m_attack = attack;
    m_attack_end = ms_to_samples(attack, SAMPLERATE);
}

void StreamSampler::setRelease(quint32 release)
{
    m_release = release;
}

void StreamSampler::setStart(qreal start)
{
    m_start = start;

    if ( m_streamer ) m_streamer->setStartSample( m_start*BUFSR );
}

void StreamSampler::setEnd(qreal end)
{
    m_end       = end;
    m_length    = end-m_start;

    if ( m_streamer ) m_streamer->setEndSample ( m_end*BUFSR);
}

void StreamSampler::setLength(qreal length)
{
    m_length    = length;
    m_end       = m_start + length;

    if ( m_streamer ) m_streamer->setEndSample ( m_end*BUFSR );
}

void StreamSampler::setRate(qreal rate)
{
    m_rate = rate;
}

void StreamSampler::setPath(QString path)
{
    m_path = path;
}

void StreamSampler::componentComplete()
{
    if ( m_path.isEmpty() ) return;

    m_soundfile  = new Soundfile(m_path);
    m_streamer   = new SoundfileStreamer(m_soundfile);

    quint64 nch     = m_soundfile->nchannels();
    quint64 srate   = m_soundfile->sampleRate();

    if ( m_length = 0 ) setLength((qreal) m_soundfile->nsamples()/srate);

    SETN_OUT ( nch );

    m_streamer->setStartSample  ( m_start*srate );
    m_streamer->setEndSample    ( m_end*srate );
    m_streamer->setBufferSize   ( BUFSTREAM_NSAMPLES_DEFAULT );
    m_streamer->setWrap         ( m_loop );

    // allocate buffers (in frames, interleaved)
    m_current_buffer  = new float[ BUFSTREAM_NSAMPLES_DEFAULT*nch ];
    m_next_buffer     = new float[ BUFSTREAM_NSAMPLES_DEFAULT*nch ];
    m_buffer_size     = BUFSTREAM_NSAMPLES_DEFAULT;

    // load first buffer
    m_streamer->moveToThread(&m_streamer_thread);
    QObject::connect(this, &StreamSampler::next, m_streamer, &SoundfileStreamer::next);
    QObject::connect(m_streamer, &SoundfileStreamer::bufferLoaded, this, &StreamSampler::onNextBufferReady);

    emit next(m_next_buffer);
}

void StreamSampler::userInitialize(qint64) {}

void StreamSampler::onNextBufferReady()
{
    m_next_buffer_ready = true;
}

inline float lininterp(float x, float a, float b)
{
    return a + x * (b - a);
}

float** StreamSampler::userProcess(float** buf, qint64 nsamples)
{
    auto bufdata        = m_current_buffer;
    auto bufnsamples    = m_buffer_size;
    auto playnsamples   = m_play_size;
    auto first          = m_first_play;
    auto spos           = m_phase;
    auto bpos           = m_stream_phase;
    auto out            = m_out;
    auto nch            = m_num_outputs;
    auto loop           = m_loop;

    auto attack         = m_attack_env;
    auto attack_end     = m_attack_end;
    auto attack_phase   = m_attack_phase;
    auto attack_inc     = m_attack_inc;
    auto xfade_point    = m_xfade_point;
    auto xfade_phase    = m_xfade_phase;
    auto xfade_inc      = m_xfade_inc;
    auto xfade_length   = m_xfade_length;

    // check for attack
    if ( loop && spos > attack_end ) m_first_play = false;

    for ( qint64 s = 0; s < nsamples; ++s )
    {
        if ( bpos == bufnsamples )
        {
            //----------------- swap buffers
            auto tmp            = m_current_buffer;
            m_current_buffer    = m_next_buffer;
            bufdata             = m_next_buffer;

            m_next_buffer       = tmp;
            m_next_buffer_ready = false;

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
            bpos++;
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

            spos++;
            bpos++;
            xfade_phase += xfade_inc;
        }

        else if ( spos == playnsamples )
        {
            if ( loop )
            {
                // if phase reaches end of 'crossfade zone'
                // main phase continues from end of 'up' crossfade
                // reset envelope phase
                spos = xfade_length+1;
                xfade_phase = 0;
            }
            else
            {
                 // if not looping, stop and set inactive
                // fill rest of buffer with zeroes
                m_active = false;

                for ( int ch = 0; ch < nch; ++ch )
                    out[ch][s] = 0.f;
            }
        }

        else if ( spos > playnsamples )
        {
            // it would mean that loop is off
            // fill rest of buffer with zeroes before going inactive
            for ( int ch = 0; ch < nch; ++ch )
                  out[ch][s] = 0.f;
        }
        else
        {
            // normal behaviour
            for ( int ch = 0; ch < nch; ++ch )
                out[ch][s] = 0.f;

            spos++;
            bpos++;
        }
    }

    m_phase             = spos;
    m_stream_phase      = bpos;
    m_attack_phase      = attack_phase;
    m_xfade_phase       = xfade_phase;
}

//-----------------------------------------------------------------------------------

Sampler::Sampler()
{    
    m_soundfile     = nullptr;
    m_buffer        = nullptr;
    m_first_play    = true;
    m_buffer_size   = 0;
    m_phase         = 0;
    m_attack_phase  = 0;
    m_release_phase = 0;
    m_xfade_phase   = 0;
    m_attack_end    = 0;
    m_xfade_point   = 0;
    m_xfade_length  = 0;
    m_attack_inc    = 0;
    m_release_inc   = 0;
    m_xfade_inc     = 0;
    m_loop          = false;
    m_xfade         = 0;
    m_attack        = 0;
    m_release       = 0;
    m_start         = 0.f;
    m_end           = 0.f;
    m_length        = 0.f;
    m_rate          = 1.f;

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

    m_soundfile     = new Soundfile(m_path);
    quint16 nch     = m_soundfile->nchannels();
    quint64 srate   = m_soundfile->sampleRate();

    if ( m_length = 0 ) setLength((qreal) m_soundfile->nsamples()/srate);

    quint64 len     = floor ( m_length*srate );
    m_buffer        = new float[len*nch]();

    m_soundfile->buffer ( m_buffer, m_start*srate, len );

    m_xfade_point = len-m_xfade_length;
    m_xfade_inc   = static_cast<float>(ENV_RESOLUTION/ms_to_samples(m_xfade, SAMPLERATE));
    m_attack_inc  = static_cast<float>(ENV_RESOLUTION/ms_to_samples(m_attack, SAMPLERATE));

    SETN_OUT ( nch );

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
    m_xfade         = xfade;
    m_xfade_length  = ms_to_samples(xfade, SAMPLERATE);
}

void Sampler::setAttack(quint32 attack)
{
    m_attack        = attack;
    m_attack_end    = ms_to_samples(attack, SAMPLERATE);
}

void Sampler::setRelease(quint32 release)
{
    m_release = release;
}

void Sampler::setStart(qreal start)
{
    m_start = start;
}

void Sampler::setEnd(qreal end)
{
    m_end       = end;
    m_length    = end-m_start;
}

void Sampler::setLength(qreal length)
{
    m_length    = length;
    m_end       = m_start + length;
}

void Sampler::setRate(qreal rate)
{
    m_rate = rate;
}

void Sampler::userInitialize(qint64)
{

}

float** Sampler::userProcess(float**, qint64 le)
{
    auto bufdata        = m_buffer;
    auto bufnsamples    = m_buffer_size;
    auto first          = m_first_play;
    auto spos           = m_phase;
    auto out            = m_out;
    auto nch            = m_num_outputs;
    auto loop           = m_loop;

    auto attack         = m_attack_env;
    auto attack_end     = m_attack_end;
    auto attack_phase   = m_attack_phase;
    auto attack_inc     = m_attack_inc;

    auto xfade_point    = m_xfade_point;
    auto xfade_phase    = m_xfade_phase;
    auto xfade_inc      = m_xfade_inc;
    auto xfade_len      = m_xfade_length;

    if ( loop && spos > attack_end ) m_first_play = false;

    for ( qint64 s = 0; s < le; ++s )
    {
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
        else if ( loop && spos >= xfade_point && spos < bufnsamples )
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
        else if ( spos == bufnsamples )
        {
            if ( loop )
            {
                // if phase reaches end of 'crossfade zone'
                // main phase continues from end of 'up' crossfade
                // reset envelope phase
                bufdata         = &m_buffer[xfade_len*nch];
                xfade_phase     = 0;

                for ( int ch = 0; ch < nch; ++ch )
                    out[ch][s] = *bufdata++;

                spos = xfade_len+1;
            }
            else
            {
                // if not looping, stop and set inactive
                // zero out rest of the buffer
                m_active = false;

                for ( int ch = 0; ch < nch; ++ch )
                    out[ch][s] = 0.f;
            }
        }

        else if ( spos > bufnsamples )
        {
            // zero out rest of the buffer
            for ( int ch = 0; ch < nch; ++ch )
                out[ch][s] = 0.f;

            spos++;
        }

        else
        {
            // normal behaviour
            for ( int ch = 0; ch < nch; ++ch )
                out[ch][s] = *bufdata++;

            spos++;
        }
    }

    // update member values
    m_phase         = spos;
    m_attack_phase  = attack_phase;
    m_xfade_phase   = xfade_phase;
}
