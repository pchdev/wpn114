#include "wpnsndfile.hpp"
#include <QtDebug>

sndbuf::sndbuf(std::string const& filepath, quint64 frames ) : sndfile(0), path(filepath)
{
    std::memset( &sfinfo, 0, sizeof(sfinfo) );

    if ( !(sndfile = sf_open ( filepath.c_str(), SFM_READ, &sfinfo )))
    {
        qDebug() << "could not open soundfile, aborting...";
        return;
    }

    // don't forget that if nframes is specified, the real total of frames would be
    // sfinfo.frames * sfinfo.channels

    if ( !frames )   nframes = sfinfo.frames*sfinfo.channels;
    else             nframes = frames;

    data             = (float*) malloc (nframes * sizeof(float));

    sf_readf_float   ( sndfile, data, nframes );

    nchannels        = sfinfo.channels;
    nsamples         = sfinfo.frames;
    sample_rate      = sfinfo.samplerate;
}

sndbuf::~sndbuf()
{
    if ( data ) delete data;
    sf_close ( sndfile );
}

void sndbuf::update(float* &rptr, quint64 sndf_start, quint64 len)
{
    sf_seek         ( sndfile, len, sndf_start );
    sf_readf_float  ( sndfile, rptr, len );
}

void sndbuf::suspend()
{
    sf_close ( sndfile );
}

void sndbuf::resume()
{
    sf_open ( path.c_str(), SFM_READ, &sfinfo );
}
