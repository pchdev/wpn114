/*
 * =====================================================================================
 *
 *       Filename:  sndf.cpp
 *
 *    Description:  :
 *
 *        Version:  1.0
 *        Created:  15.10.2017 17:39:46
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <wpn114/audio/backend/sndfile_support.hpp>

int wpn114::audio::load_soundfile(sndbuf_t& buffer, const std::string& path_to_soundfile)
{
    SNDFILE*    infile;
    SF_INFO     sfinfo;
    int         readcount;

    std::memset(&sfinfo, 0, sizeof(sfinfo));

    if(!(infile = sf_open (path_to_soundfile.c_str(), SFM_READ, &sfinfo)))
    {
        std::cerr << "could not open soundfile, aborting..\n";
        return -1;
    }

    buffer.data = (float*) malloc(sfinfo.frames * sfinfo.channels * sizeof(float));

    // puts contents into buffer
    readcount = sf_readf_float(infile, buffer.data, sfinfo.frames);

    buffer.num_channels     = sfinfo.channels;
    buffer.num_frames       = sfinfo.frames * sfinfo.channels;
    buffer.num_samples      = sfinfo.frames;
    buffer.sample_rate      = sfinfo.samplerate;

    sf_close(infile);

    return 0;
}
