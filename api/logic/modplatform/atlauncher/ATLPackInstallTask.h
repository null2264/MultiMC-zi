#pragma once

#include <meta/VersionList.h>
#include "ATLPackManifest.h"

#include "InstanceTask.h"
#include "multimc_logic_export.h"
#include "net/NetJob.h"
#include "settings/INISettingsObject.h"
#include "minecraft/MinecraftInstance.h"
#include "minecraft/PackProfile.h"
#include "meta/Version.h"

#include <nonstd/optional>

namespace ATLauncher {

class MULTIMC_LOGIC_EXPORT UserInteractionSupport {

public:
    /**
     * Requests a user interaction to select which optional mods should be installed.
     */
    virtual QVector<QString> chooseOptionalMods(QVector<ATLauncher::VersionMod> mods) = 0;

    /**
     * Requests a user interaction to select a component version from a given version list
     * and constrained to a given Minecraft version.
     */
    virtual QString chooseVersion(Meta::VersionListPtr vlist, QString minecraftVersion) = 0;

};

class MULTIMC_LOGIC_EXPORT PackInstallTask : public InstanceTask
{
Q_OBJECT

public:
    explicit PackInstallTask(UserInteractionSupport *support, QString pack, QString version);
    virtual ~PackInstallTask(){}

    bool abort() override;

protected:
    virtual void executeTask() override;

private slots:
    void onDownloadSucceeded();
    void onDownloadFailed(QString reason);

    void onModsDownloaded();
    void onModsExtracted();

private:
    QString getDirForModType(ModType type, QString raw);
    QString getVersionForLoader(QString uid);
    QString detectLibrary(VersionLibrary library);

    bool createLibrariesComponent(QString instanceRoot, std::shared_ptr<PackProfile> profile);
    bool createPackComponent(QString instanceRoot, std::shared_ptr<PackProfile> profile);

    void installConfigs();
    void extractConfigs();
    void downloadMods();
    bool extractMods(
        const QMap<QString, VersionMod> &toExtract,
        const QMap<QString, VersionMod> &toDecomp,
        const QMap<QString, QString> &toCopy
    );
    void install();

private:
    UserInteractionSupport *m_support;

    NetJobPtr jobPtr;
    QByteArray response;

    QString m_pack;
    QString m_version_name;
    PackVersion m_version;

    QMap<QString, VersionMod> modsToExtract;
    QMap<QString, VersionMod> modsToDecomp;
    QMap<QString, QString> modsToCopy;

    QString archivePath;
    QStringList jarmods;
    Meta::VersionPtr minecraftVersion;
    QMap<QString, Meta::VersionPtr> componentsToInstall;

    QFuture<nonstd::optional<QStringList>> m_extractFuture;
    QFutureWatcher<nonstd::optional<QStringList>> m_extractFutureWatcher;

    QFuture<bool> m_modExtractFuture;
    QFutureWatcher<bool> m_modExtractFutureWatcher;

};

}
