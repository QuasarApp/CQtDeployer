#ifndef PACKING_H
#define PACKING_H

/**
 * @brief The Packing class - this class have interface for configure package and
 * create package from deployment distibution.
 */

class ConfigParser;
class iDistribution;

class Packing
{
public:
    Packing();
    ~Packing();
    void setDistribution(iDistribution *pakage);
    bool create();
private:
    iDistribution *_pakage = nullptr;
};

#endif // PACKING_H
