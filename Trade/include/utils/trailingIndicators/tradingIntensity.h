#pragma once

#include <vector>
#include <map>
#include <mutex>
#include <string.h>
#include "common/mydatamanager.h"

//forward declare
struct Mydata; 

class CTradingIntensityIndicator
{
private:
    double _alpha;
    double _kappa;
    std::vector<Mydata>* _marketData;
    unsigned int _samplingLength;
    std::vector<int> _sampleIndex;
    const unsigned int _priceMultiplier;

public:
    CTradingIntensityIndicator(std::vector<Mydata>* marketData, unsigned int samplingLength, unsigned int priceMultiplier)
        : _marketData(marketData), _samplingLength(samplingLength), _priceMultiplier(priceMultiplier)
    {}
    ~CTradingIntensityIndicator();
    void register_md(int i);
    double estimate_intensity();

private:
    std::map<unsigned int, int> _calculate_instant_volume_at_price_level(const Mydata oldLOBSnapshot, const Mydata newLOBSnapshot);
}