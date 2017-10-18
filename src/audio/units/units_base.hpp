#ifndef SUNITS_BASE_HPP
#define SUNITS_BASE_HPP

namespace wpn114 {
namespace audio {
namespace units {

enum units_type {
    EFFECT_UNIT,
    GENERATOR_UNIT,
    HYBRID_UNIT
};

class units_base
{
public:
    virtual void ~units_base();
    virtual void start() = 0;
    virtual void suspend() = 0;
    virtual void resume() = 0;
    virtual void expose() = 0;
    virtual void show() = 0;
    virtual void process_midi() = 0;
    void get_callback()
    {
        return m_callback;
    }

private:
    virtual float process_audio() = 0;
    int m_num_inputs;
    int m_num_outputs;
    units_type m_type;
    float m_callback;

};
}
}
}

#endif // SUNITS_BASE_HPP
