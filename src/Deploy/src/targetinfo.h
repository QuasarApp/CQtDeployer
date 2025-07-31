//#
//# Copyright (C) 2018-2024 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#ifndef TARGETINFO_H
#define TARGETINFO_H

#include "distrostruct.h"
#include "libinfo.h"

/**
 * @brief The TargetInfo class This clas scontains information about target object. (executable or another files marked in a bin option)
 */
class DEPLOYSHARED_EXPORT TargetInfo: public LibInfo
{
public:
    TargetInfo();

    /**
     * @brief getPackage This method return name of the package in that contains this target.
     * @return  name of the package in that contains this target.
     */
    const QString & getPackage() const;

    /**
     * @brief setPackage This method sets new value for package name in that contains this target.
     * @param value new This is new value of the package.
     */
    void setPackage(const QString &value);

    /**
     * @brief getIcon This method return path to default icon of the target.
     * @return icon path.
     * @note if the icon not set then return default icon.
     */
    QString getIcon() const;

    /**
     * @brief setIcon This method sets new iconf for target.
     * @param value
     */
    void setIcon(const QString &value);

    /**
     * @brief getShortCut This method return true if the target need to create shortcut.
     * @return true if shortcut needed for this target.
     */
    bool getShortCut() const;

    /**
     * @brief setShortCut This method sets new status of the creating shortcut.
     * @param shortcut This is new value of the shortcut
     */
    void setShortCut(bool shortcut);

    /**
     * @brief disableShortCut This method disable shortcut for this target.
     */
    void disableShortCut();

    /**
     * @brief getRunScript This method return path to run script.
     * @return path to run script
     * @note By Default return path to default run scrip file from cqtdeployer resources.
     */
    QString getRunScript() const;

    /**
     * @brief setRunScript sets New path of the run script.
     * @param newRunScript This is new path of the run sscript.
     */
    void setRunScript(const QString &newRunScript);

    /**
     * @brief getRunScriptFile This method return name fo the run script file or executable file if the run script are not needed
     * @return runscript name
     */
    QString getRunScriptFile() const;


    /**
     * @brief fEnableRunScript This return true if the run script will be generateed for this target else false.
     * @return true if the run script will be generateed for this target else false.
     */
    bool fEnableRunScript() const;

    /**
     * @brief setFEnableRunScript This method enable or disable generation runScript.
     * @param newFEnableRunScript New value.
     */
    void setFEnableRunScript(bool newFEnableRunScript);

    /**
     * @brief disableRunScript This method disable run script for this target.
     */
    void disableRunScript();

private:
    QString _package = "";
    QString _icon = "";
    bool _fEnableShortCut = true;
    bool _fEnableRunScript = true;

    QString _runScript = "";

 };

#endif // TARGETINFO_H
