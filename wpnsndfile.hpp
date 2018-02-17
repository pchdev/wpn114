#ifndef WPNSNDFILE_HPP
#define WPNSNDFILE_HPP

#include <sndfile.h>
#include <cstdint>
#include <iostream>
#include <QIntegerForSize>

struct sndbuf
{
    sndbuf ( std::string const& filepath, quint64 frames = 0 );

    void    update      ( float*&rptr, quint64 start, quint64 len );
    void    suspend     ( );
    void    resume      ( );

    ~sndbuf();

    uint8_t         nchannels;
    size_t          nframes;
    size_t          nsamples;
    size_t          sample_rate;
    float*          data;
    std::string     path;
    SF_INFO         sfinfo;

private:
    SNDFILE*    sndfile;


};

#endif // WPNSNDFILE_HPP
