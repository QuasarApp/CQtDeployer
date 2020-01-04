#include "Distributions/idistribution.h"
#include "packing.h"

Packing::Packing() {

}

Packing::~Packing() {
    if (_pakage)
        delete _pakage;
}

void Packing::setDistribution(iDistribution *pakage) {
    _pakage = pakage;
}

bool Packing::create() {

    if (!_pakage)
        return false;

    return true;
}
