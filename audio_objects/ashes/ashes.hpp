#ifndef ASHES_H
#define ASHES_H

#include <src/audiobackend.hpp>

class Ashes : public AudioObject, public QQmlParserStatus
{

public:
    Ashes();
    void classBegin() override;
    void componentComplete() override;
    float** process(const quint16 nsamples) override;

};

#endif // ASHES_H
