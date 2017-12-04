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
    auto units          = backend->units();
    auto master         = *backend->out();
    uint8_t nchannels   = backend->nchannels();

    // call audio processing on each of the registered units
    for (auto& unit : units)
    if  (unit->active())
         unit->process(nsamples);

    for(uint16_t i = 0; i < nsamples; ++i)
    {
        for (uint8_t n = 0; n < nchannels; ++n)
        {
            // initialize sample data for each channel
            master[n][i] = 0.f;

            //      poll registered units
            for     (auto& unit : units)
            if      (unit->active() && n <= unit->nchannels()-1)
                    master[n][i] += unit->framedata(n,i) * unit->level();

            // output sample data for channel n
            *out++ = master[n][i];
        }
    }

    return paContinue;
}

backend_hdl::backend_hdl(uint8_t nchannels) :
    m_stream(nullptr),
    m_nchannels(nchannels) {}

backend_hdl::~backend_hdl()
{
    PaError     err = Pa_StopStream(m_stream);
                err = Pa_CloseStream(m_stream);

    Pa_Terminate();
}

inline std::vector<unit_base*> backend_hdl::units() const
{
    return m_units;
}

inline uint8_t backend_hdl::nchannels() const
{
    return m_nchannels;
}

void backend_hdl::register_unit(unit_base& unit)
{
    m_units.push_back(&unit);
}

void backend_hdl::unregister_unit(unit_base& unit)
{
    m_units.erase(
                std::remove(
                    m_units.begin(), m_units.end(), &unit),
                m_units.end());
}

void backend_hdl::initialize(size_t srate, uint16_t nsamples)
{
    PaError err;

    // dont't forget error management
    err = Pa_Initialize();

    m_outparameters.device                      = Pa_GetDefaultOutputDevice();
    m_outparameters.channelCount                = m_nchannels;
    m_outparameters.sampleFormat                = paFloat32;
    m_outparameters.hostApiSpecificStreamInfo   = NULL;
    m_main_stream_cb_funcptr                    = &main_stream_callback;
    m_outparameters.suggestedLatency            = Pa_GetDeviceInfo
                                                  (m_outparameters.device)->defaultLowOutputLatency;

    auto device_name = Pa_GetDeviceInfo(m_outparameters.device)->name;
    std::cout << device_name << std::endl;

    bufalloc(nsamples);

    for(auto& unit : m_units)
    {
        // initialize registered units
        unit->bufalloc(nsamples);
        unit->preprocess(srate, nsamples);
    }
}

void backend_hdl::bufalloc(uint16_t nsamples)
{
    m_out = new float*[m_nchannels];

    for (int i = 0; i < m_nchannels; ++i)
    {
        m_out[i] = new float[nsamples];
        memset(m_out[i], 0.f, sizeof(nsamples));
    }
}

inline float*** backend_hdl::out()
{
    return &m_out;
}

void backend_hdl::start(size_t srate, uint16_t nsamples)
{
    PaError err = Pa_OpenStream(
                        &m_stream,
                        NULL,
                        &m_outparameters,
                        srate,
                        nsamples,
                        paClipOff,
                        main_stream_callback,
                        this );

    if( err != paNoError )
        std::cerr << "error: " << Pa_GetErrorText(err) << std::endl;

    err = Pa_StartStream(m_stream);
}

void backend_hdl::stop()
{
    PaError err = Pa_StopStream(m_stream);
    if ( err != paNoError )
        std::cerr << "error when stopping stream: " << Pa_GetErrorText(err) << std::endl;
}
