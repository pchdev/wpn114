/* adaptation of remaincalm.org Paranoia Mangler JS effect for Reaper
 * author: Daniel Arena (dan@remaincalm.org)
*/

#ifndef MANGLER_HPP
#define MANGLER_HPP

#include <src/audio/audio.hpp>

class Mangler : public StreamNode
{
    Q_OBJECT

    Q_PROPERTY  ( qreal inputGain READ inputGain WRITE setInputGain )
    Q_PROPERTY  ( qreal dryOut READ dryOut WRITE setDryOut )
    Q_PROPERTY  ( qreal badResampler READ badResampler WRITE setBadResampler )
    Q_PROPERTY  ( bool bitcrusher READ bitcrusher WRITE setBitcrusher )
    Q_PROPERTY  ( qreal thermonuclear READ thermonuclear WRITE setThermonuclear )
    Q_PROPERTY  ( int bitdepth READ bitdepth WRITE setBitdepth )
    Q_PROPERTY  ( qreal gate READ gate WRITE setGate )
    Q_PROPERTY  ( qreal love READ love WRITE setLove )
    Q_PROPERTY  ( qreal jive READ jive WRITE setJive )
    Q_PROPERTY  ( int attitude READ attitude WRITE setAttitude )

    public:
    Mangler();

    virtual void initialize ( qint64 ) override;
    virtual float** process ( float**, qint64 ) override;

    qreal inputGain         ( ) const { return m_input_gain; }
    qreal dryOut            ( ) const { return m_dry_out; }
    qreal badResampler      ( ) const { return m_bad_resampler; }
    bool bitcrusher         ( ) const { return m_bitcrusher; }
    qreal thermonuclear     ( ) const { return m_thermonuclear; }
    int bitdepth            ( ) const { return m_bitdepth; }
    qreal gate              ( ) const { return m_gate; }
    qreal love              ( ) const { return m_love; }
    qreal jive              ( ) const { return m_jive; }
    int attitude            ( ) const { return m_attitude; }

    void setInputGain       ( qreal input_gain ) { m_input_gain = input_gain; }
    void setDryOut          ( qreal dry_out ) { m_dry_out = dry_out; }
    void setBadResampler    ( qreal bad_resampler ) { m_bad_resampler = bad_resampler; }
    void setBitcrusher      ( bool bitcrusher ) { m_bitcrusher = bitcrusher; }
    void setThermonuclear   ( qreal thermonuclear ) { m_thermonuclear = thermonuclear; }
    void setBitdepth        ( int bitdepth ) { m_bitdepth = bitdepth; }
    void setGate            ( qreal gate ) { m_gate = gate; }
    void setLove            ( qreal love ) { m_love = love; }
    void setJive            ( qreal jive ) { m_jive = jive; }
    void setAttitude        ( int attitude ) { m_attitude = attitude; }

    private:
    qreal m_input_gain;
    qreal m_dry_out;
    qreal m_bad_resampler;
    bool m_bitcrusher;
    qreal m_thermonuclear;
    int m_bitdepth;
    qreal m_gate;
    qreal m_love;
    qreal m_jive;
    int m_attitude;
};

#endif // MANGLER_HPP
