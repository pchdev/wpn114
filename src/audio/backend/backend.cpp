#pragma once

#include <portaudio.h>
#define NULL 0
#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 256

namespace wpn114
{
namespace audio
{
namespace backend
{
static void initialize_audio()
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

     //err = Pa_WriteStream(stream, buffer, FRAMES_PER_BUFFER);
     err = Pa_StopStream(stream);

     err = Pa_CloseStream(stream);
     Pa_Terminate();
}
}
}
}
