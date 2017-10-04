#include <iostream>
#include <portaudio.h>
#include <sndfile.h>

using namespace std;

// a vst host for kaivo
// custom granular / multifunction sampler
// libossia for communication
// ableton push support?

// dependencies:
// - vst3sdk
// - libossia
// - portaudio
// - libsndfile

#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 256

typedef short*  sndbuf_16;
typedef int*    sndbuf_32;

template<typename T> T load_soundfile()
{
    SNDFILE* infile;
    SF_INFO sfinfo;
    int readcount;
    const char* infilename = "input.wav";

    memset(&sfinfo, 0, sizeof(sfinfo));

    if(!(infile = sf_open (infilename, SFM_READ, &sfinfo)))
    {
        // throw error
    }

    // load file contents into buffer
    T buffer = nullptr;
    buffer = (T*) malloc(sfinfo.frames * sfinfo.channels * sizeof(T));

    // puts contents into buffer
    sf_count_t frames_read = sf_read_short(infile, buffer, sfinfo.frames);

    return buffer;
}

void load_vst()
{

}

void write_main_stream(sndbuf_16 buffer)
{

}

// units will then write on a main portaudio stream with blocking API
int main()
{

    PaStreamParameters output_parameters;
    PaStream* stream;
    PaError err;

    // dont't forget error management
    err = Pa_Initialize();

    output_parameters.device = Pa_GetDefaultOutputDevice();
    output_parameters.channelCount = 2;
    output_parameters.sampleFormat = paFloat32;
    output_parameters.suggestedLatency = Pa_GetDeviceInfo(output_parameters.device)->defaultLowOutputLatency;
    output_parameters.hostApiSpecificStreamInfo = NULL;

    err = Pa_OpenStream(&stream,
                        NULL,
                        &output_parameters,
                        SAMPLE_RATE,
                        FRAMES_PER_BUFFER,
                        paClipOff,
                        NULL,
                        NULL );

    err = Pa_WriteStream(stream, buffer, FRAMES_PER_BUFFER);
    err = Pa_StopStream(stream);

    err = Pa_CloseStream(stream);
    Pa_Terminate();

    return 0;
}
