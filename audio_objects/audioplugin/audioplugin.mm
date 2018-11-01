#include "audioplugin.hpp"
#include <QtDebug>
#include <QFile>
#include <QDataStream>

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif

#include <array>
#include <QMacCocoaViewContainer>

#define PARAMNAME_MAXLE     256
#define PROGRAMNAME_MAXLE   256

// QT INSTANCE --------------------------------------------------------------------

AudioPlugin::AudioPlugin() : m_path(""), m_view(nullptr), m_plugin_hdl(nullptr) {}
AudioPlugin::~AudioPlugin()
{    
    delete m_view;
    delete m_view_container;
    delete m_plugin_hdl;
}

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
        auto str = QString(m_plugin_hdl->get_parameter_name(i).c_str());
        m_parameters << str;
    }

    for ( int i = 0; i < m_plugin_hdl->get_nprograms(); ++i )
    {
        auto str = QString(m_plugin_hdl->get_program_name(i).c_str());
        m_programs << str;
    }

    SETN_IN    ( m_plugin_hdl->get_ninputs()  );
    SETN_OUT   ( m_plugin_hdl->get_noutputs() );

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

void AudioPlugin::initialize(qint64 nsamples)
{
    m_plugin_hdl->configure(m_stream_properties.sample_rate, m_stream_properties.block_size);
}

float** AudioPlugin::process(float**input, qint64 nsamples)
{                
    if  ( m_num_inputs )
        m_plugin_hdl->process_audio(input, m_out, nsamples);

    else    m_plugin_hdl->process_audio(m_out, nsamples);
    return  m_out;
}

void AudioPlugin::expose(WPNNode* root)
{
    // TODO: expose parameters and programs
    auto funcs = m_exp_node->createSubnode("functions");
    auto show = funcs->createSubnode("show");

    show->setType(Type::Impulse);
    QObject::connect(show, SIGNAL(valueReceived(QVariant)), this, SLOT(showEditorWindow()));
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

QStringList AudioPlugin::programs()
{
    return m_programs;
}

QStringList AudioPlugin::parameters()
{
    return m_parameters;
}

float AudioPlugin::get(QString name) const
{
    return m_plugin_hdl->get_parameter_value(m_parameters.indexOf(name));
}

float AudioPlugin::get(int index) const
{
    return m_plugin_hdl->get_parameter_value(index);
}

void AudioPlugin::set(QString name, float value)
{
    m_plugin_hdl->set_parameter_value(m_parameters.indexOf(name), value);
}

void AudioPlugin::set(int index, float value)
{
    m_plugin_hdl->set_parameter_value(index, value);
}

void AudioPlugin::save(QString name)
{
    m_plugin_hdl->set_program_name(name.toStdString());
}

void AudioPlugin::setChunk(QString name)
{
    m_chunk = name;
}

void AudioPlugin::saveChunk(QString name)
{
    // write to file
    QFile out(name);
    out.open(QIODevice::WriteOnly | QIODevice::Truncate);

    QDataStream stream(&out);
    auto chunk = m_plugin_hdl->get_chunk();
    stream << chunk;

    out.close();
}

void AudioPlugin::loadChunk(QString name)
{
    // read from file
    QFile in(name);
    in.open(QIODevice::ReadOnly);

    QByteArray chunk = in.readAll();
    m_plugin_hdl->set_chunk(chunk);

    in.close();
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
//    if ( !m_active ) m_plugin_hdl->process_midi_offline();
}

void AudioPlugin::programChange(int channel, int value)
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

void AudioPlugin::allNotesOff()
{
    for ( quint8 i = 0; i < 128; ++i )
        noteOff(0, i, 127);
}

// HDLS ----------------------------------------------------------------------------------------

#define GET_2X_PNAME_STR(command) \
    char name [ PARAMNAME_MAXLE ]; \
    m_aeffect->dispatcher(m_aeffect, command, index, 0, &name, 0); \
    return (std::string) name;

vst2x_plugin::vst2x_plugin(std::string path) : m_block_pos(0), m_event_queue(new VstEvents)
{

#ifdef __APPLE__ //-----------------------------------------------------------------------------

    CFStringRef fns     = CFStringCreateWithCString
                        ( NULL, path.c_str(), kCFStringEncodingUTF8 );
    if ( !fns  )        return;

    CFURLRef url        = CFURLCreateWithFileSystemPath
                        ( NULL, fns, kCFURLPOSIXPathStyle, false );
    if ( !url  )        return;

    m_module            = (void*) CFBundleCreate
                        ( NULL, url );
    CFRelease           ( url );

    if      ( m_module && CFBundleLoadExecutable((CFBundleRef) m_module) == false )
    return  ;

    PluginEntryProc main_proc   = 0;
    main_proc                   = (PluginEntryProc) CFBundleGetFunctionPointerForName
                                ((CFBundleRef) m_module, CFSTR("VSTPluginMain"));
    if  (!main_proc)
        main_proc               = (PluginEntryProc) CFBundleGetFunctionPointerForName
                                ((CFBundleRef) m_module, CFSTR("main_macho"));

#endif //------------------------------------------------------------------------------------

    m_aeffect  = main_proc(HostCallback);
    m_aeffect -> dispatcher(m_aeffect, effOpen, 0, 0, nullptr, 0.f);
}

vst2x_plugin::~vst2x_plugin()
{
    m_aeffect->dispatcher(m_aeffect, effStopProcess, 0, 0, nullptr, 0.f);
    m_aeffect->dispatcher(m_aeffect, effMainsChanged, 0, 0, nullptr, 0.f);
    m_aeffect->dispatcher(m_aeffect, effClose, 0, 0, nullptr, 0.f);

#ifdef __APPLE__ // ---------------------------------------------------------------
    CFBundleUnloadExecutable( (CFBundleRef) m_aeffect);
    CFRelease( (CFBundleRef) m_module );
#endif //--------------------------------------------------------------------------
    delete m_event_queue;
}

void vst2x_plugin::configure(const uint32_t srate, const uint16_t bsize)
{
    m_aeffect->dispatcher(m_aeffect, effSetSampleRate, 0, 0, nullptr, srate);
    m_aeffect->dispatcher(m_aeffect, effSetBlockSize, 0, bsize, nullptr, 0.f);
    m_aeffect->dispatcher(m_aeffect, effMainsChanged, 0, 1, nullptr, 0.f);
    m_aeffect->dispatcher(m_aeffect, effStartProcess, 0, 0, nullptr, 0.0);
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

void vst2x_plugin::set_parameter_value(const uint16_t index, float value)
{
    m_aeffect->setParameter(m_aeffect, index, value);
}

void vst2x_plugin::set_program(const uint16_t index)
{
    m_aeffect->dispatcher(m_aeffect, effSetProgram, index, 0, nullptr, 0);
}

void vst2x_plugin::set_program_name(const std::string name)
{
    char* cname = const_cast<char*>(name.c_str());
    m_aeffect->dispatcher(m_aeffect, effSetProgramName, 0, 0, cname, 0);
}

void vst2x_plugin::process_audio(float** inputs, float **outputs, const uint16_t nsamples)
{
    if ( m_event_queue->numEvents )
        m_aeffect->dispatcher(m_aeffect, effProcessEvents, 0, 0, m_event_queue, 0.f);

    m_event_queue->numEvents = 0;
    m_aeffect->processReplacing(m_aeffect, inputs, outputs, nsamples);
}

void vst2x_plugin::process_audio(float **&outputs, const uint16_t nsamples)
{
    if ( m_event_queue->numEvents )
        m_aeffect->dispatcher(m_aeffect, effProcessEvents, 0, 0, m_event_queue, 0.f);

    m_event_queue->numEvents = 0;
    m_aeffect->processReplacing(m_aeffect, nullptr, outputs, nsamples);
}

void vst2x_plugin::process_midi_offline()
{
    if ( m_event_queue->numEvents )
         m_aeffect->dispatcher(m_aeffect, effProcessEvents, 0, 0, m_event_queue, 0.f);
}

void vst2x_plugin::process_midi(const uint8_t data[4])
{
    auto midiev = new VstMidiEvent;
    std::memset ( midiev, 0, sizeof(VstMidiEvent) );
    std::copy_n ( data, 4, midiev->midiData );

    midiev->deltaFrames = 0;
    midiev->flags       = kVstMidiEventIsRealtime;
    midiev->byteSize    = sizeof(VstMidiEvent);
    midiev->type        = kVstMidiType;

    auto nevents = m_event_queue->numEvents;
    m_event_queue->events[nevents] = reinterpret_cast<VstEvent*>(midiev);
    m_event_queue->numEvents++;
}

void vst2x_plugin::open_editor(void* view)
{
    m_aeffect->dispatcher(m_aeffect, effEditOpen, 0, 0, view, 0);
}

version vst2x_plugin::get_version() const
{
    return version::VST2X;
}

std::array<uint16_t, 2> vst2x_plugin::get_editor_size() const
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

QByteArray vst2x_plugin::get_chunk()
{
    QByteArray chunk;
    m_aeffect->dispatcher(m_aeffect, effGetChunk, 0, 0, chunk.data(), 0.0);

    return chunk;
}

void vst2x_plugin::set_chunk(QByteArray chunk)
{
    m_aeffect->dispatcher(m_aeffect, effSetChunk, 0, chunk.size(), chunk.data(), 0);
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
    {
        VstTimeInfo* info = new VstTimeInfo;
        std::memset(info, 0, sizeof(VstTimeInfo));

        if ( value & kVstTempoValid )
        {
          info->tempo = 120;
          info->flags |= kVstTempoValid;
        }

        result = reinterpret_cast<VstIntPtr>(info);
        break;
    }
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
        result = kVstProcessLevelUser;
        break;
    case audioMasterGetAutomationState:
        break;
    case audioMasterGetVendorString:
        break;
    case audioMasterGetProductString:
         std::copy_n("wpn214", 7, static_cast<char*>(ptr));
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

vst3x_plugin::~vst3x_plugin()
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

void vst3x_plugin::process_audio(float **&outputs, const uint16_t nsamples)
{

}

void vst3x_plugin::process_midi(const uint8_t data[])
{

}

void vst3x_plugin::process_midi_offline()
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

QByteArray vst3x_plugin::get_chunk()
{
    QByteArray chunk;
    return chunk;
}

void vst3x_plugin::set_chunk(QByteArray chunk)
{

}

