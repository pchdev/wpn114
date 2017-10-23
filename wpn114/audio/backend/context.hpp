#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <cstdint>

namespace wpn114 {
namespace audio {

static struct context
{
    uint32_t    sample_rate;
    uint32_t    blocksize;
    uint16_t    num_inputs;
    uint16_t    num_outputs;
    float       master_tempo;

} context;

}
}

#endif // CONTEXT_HPP
