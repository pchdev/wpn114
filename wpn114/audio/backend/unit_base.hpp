#pragma once
#include <cstdint>
#include <memory>
//-------------------------------------------------------------------------------------------------------
#ifdef WPN_AUDIO_SNDFILE
#include <wpn114/audio/backend/sndfile_support.hpp>
#endif
//-------------------------------------------------------------------------------------------------------
#ifdef WPN_CONTROL_OSSIA
    #include <ossia/ossia.hpp>
#endif
//-------------------------------------------------------------------------------------------------------
namespace wpn114 {
namespace audio {
//-------------------------------------------------------------------------------------------------------
enum class unit_type
{
    ANALYSIS_UNIT   = 0,
    GENERATOR_UNIT  = 1,
    EFFECT_UNIT     = 2,
    HYBRID_UNIT     = 3
};

class aux_unit; // fwd
//-------------------------------------------------------------------------------------------------------
class unit_base
// the main base class for all units
//-------------------------------------------------------------------------------------------------------
{
public:
    virtual ~unit_base();

#ifdef WPN_CONTROL_OSSIA //--------------------------------------------------------------------------------------
    void net_expose(ossia::net::node_base& application_node, std::string name);
    void net_expose(ossia::net::node_base& application_node);
    virtual void net_expose_plugin_tree(ossia::net::node_base& plugin_root) = 0;
    // gets called whenever we want to expose the parameters' unit to the network
    void set_netname(std::string name);
    const std::string& get_netname() const;
#endif //------------------------------------------------------------------------------------------------
    virtual void process_audio(uint16_t nsamples) = 0;
    virtual void process_audio(float** input_buffer, uint16_t nsamples) = 0;
    // the unit's audio callbacks
    virtual void preprocessing(size_t sample_rate, uint16_t nsamples) = 0;
    // preparing the units user-defined audio processing
    void        bufalloc(uint16_t nsamples);
    //          pre-initializes output buffer, setting it to 0
    float       get_framedata(uint8_t channel, uint16_t frame) const;
    float**     get_output_buffer();

    uint8_t     get_num_channels() const;
    unit_type   get_unit_type() const;
    float       get_level() const;
    void        set_level(float level);

    void        activate();
    void        deactivate();
    bool        is_active() const;

#ifdef WPN_AUDIO_AUX //---------------------------------------------------------------------------------
    void        add_aux_send(aux_unit& aux); // register the unit to an aux bus
#endif //------------------------------------------------------------------------------------------------
protected:
#define OUT                 m_output_buffer
#define SET_UTYPE(u)        m_unit_type = u
#define SETN_IN(n)          m_num_inputs = n
#define SETN_OUT(n)         m_num_outputs = n
#define N_OUT               m_num_outputs
#define N_IN                m_num_inputs
//-------------------------------------------------------------------------------------------------------
    bool            m_active;
    float           m_level;
    uint8_t         m_num_inputs;
    uint8_t         m_num_outputs;
    float**         m_output_buffer;
    unit_type       m_unit_type;

#ifdef WPN_CONTROL_OSSIA // -------------------------------------------------------------------------------------
    std::string                 m_netname;
    ossia::net::node_base*      m_netnode;
#endif //------------------------------------------------------------------------------------------------

};
//-------------------------------------------------------------------------------------------------------
#ifdef WPN_AUDIO_SNDFILE
// manages audio soundfiles (mostly convenience methods)
//-------------------------------------------------------------------------------------------------------
class buffer_unit : public unit_base
{
public:
    ~buffer_unit();
protected:
#define SFBUF          m_sf_buffer
#define SFLOAD(p)      load_soundfile(m_sf_buffer, p)
#define CLEAR_SFBUF    delete m_sf_buffer.data
    sndbuf_t           m_sf_buffer;
};
#endif

//-------------------------------------------------------------------------------------------------------
#ifdef WPN_AUDIO_AUX
// manages audio aux-buses/tracks
//-------------------------------------------------------------------------------------------------------
struct aux_send
{
    unit_base*    m_sender;
    float         m_level;
};
//-------------------------------------------------------------------------------------------------------
class aux_unit : public unit_base
{
public:
#ifdef WPN_CONTROL_OSSIA //--------------------------------------------------------------------------------------
    void net_expose_plugin_tree(ossia::net::node_base& plugin_root) override;
#endif //------------------------------------------------------------------------------------------------

    aux_unit();
    ~aux_unit();
    aux_unit(std::unique_ptr<unit_base> receiver);
    void preprocessing(size_t sample_rate, uint16_t nsamples) override;
    void process_audio(uint16_t nsamples) override;
    void process_audio(float** input_buffer, uint16_t nsamples) override;
    float get_framedata(uint8_t channel, uint16_t frame) const;
    void set_receiver(std::unique_ptr<unit_base> receiver);
    void add_sender(unit_base* sender, float level);

private:
    std::unique_ptr<unit_base>  m_receiver;
    std::vector<aux_send>       m_sends;
};

#endif

//-------------------------------------------------------------------------------------------------------
#ifdef WPN_AUDIO_TRACKS
// automate audio processing for a chain of units
//-------------------------------------------------------------------------------------------------------

class track_unit : public unit_base
{
public:
#ifdef WPN_CONTROL_OSSIA //--------------------------------------------------------------------------------------
    void net_expose_plugin_tree(ossia::net::node_base& plugin_root) override;
#endif //------------------------------------------------------------------------------------------------

    track_unit();
    ~track_unit();
    void preprocessing(size_t sample_rate, uint16_t nsamples) override;
    void process_audio(uint16_t nsamples) override;
    void process_audio(float** input_buffer, uint16_t nsamples) override;
    void add_unit(unit_base* unit);
    void remove_unit(unit_base* unit);

private:
    std::vector<unit_base*> m_units;
};

#endif

}
}
