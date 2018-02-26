#ifndef ASHES_H
#define ASHES_H

#include <src/audiobackend.hpp>

class Ashes : public AudioObject, public QQmlParserStatus
{
    Q_OBJECT

public:
    Ashes();
    virtual void classBegin()           override;
    virtual void componentComplete()    override;
    virtual float** process(const quint16 nsamples) override;

};

#endif // ASHES_H
