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
    uint8_t         num_channels;
    user_size_t     num_frames;
    user_size_t     num_samples;
    user_size_t     sample_rate;
    float*          data;
};

int load_soundfile(sndbuf_t& buffer, const std::string& path_to_soundfile);

}
}


