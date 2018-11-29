#ifndef CONVOLVER_HPP
#define CONVOLVER_HPP

#include <source/audio/audio.hpp>
#include <source/audio/soundfile.hpp>
#include <external/fftconvolver/FFTConvolver.h>

using namespace fftconvolver;
#define CONVOLVER_BUFFER_SIZE 1024

class Convolver : public StreamNode
{
    Q_OBJECT

    Q_PROPERTY  ( QString irPath READ irPath WRITE setIrPath )

    public:
    Convolver();
    ~Convolver();

    virtual void componentComplete() override;

    virtual void initialize(qint64) override;
    virtual float** process(float** buf, qint64 nsamples) override;

    QString irPath() const { return m_ir_path; }
    void setIrPath(QString path);

    private:
    FFTConvolver* m_convolver_l;
    FFTConvolver* m_convolver_r;
    float** m_buffer;
    Soundfile* m_ir;
    QString m_ir_path;
};

#endif // CONVOLVER_HPP
