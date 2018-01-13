#include "SC_PlugIn.h"
#include "SC_SndBuf.h"
#include <math.h>
#include <iostream>
#include "aeffect.h"
#include "aeffectx.h"

static InterfaceTable *ft;

struct VSTGen : public Unit
{    
    AEffect*    m_plugin;
};

struct VSTEffect    : public VSTGen {};
struct VSTEffect2   : public VSTGen {};

static void VSTGen_next(VSTGen* unit, int nsamples);
static void VSTEffect_next(VSTEffect* unit, int nsamples);
static void VSTEffect2_next(VSTEffect2* unit, int nsamples);

static void VSTGen_Ctor(VSTGen* unit);
static void VSTEffect_Ctor(VSTEffect* unit);
static void VSTEffect2_Ctor(VSTEffect2* unit);

template<typename T> void VST_Ctor(T* unit, void (*next_func)(T*, int))
{
    SETCALC(next_func);

    // dave... this is highly irregular...
    AEffect* plugin = (AEffect*) (uintptr_t) IN0(0);
    std::cout << IN0(0) << std::endl;

    if  (plugin)
    {
        unit->m_plugin = plugin;
        plugin->dispatcher(plugin, effOpen, 0, 0, NULL, 0.0f);
        plugin->dispatcher(plugin, effSetSampleRate, 0, 0, NULL, SAMPLERATE);
        plugin->dispatcher(plugin, effSetBlockSize, 0, 0, NULL, BUFLENGTH);
        plugin->dispatcher(plugin, effMainsChanged, 0, 0, NULL, 0.0f);
    }
}

void VSTGen_Ctor(VSTGen *unit)              { VST_Ctor<VSTGen>(unit, VSTGen_next); }
void VSTEffect_Ctor(VSTEffect *unit)        { VST_Ctor<VSTEffect>(unit, VSTEffect_next); }
void VSTEffect2_Ctor(VSTEffect2 *unit)      { VST_Ctor<VSTEffect2>(unit, VSTEffect2_next); }

void VSTGen_next(VSTGen* unit, int nsamples)
{
    AEffect* plugin = unit->m_plugin;
    plugin->processReplacing(plugin, nullptr, &OUT(0), nsamples);
}

void VSTEffect_next(VSTEffect *unit, int nsamples)
{
    AEffect* plugin = unit->m_plugin;
    plugin->processReplacing(plugin, &IN(0), &OUT(0), nsamples);
}

void VSTEffect2_next(VSTEffect2 *unit, int nsamples)
{
    AEffect* plugin = unit->m_plugin;
    plugin->processReplacing(plugin, &IN(0), &OUT(0), nsamples);
}

PluginLoad(VSTGen)
{
    ft = inTable;
    DefineSimpleUnit(VSTGen);
}
