#include "audioplugin.h"

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif

#include <array>
#include <QMacCocoaViewContainer>

#define PARAMNAME_MAXLE     256
#define PROGRAMNAME_MAXLE   256

// QT INSTANCE --------------------------------------------------------------------

AudioPlugin::AudioPlugin() : m_path(""), m_view(0), m_plugin_hdl(0) {}
AudioPlugin::~AudioPlugin()
{
    delete m_view;
    delete m_view_container;
}

void AudioPlugin::classBegin() {}
void AudioPlugin::componentComplete()
{
    //          load plugin
    if          ( m_path.endsWith(".vst3"))
                m_plugin_hdl = new vst3x_plugin(m_path.toStdString());

    else if     ( m_path.endsWith(".vst"))
                m_plugin_hdl = new vst2x_plugin(m_path.toStdString());
    else        return;

    if          ( !m_plugin_hdl ) return;

    for ( int i = 0; i < m_plugin_hdl->get_nparameters(); ++i )
    {
        auto str = new QString(m_plugin_hdl->get_parameter_name(i).c_str());
        m_parameters << str;
    }

    for(int i = 0; i < m_plugin_hdl->get_nprograms(); ++i )
    {
        auto str = new QString(m_plugin_hdl->get_program_name(i).c_str());
        m_programs << str;
    }

    SET_OFFSET      ( 0 );
    SETN_IN         ( m_plugin_hdl->get_ninputs()  );
    SETN_OUT        ( m_plugin_hdl->get_noutputs() );

    INITIALIZE_AUDIO_IO;

#ifdef __APPLE__ //----------------------------------------------------------
    m_view              = new QMacNativeWidget();
    m_view_container    = new QMacCocoaViewContainer(m_view->nativeView());
#endif //--------------------------------------------------------------------

    auto size           = m_plugin_hdl->get_editor_size();    
    m_view              ->resize(size[0], size[1]);
    m_view              ->update();
    m_view_container    ->setFixedSize(size[0], size[1]);
    m_view_container    ->update();

    emit pluginLoaded();
}

void AudioPlugin::showEditorWindow()
{    
#ifdef __APPLE__ //----------------------------------------------------------
    m_plugin_hdl->open_editor((void*) m_view->nativeView());
#endif //--------------------------------------------------------------------

    m_view_container->show();
}

float** AudioPlugin::process(const quint16 nsamples)
{
    if  ( m_num_inputs )
    {
        ZEROBUF(IN, m_num_inputs);
        auto in = get_inputs( nsamples );

        m_plugin_hdl->process_audio(in, m_outputs, nsamples);
    }

    else    m_plugin_hdl->process_audio(m_outputs, nsamples);
    return  m_outputs;
}

QString AudioPlugin::path() const
{
    return m_path;
}

void AudioPlugin::setPath(const QString path)
{
    m_path = path;
}

quint16 AudioPlugin::program() const
{
    return m_program;
}

void AudioPlugin::setProgram(const quint16 program)
{
    m_program = program;
    m_plugin_hdl->set_program(program);
}

QQmlListProperty<QString> AudioPlugin::programs()
{
    return QQmlListProperty<QString>(this, m_programs);
}

QQmlListProperty<QString> AudioPlugin::parameters()
{
    return QQmlListProperty<QString>(this, m_parameters);
}

float AudioPlugin::get(int index) const
{
    return m_plugin_hdl->get_parameter_value(index);
}

void AudioPlugin::set(int index, float value)
{
    m_plugin_hdl->set_parameter_value(index, value);
}

void AudioPlugin::save(QString name)
{
    m_plugin_hdl->set_program_name(name.toStdString());
}

#define MIDI_AR2(cmd) \
    uint8_t command = static_cast<uint8_t>( cmd ); \
    uint8_t data[4] = { static_cast<uint8_t>(command + channel), (uint8_t) value, 0, 0 }; \
    m_plugin_hdl->process_midi(data);

#define MIDI_AR3(cmd) \
    uint8_t command = static_cast<uint8_t>( cmd ); \
    uint8_t data[4] = { static_cast<uint8_t>(command + channel), (uint8_t) index, (uint8_t) value, 0 }; \
    m_plugin_hdl->process_midi(data);


void AudioPlugin::noteOn(int channel, int index, int value)
{
    MIDI_AR3 ( MIDI::NOTE_ON );
}

void AudioPlugin::noteOff(int channel, int index, int value)
{
    MIDI_AR3 ( MIDI::NOTE_OFF );
}

void AudioPlugin::control(int channel, int index, int value)
{
    MIDI_AR3 ( MIDI::CONTINUOUS_CONTROL );
}

void AudioPlugin::program(int channel, int value)
{
    MIDI_AR2 ( MIDI::PATCH_CHANGE );
}

void AudioPlugin::bend(int channel, int value)
{
    MIDI_AR2 ( MIDI::PITCH_BEND );
}

void AudioPlugin::aftertouch(int channel, int index, int value)
{
    MIDI_AR3 ( MIDI::AFTERTOUCH );
}

void AudioPlugin::aftertouch(int channel, int value)
{
    MIDI_AR2 ( MIDI::CHANNEL_PRESSURE );
}

void AudioPlugin::sysex(QVariantList bytes)
{
    // l2d
}

// HDLS ----------------------------------------------------------------------------------------

#define GET_2X_PNAME_STR(command) \
    char name [ PARAMNAME_MAXLE ]; \
    m_aeffect->dispatcher(m_aeffect, command, index, reinterpret_cast<VstIntPtr>(&name), 0, 0); \
    return (std::string) name;

vst2x_plugin::vst2x_plugin(std::string path)
{

#ifdef __APPLE__ //-----------------------------------------------------------------------------

    CFStringRef fns     = CFStringCreateWithCString
                        (NULL, path.c_str(), kCFStringEncodingUTF8);
    if ( !fns  )        return;

    CFURLRef url        = CFURLCreateWithFileSystemPath
                        (NULL, fns, kCFURLPOSIXPathStyle, false);
    if ( !url  )        return;

    void* module        = (void*) CFBundleCreate
                        (NULL, url);
    CFRelease           ( url );

    if      ( module && CFBundleLoadExecutable((CFBundleRef) module) == false )
    return  ;

    PluginEntryProc main_proc   = 0;
    main_proc                   = (PluginEntryProc) CFBundleGetFunctionPointerForName
                                ((CFBundleRef) module, CFSTR("VSTPluginMain"));
    if  (!main_proc)
        main_proc               = (PluginEntryProc) CFBundleGetFunctionPointerForName
                                ((CFBundleRef) module, CFSTR("main_macho"));

#endif //------------------------------------------------------------------------------------

    m_aeffect  = main_proc(HostCallback);
    m_aeffect -> dispatcher(m_aeffect, effOpen, 0, 0, nullptr, 0.f);
}

vst2x_plugin::~vst2x_plugin()
{
    m_aeffect->dispatcher(m_aeffect, effClose, 0, 0, nullptr, 0.f);

#ifdef __APPLE__ // ---------------------------------------------------------------
    CFBundleUnloadExecutable( (CFBundleRef) m_aeffect);
    CFRelease( (CFBundleRef) m_aeffect);
#endif //--------------------------------------------------------------------------

    delete m_aeffect;
}

void vst2x_plugin::configure(const uint32_t srate, const uint16_t bsize)
{
    m_aeffect->dispatcher(m_aeffect, effSetSampleRate, 0, 0, nullptr, srate);
    m_aeffect->dispatcher(m_aeffect, effSetBlockSize, 0, 0, nullptr, bsize);
    m_aeffect->dispatcher(m_aeffect, effMainsChanged, 0, 1, nullptr, 0.f);
}

uint16_t vst2x_plugin::get_nparameters() const
{
    return m_aeffect->numParams;
}

uint16_t vst2x_plugin::get_nprograms() const
{
    return m_aeffect->numPrograms;
}

uint16_t vst2x_plugin::get_ninputs() const
{
    return m_aeffect->numInputs;
}

uint16_t vst2x_plugin::get_noutputs() const
{
    return m_aeffect->numOutputs;
}

std::string vst2x_plugin::get_parameter_name(uint16_t index) const
{
    GET_2X_PNAME_STR(effGetParamName);
}

std::string vst2x_plugin::get_program_name(uint16_t index) const
{
    GET_2X_PNAME_STR(effGetProgramName);
}

float vst2x_plugin::get_parameter_value(const uint16_t index) const
{
    return m_aeffect->getParameter(m_aeffect, index);
}

void vst2x_plugin::set_parameter_value(const uint16_t index, const float value)
{
    m_aeffect->setParameter(m_aeffect, index, value);
}

void vst2x_plugin::set_program(const uint16_t index)
{
    m_aeffect->dispatcher(m_aeffect, effSetProgram, index, 0, 0, 0);
}

void vst2x_plugin::set_program_name(const std::string name)
{
    m_aeffect->dispatcher(m_aeffect, effSetProgramName, 0,
                          reinterpret_cast<VstIntPtr>(name.c_str()), 0, 0);
}

void vst2x_plugin::process_audio(float **inputs, float **outputs, const uint16_t nsamples)
{
    m_aeffect->processReplacing(m_aeffect, inputs, outputs, nsamples);
}

void vst2x_plugin::process_audio(float **outputs, const uint16_t nsamples)
{
    m_aeffect->processReplacing(m_aeffect, nullptr, outputs, nsamples);
}

void vst2x_plugin::process_midi(const uint8_t data[4])
{
    auto events         = new VstEvents();
    events->events[0]   = new VstEvent();
    events->numEvents   = 1;

    auto midiev         = (VstMidiEvent*) events->events[0];

    for ( int i = 0; i < 4; ++i )
        midiev->midiData[i] = data[i];

    midiev->flags       = kVstMidiEventIsRealtime;
    midiev->byteSize    = sizeof(VstMidiEvent);
    midiev->type        = kVstMidiType;

    m_aeffect->dispatcher(m_aeffect, effProcessEvents, 0, 0, events, 0.f);
}

void vst2x_plugin::open_editor(void* view)
{
    m_aeffect->dispatcher(m_aeffect, effEditOpen, 0, 0, view, 0);
}

version vst2x_plugin::get_version() const
{
    return version::VST2X;
}

std::array<uint16_t, 2>  vst2x_plugin::get_editor_size() const
{
    std::array<uint16_t, 2> res = { 0, 0 };
    ERect* rect = 0;

    m_aeffect->dispatcher(m_aeffect, effEditGetRect, 0, 0, &rect, 0.f);

    if ( rect )
    {
        res[0]   = rect->right - rect->left;
        res[1]   = rect->bottom - rect->top;
    }

    return res;
}

VstIntPtr VSTCALLBACK HostCallback
(AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void *ptr, float opt)
{
    VstIntPtr result = 0;

    switch(opcode)
    {
    case audioMasterAutomate: break;
    case audioMasterVersion:
        result = 2400;
        break;
    case audioMasterCurrentId:
        break;
    case audioMasterIdle:
        result = 1;
        break;
    case audioMasterGetTime:
        break;
    case audioMasterProcessEvents:
        break;
    case audioMasterIOChanged:
        break;
    case audioMasterGetSampleRate:
        break;
    case audioMasterGetBlockSize:
        break;
    case audioMasterGetInputLatency:
        break;
    case audioMasterGetOutputLatency:
        break;
    case audioMasterGetCurrentProcessLevel:
        break;
    case audioMasterGetAutomationState:
        break;
    case audioMasterGetVendorString:
        break;
    case audioMasterGetProductString:
        break;
    case audioMasterGetVendorVersion:
        break;
    case audioMasterCanDo:
        break;
    case audioMasterGetLanguage:
        break;
    case audioMasterUpdateDisplay:
        break;
    default:
        break;
    }

    return result;
}

// ----------------------------------------------------------------------------------------------
// VST3x
// ----------------------------------------------------------------------------------------------

vst3x_plugin::vst3x_plugin(const std::string path)
{

}

void vst3x_plugin::configure(const uint32_t srate, const uint16_t bsize)
{

}

uint16_t vst3x_plugin::get_nparameters() const
{
    return 0;
}

uint16_t vst3x_plugin::get_nprograms() const
{
    return 0;
}

uint16_t vst3x_plugin::get_ninputs() const
{
    return 0;
}

uint16_t vst3x_plugin::get_noutputs() const
{
    return 0;
}

std::string vst3x_plugin::get_parameter_name(uint16_t index) const
{
    return "";
}

std::string vst3x_plugin::get_program_name(uint16_t index) const
{
    return "";
}

float vst3x_plugin::get_parameter_value(const uint16_t index) const
{
    return 0.f;
}

void vst3x_plugin::set_parameter_value(const uint16_t index, float value)
{

}

void vst3x_plugin::set_program(const uint16_t index)
{

}

void vst3x_plugin::set_program_name(const std::string name)
{

}

void vst3x_plugin::process_audio(float **inputs, float **outputs, const uint16_t nsamples)
{

}

void vst3x_plugin::process_audio(float **outputs, const uint16_t nsamples)
{

}

void vst3x_plugin::process_midi(const uint8_t data[])
{

}

std::array<uint16_t, 2> vst3x_plugin::get_editor_size() const
{
    std::array<uint16_t, 2> res = {0, 0};
    return res;
}

void vst3x_plugin::open_editor(void* view)
{

}

version vst3x_plugin::get_version() const
{
    return version::VST3X;
}

