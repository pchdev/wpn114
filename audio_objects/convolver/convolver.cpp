#include "convolver.hpp"
#include <QtDebug>

Convolver::Convolver() : m_ir(nullptr), m_convolver_l(nullptr), m_convolver_r(nullptr),
    m_buffer(nullptr)
{
    SETN_IN     ( 2 );
    SETN_OUT    ( 2 );
    SETTYPE     ( StreamType::Effect );
}

Convolver::~Convolver()
{
    delete m_ir;
    delete m_convolver_l;
    delete m_convolver_r;
}

void Convolver::setIrPath(QString path)
{
    m_ir_path = path;
}

void Convolver::componentComplete()
{
    m_ir = new Soundfile( m_ir_path );
    m_convolver_l = new FFTConvolver;
    m_convolver_r = new FFTConvolver;

    auto nsamples = m_ir->nsamples();

    StreamNode::allocateBuffer(m_buffer, m_ir->nchannels(), nsamples);
    m_ir->buffer(m_buffer, 0, nsamples);
}

void Convolver::initialize(qint64 nsamples)
{    
    auto ns = m_ir->nsamples();

    m_convolver_l->init( CONVOLVER_BUFFER_SIZE, m_buffer[0], ns );

    if      ( m_ir->nchannels() == 1 )
        m_convolver_r->init( CONVOLVER_BUFFER_SIZE, m_buffer[0], ns );

    else if ( m_ir->nchannels() == 2 )
        m_convolver_r->init( CONVOLVER_BUFFER_SIZE, m_buffer[1], ns );
}

float** Convolver::process(float** buf, qint64 nsamples)
{
    auto nout   = m_num_outputs;
    auto out    = m_out;   

    StreamNode::resetBuffer( m_out, nout, nsamples );

    m_convolver_l->process( buf[ 0 ], out[ 0 ], nsamples );
    m_convolver_r->process( buf[ 1 ], out[ 1 ], nsamples );

    return out;
}

