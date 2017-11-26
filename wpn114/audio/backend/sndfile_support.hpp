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
#include <iostream>
#include <exception>

namespace wpn114 {
namespace audio {

struct sndbuf_t
{
    uint8_t         nchannels;
    size_t          nframes;
    size_t          nsamples;
    size_t          sample_rate;
    float*          data;               // interleaved
};

int load_soundfile(sndbuf_t& buffer, const std::string& path_to_soundfile);

}
}


