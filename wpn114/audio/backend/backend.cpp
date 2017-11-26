#include <wpn114/audio/backend/backend.hpp>
#include <iostream>

using namespace wpn114::audio;

static int main_stream_callback
(const void *input_buffer, void *output_buffer, unsigned long nsamples,
 const PaStreamCallbackTimeInfo *time_info, PaStreamCallbackFlags status_flags, void *user_data)
{
    float*          out     = (float*) output_buffer;
    backend_hdl*    backend = (backend_hdl*) user_data;

    (void) time_info;
    (void) status_flags;
    (void) input_buffer;

    // copy unit's configuration
    std::vector<unit_base*> units = backend->get_registered_units();
    uint8_t nchannels = backend->get_num_channels();
    auto master = *backend->get_master_output_buffer();

    // call audio processing on each of the registered units
    for (auto& unit : units)
    if  (unit->is_active())
         unit->process_audio(nsamples);

    for(uint16_t i = 0; i < nsamples; ++i)
    {
        for (uint8_t n = 0; n < nchannels; ++n)
        {
            // initialize sample data for each channel
            master[n][i] = 0.f;

            //      poll registered units
            for     (auto& unit : units)
            if      (unit->is_active() && n <= unit->get_num_channels()-1)
                    master[n][i] += unit->get_framedata(n,i) * unit->get_level();

            // output sample data for channel n
            *out++ = master[n][i];
        }
    }

    return paContinue;
}

backend_hdl::backend_hdl(uint8_t nchannels) :
    m_main_stream(nullptr),
    m_num_channels(nchannels)
{

}

backend_hdl::~backend_hdl()
{
    PaError err = Pa_StopStream(m_main_stream);
            err = Pa_CloseStream(m_main_stream);

    Pa_Terminate();
}

inline std::vector<unit_base*> backend_hdl::get_registered_units() const
{
    return m_units;
}

inline uint8_t backend_hdl::get_num_channels() const
{
    return m_num_channels;
}

void backend_hdl::register_unit(unit_base* unit)
{
    m_units.push_back(unit);
}

void backend_hdl::unregister_unit(unit_base* unit)
{
    m_units.erase(
                std::remove(
                    m_units.begin(), m_units.end(), unit),
                m_units.end());
}

void backend_hdl::initialize(size_t sample_rate, uint16_t nsamples)
{
    PaError err;

    // dont't forget error management
    err = Pa_Initialize();

    m_output_parameters.device                      = Pa_GetDefaultOutputDevice();
    m_output_parameters.channelCount                = m_num_channels;
    m_output_parameters.sampleFormat                = paFloat32;
    m_output_parameters.hostApiSpecificStreamInfo   = NULL;
    m_main_stream_cb_funcptr                        = &main_stream_callback;
    m_output_parameters.suggestedLatency            = Pa_GetDeviceInfo
                                                      (m_output_parameters.device)->defaultLowOutputLatency;

    auto device_name = Pa_GetDeviceInfo(m_output_parameters.device)->name;
    std::cout << device_name << std::endl;
    bufalloc(nsamples);

    for(auto& unit : m_units)
    {
        // initialize registered units
        unit->bufalloc(nsamples);
        unit->preprocessing(sample_rate, nsamples);
    }
}

void backend_hdl::bufalloc(uint16_t nsamples)
{
    m_master_output = new float*[m_num_channels];

    for (int i = 0; i < m_num_channels; ++i)
    {
        m_master_output[i] = new float[nsamples];
        memset(m_master_output[i], 0.f, sizeof(nsamples));
    }
}

inline float*** backend_hdl::get_master_output_buffer()
{
    return &m_master_output;
}

void backend_hdl::start_stream(size_t sample_rate, uint16_t nsamples)
{
    PaError err = Pa_OpenStream(
                        &m_main_stream,
                        NULL,
                        &m_output_parameters,
                        sample_rate,
                        nsamples,
                        paClipOff,
                        main_stream_callback,
                        this );

    if( err != paNoError )
        std::cerr << "error: " << Pa_GetErrorText(err) << std::endl;

    err = Pa_StartStream(m_main_stream);
}

void backend_hdl::stop_stream()
{
    PaError err = Pa_StopStream(m_main_stream);
    if ( err != paNoError )
        std::cerr << "error when stopping stream: " << Pa_GetErrorText(err) << std::endl;
}
