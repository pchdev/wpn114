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
#include <cstring>
#include <wpn114/audio/backend/sndfile_support.hpp>

template<typename T> T load_soundfile()
{
    SNDFILE*    infile;
    SF_INFO     sfinfo;
    int         readcount;
    const char* infilename = "input.wav";

    std::memset(&sfinfo, 0, sizeof(sfinfo));

    if(!(infile = sf_open (infilename, SFM_READ, &sfinfo)))
    {
        // throw error
    }

    // load file contents into buffer
    T buffer    = nullptr;
    buffer      = (T*) malloc(sfinfo.frames * sfinfo.channels * sizeof(T));

    // puts contents into buffer
    sf_count_t frames_read = sf_read_short(infile, buffer, sfinfo.frames);

    return buffer;
}
