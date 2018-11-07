#ifndef AUDIOPLUGIN_H
#define AUDIOPLUGIN_H

#include <QObject>
#include <src/audio/audio.hpp>
#include "aeffect.h"
#include "aeffectx.h"
#include <QMacNativeWidget>
#include <QMacCocoaViewContainer>
#include <QQueue>
#include <QMutex>
#include <QMutexLocker>

static VstIntPtr VSTCALLBACK HostCallback
(AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt);
typedef AEffect* (*PluginEntryProc) (audioMasterCallback audioMaster);

enum class version
{
    AU          = 1,
    VST2X       = 2,
    VST3X       = 3
};

enum class MIDI
{
    SYSEX                   = 0xf0,
    EOX                     = 0xf7,
    NOTE_OFF                = 0x80,
    NOTE_ON                 = 0x90,
    AFTERTOUCH              = 0xa0,
    CONTINUOUS_CONTROL      = 0xb0,
    PATCH_CHANGE            = 0xc0,
    CHANNEL_PRESSURE        = 0xd0,
    PITCH_BEND              = 0xe0
};

#define AUDIO_PLUGIN_INTERFACE(pure) \
    public: \
    virtual void                        configure(const uint32_t srate, const uint16_t bsize) pure ;\
    virtual uint16_t                    get_nparameters() const pure; \
    virtual uint16_t                    get_nprograms() const pure; \
    virtual uint16_t                    get_ninputs() const pure; \
    virtual uint16_t                    get_noutputs() const pure; \
    virtual std::string                 get_parameter_name(uint16_t index) const pure; \
    virtual std::string                 get_program_name(uint16_t index) const pure; \
    virtual float                       get_parameter_value(const uint16_t index) const pure ; \
    virtual void                        set_parameter_value(const uint16_t index, float value, bool normalized) pure; \
    virtual void                        set_program(const uint16_t index) pure ; \
    virtual void                        set_program_name(const std::string name) pure; \
    virtual QByteArray                  get_chunk() pure; \
    virtual void                        set_chunk(QByteArray) pure; \
    virtual void                        process_midi_offline() pure; \
    virtual void                        process_midi(const uint8_t data[4]) pure; \
    virtual void                        process_audio(float** inputs, float** outputs, const uint16_t nsamples) pure; \
    virtual void                        process_audio(float**& outputs, const uint16_t nsamples) pure; \
    virtual void                        open_editor(void* view) pure; \
    virtual version                     get_version() const pure; \
    virtual std::array<uint16_t,2>      get_editor_size() const pure;

class plugin_hdl
{
    AUDIO_PLUGIN_INTERFACE( =0 )
    virtual ~plugin_hdl() {}

    void set_world(WorldStream* world) { m_world = world; }

    protected:
    WorldStream* m_world = nullptr;
    qint64 m_clock = 0;
    std::string m_path;
};

class vst3x_plugin : public plugin_hdl
{
    AUDIO_PLUGIN_INTERFACE ( )
    vst3x_plugin( const std::string path );
    vst3x_plugin( const vst3x_plugin& ) = delete;
    vst3x_plugin& operator= ( const vst3x_plugin& ) = delete;
    vst3x_plugin( vst3x_plugin&& ) = delete;
    ~vst3x_plugin();
};

class vst2x_plugin : public plugin_hdl
{
    AUDIO_PLUGIN_INTERFACE ( )
    vst2x_plugin( const std::string path );
    vst2x_plugin( const vst2x_plugin& ) = delete;
    vst2x_plugin& operator= ( const vst2x_plugin& ) = delete;
    vst2x_plugin( vst2x_plugin&& ) = delete;
    ~vst2x_plugin();

    private:
    void set_delta(quint16 clock);

    void* m_module = nullptr;
    QMutex m_delta_mtx;
    qint64 m_delta_frames = 0;
    VstEvents* m_event_queue;
    AEffect* m_aeffect;
};

// QT INSTANCE --------------------------------------------------------------------

class AudioPlugin : public StreamNode
{
    Q_OBJECT
    Q_INTERFACES    ( QQmlParserStatus )
    Q_PROPERTY      ( QString path READ path WRITE setPath NOTIFY pathChanged )
    Q_PROPERTY      ( int program READ program WRITE setProgram NOTIFY programChanged )
    Q_PROPERTY      ( QStringList programs READ programs )
    Q_PROPERTY      ( QStringList parameters READ parameters )
    Q_PROPERTY      ( QString chunk READ chunk WRITE setChunk )
    Q_PROPERTY      ( WorldStream* world READ world WRITE setWorld )

public:
    AudioPlugin();
    ~AudioPlugin() override;

    virtual float** process ( float**, qint64 ) override;
    virtual void initialize ( qint64 ) override;

    virtual void componentComplete() override;
    virtual void expose(WPNNode*) override;

    WorldStream* world      ( ) const { return m_world_stream; }
    QString chunk           ( ) { return m_chunk; }
    quint16 program         ( ) const;
    QString path            ( ) const;
    QStringList programs    ( );
    QStringList parameters  ( );

    void setWorld       ( WorldStream* world ) { m_world_stream = world; }
    void setChunk       ( QString name  );
    void setPath        ( const QString );
    void setProgram     ( const quint16 );

    public slots:

    Q_INVOKABLE void showEditorWindow();

    Q_INVOKABLE void set(QString name, float value, bool normalized = false );
    Q_INVOKABLE void set(int index, float value);

    Q_INVOKABLE float get(QString name) const;
    Q_INVOKABLE float get(int index) const;
    Q_INVOKABLE void save(QString name);

    Q_INVOKABLE void loadChunk(QString name);
    Q_INVOKABLE void saveChunk(QString name);

    Q_INVOKABLE void noteOn(int channel, int index, int value);
    Q_INVOKABLE void noteOff(int channel, int index, int value);
    Q_INVOKABLE void control(int channel, int index, int value);
    Q_INVOKABLE void programChange(int channel, int value);
    Q_INVOKABLE void bend(int channel, int value);
    Q_INVOKABLE void aftertouch(int channel, int value);
    Q_INVOKABLE void aftertouch(int channel, int index, int value);
    Q_INVOKABLE void sysex(QVariantList bytes);   

    Q_INVOKABLE void allNotesOff();

signals:
    void pathChanged();
    void pluginLoaded();
    void programChanged();
    void parameterChanged();

private:
    void push( MIDI command, int channel, int value );
    void push( MIDI command, int channel, int index, int value );

    quint16             m_program;
    QString             m_path;
    plugin_hdl*         m_plugin_hdl;
    QStringList         m_programs;
    QStringList         m_parameters;
    QString             m_chunk;
    WorldStream*        m_world_stream;

#ifdef __APPLE__
    QMacNativeWidget*           m_view;
    QMacCocoaViewContainer*     m_view_container;
#endif

};

#endif // AUDIOPLUGIN_H
