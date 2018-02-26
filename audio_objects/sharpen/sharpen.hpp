#pragma once

#include <src/audiobackend.hpp>

class Sharpen : public AudioEffectObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES    ( QQmlParserStatus )
    Q_PROPERTY      ( qreal depth READ depth WRITE setDepth )

public:
    Sharpen();

    virtual void classBegin() override;
    virtual void componentComplete() override;
    virtual float** process(const quint16 nsamples) override;

    qreal   depth() const;
    void    setDepth(const qreal);

private:
    qreal*  m_ssd;
    qreal   m_depth;

};


