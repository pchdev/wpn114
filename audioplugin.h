#ifndef AUDIOPLUGIN_H
#define AUDIOPLUGIN_H

#include <QObject>
#include "audiobackend.h"
#include "aeffect.h"
#include "aeffectx.h"
#include <QMacNativeWidget>
#include <QMacCocoaViewContainer>

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
    virtual void                        set_parameter_value(const uint16_t index, float value) pure; \
    virtual void                        set_program(const uint16_t index) pure ; \
    virtual void                        set_program_name(const std::string name) pure; \
    virtual void                        process_midi(const uint8_t data[4]) pure; \
    virtual void                        process_audio(float** inputs, float** outputs, const uint16_t nsamples) pure; \
    virtual void                        process_audio(float** outputs, const uint16_t nsamples) pure; \
    virtual void                        open_editor(void* view) pure; \
    virtual version                     get_version() const pure; \
    virtual std::array<uint16_t,2>      get_editor_size() const pure;

class plugin_hdl
{
    AUDIO_PLUGIN_INTERFACE( =0 )
    protected:
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
        AEffect* m_aeffect;
};

// QT INSTANCE --------------------------------------------------------------------

class AudioPlugin : public AudioEffectObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES    ( QQmlParserStatus )
    Q_PROPERTY      ( QString path READ path WRITE setPath NOTIFY pathChanged )
    Q_PROPERTY      ( int program READ program WRITE setProgram NOTIFY programChanged )
    Q_PROPERTY      ( QQmlListProperty<QString> programs READ programs )
    Q_PROPERTY      ( QQmlListProperty<QString> parameters READ parameters )

public:
    AudioPlugin();
    ~AudioPlugin();

    virtual float** process(const quint16 nsamples);

    virtual void classBegin();
    virtual void componentComplete();

    quint16 program() const;
    QString path() const;
    QQmlListProperty<QString> programs();
    QQmlListProperty<QString> parameters();

    void setPath(const QString);
    void setProgram(const quint16);

    Q_INVOKABLE void showEditorWindow();

    Q_INVOKABLE void set(int index, float value);
    Q_INVOKABLE float get(int index) const;
    Q_INVOKABLE void save(QString name);

    Q_INVOKABLE void noteOn(int channel, int index, int value);
    Q_INVOKABLE void noteOff(int channel, int index, int value);
    Q_INVOKABLE void control(int channel, int index, int value);
    Q_INVOKABLE void program(int channel, int value);
    Q_INVOKABLE void bend(int channel, int value);
    Q_INVOKABLE void aftertouch(int channel, int value);
    Q_INVOKABLE void aftertouch(int channel, int index, int value);
    Q_INVOKABLE void sysex(QVariantList bytes);

signals:
    void pathChanged();
    void pluginLoaded();
    void programChanged();

private:
    quint16         m_program;
    QString         m_path;
    plugin_hdl*     m_plugin_hdl;
    QList<QString*>  m_programs;
    QList<QString*>  m_parameters;

#ifdef __APPLE__
    QMacNativeWidget*           m_view;
    QMacCocoaViewContainer*     m_view_container;
#endif

};

#endif // AUDIOPLUGIN_H
