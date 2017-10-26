/*
 * =====================================================================================
 *
 *       Filename:  sndf.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  15.10.2017 17:46:12
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#pragma once
#include <string>
#include <sndfile.h>

namespace wpn114 {
namespace audio {

template <typename T> struct sndbuf_t
{
    uint8_t         num_channels;
    user_size_t     num_frames;
    user_size_t     num_samples;
    user_size_t     sample_rate;
    T*              data;
};

template<typename T> static sndbuf_t<T> load_soundfile(const std::string& path_to_soundfile)
{
    SNDFILE*    infile;
    SF_INFO     sfinfo;
    int         readcount;

    std::memset(&sfinfo, 0, sizeof(sfinfo));

    if(!(infile = sf_open (path_to_soundfile.c_str(), SFM_READ, &sfinfo)))
    {
        // throw error
    }

    // load file contents into buffer
    sndbuf_t<T> buffer    = {0, 0, 0, 0};
    buffer.data = (T*) malloc(sfinfo.frames * sfinfo.channels * sizeof(T));
    // remember that sndbuffers_t are interleaved

    // puts contents into buffer
    if(sizeof(T) == sizeof(float))
        sf_count_t frames_read  = sf_read_float(infile, buffer.data, sfinfo.frames);

    buffer.num_channels     = sfinfo.channels;
    buffer.num_frames       = sfinfo.frames;
    buffer.num_samples      = sfinfo.frames / sfinfo.channels;
    buffer.sample_rate      = sfinfo.samplerate;

    return buffer;
}

}
}


