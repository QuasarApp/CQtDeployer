#ifndef CQT_H
#define CQT_H
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>


/**
 * @brief The CQT class - it is wrapper for cqt file. qcn it is configuration file of deploying project.
 */
class CQT
{
private:

    bool createFromDeploy() const;
    bool loadFromFile();

public:
    CQT(const QString& targetDir);

};

#endif // CQT_H
