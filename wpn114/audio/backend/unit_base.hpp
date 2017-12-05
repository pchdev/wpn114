#pragma once
//-------------------------------------------------------------------------------------------------------
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
    const std::string& netname() const;
#endif //------------------------------------------------------------------------------------------------
    virtual void    process(uint16_t nsamples) = 0;
    virtual void    process(float** inputs, uint16_t nsamples) = 0;
    // the unit's audio callbacks
    virtual void    preprocess(size_t srate, uint16_t nsamples) = 0;
    // preparing the units user-defined audio processing
    void            bufalloc(uint16_t nsamples);
    //              pre-initializes output buffer, setting it to 0
    float           framedata(uint8_t channel, uint16_t frame) const;
    float**         out();

    bool            active()        const;
    float           level()         const;
    uint8_t         nchannels()     const;
    unit_type       uttype()        const;

    void            set_level(float level);
    void            activate();
    void            deactivate();

#ifdef WPN_AUDIO_AUX //---------------------------------------------------------------------------------
    void        add_aux_send(aux_unit& aux); // register the unit to an aux bus
#endif //------------------------------------------------------------------------------------------------
protected:
#define OUT                 m_out
#define SET_UTYPE(u)        m_uttype = u
#define SETN_IN(n)          m_nin = n
#define SETN_OUT(n)         m_nout = n
#define N_OUT               m_nout
#define N_IN                m_nin
#define VOID_PROCESS        void process(uint16_t) override {}
#define VOID_INPUT_PROCESS  void process(float**, uint16_t) override {}
#define VOID_PREPROCESS     void preprocess(size_t, uint16_t) override {}
//-------------------------------------------------------------------------------------------------------
    bool            m_active;
    float           m_level;
    uint8_t         m_nin;
    uint8_t         m_nout;
    float**         m_out;
    unit_type       m_uttype;

#ifdef WPN_CONTROL_OSSIA // -------------------------------------------------------------------------------------
    std::string                 m_netname;
    ossia::net::node_base*      m_netnode;
#define VOID_NET_EXPOSE         void net_expose_plugin_tree(ossia::net::node_base&) override {}
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
#define SFBUF          m_sndbuf
#define SFLOAD(p)      load_soundfile(m_sndbuf, p)
#define SFBUF_CLEAR    delete m_sndbuf.data
    sndbuf_t           m_sndbuf;
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

    void    process(uint16_t nsamples)                              override;
    void    process(float** inputs, uint16_t nsamples)              override;
    void    preprocess(size_t srate, uint16_t nsamples)             override;
    float   framedata(uint8_t channel, uint16_t frame)              const;
    void    set_receiver(std::unique_ptr<unit_base> receiver);
    void    add_sender(unit_base& sender, float level);

private:
    std::unique_ptr<unit_base>  m_receiver;
    std::vector<aux_send>       m_sends;
};

//std::ostream& operator<<(std::ostream& aux, const unit_base& unit);
//std::ostream& operator>>(std::ostream& unit, const aux_unit& aux);

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

    void    process(uint16_t nsamples)                      override;
    void    process(float** inputs, uint16_t nsamples)      override;
    void    preprocess(size_t srate, uint16_t nsamples)     override;
    void    add_unit(unit_base& unit);
    void    remove_unit(unit_base& unit);

private:
    std::vector<unit_base*> m_units;
};

//std::ostream& operator<<(std::ostream& track, const track_unit& unit);
//std::ostream& operator>>(std::ostream& track, const track_unit& unit);

#endif

}
}
