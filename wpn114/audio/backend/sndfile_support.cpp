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
#include <sndfile.h>
#include <wpn114/audio/backend/sndfile_support.hpp>

template<typename T> T load_soundfile(std::string &path_to_soundfile)
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
    T buffer    = nullptr;
    buffer      = (T*) malloc(sfinfo.frames * sfinfo.channels * sizeof(T));
    // remember that sndbuffers_t are interleaved

    // puts contents into buffer
    sf_count_t frames_read = sf_read_short(infile, buffer, sfinfo.frames);

    return buffer;
}
