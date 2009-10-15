/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2009-08-09
 * Description : central place for ICC settings
 *
 * Copyright (C) 2005-2006 by F.J. Cruz <fj.cruz@supercable.es>
 * Copyright (C) 2005-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "iccsettings.h"
#include "iccsettings.moc"

// X11 includes

#ifdef Q_WS_X11
#include <climits>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <fixx11h.h>
#include <QX11Info>
#endif /* Q_WS_X11 */

// Qt includes

#include <QApplication>
#include <QDesktopWidget>
#include <QDir>
#include <QFileInfo>
#include <QMutex>
#include <QMutexLocker>

// KDE includes

#include <kconfig.h>
#include <kconfiggroup.h>
#include <kglobal.h>
#include <ksharedconfig.h>

// Local includes

#include "iccprofile.h"
#include "icctransform.h"
#include "debug.h"

namespace Digikam
{

class IccSettingsPriv
{
public:

    IccSettingsPriv(){}

    ICCSettingsContainer   settings;
    QMutex                 mutex;

    QList<IccProfile>      profiles;

    QHash<int, IccProfile> screenProfiles;

    QList<IccProfile> scanDirectories(const QStringList& dirs);
    void scanDirectory(const QString& path, const QStringList& filter, QList<IccProfile> *profiles);

    IccProfile profileFromWindowSystem(QWidget *widget);
};

class IccSettingsCreator { public: IccSettings object; };
K_GLOBAL_STATIC(IccSettingsCreator, creator)

IccSettings *IccSettings::instance()
{
    return &creator->object;
}

IccSettings::IccSettings()
           : d(new IccSettingsPriv)
{
    IccTransform::init();
    readFromConfig();

    qRegisterMetaType<ICCSettingsContainer>("ICCSettingsContainer");
}

IccSettings::~IccSettings()
{
    delete d;
}

ICCSettingsContainer IccSettings::settings()
{
    QMutexLocker lock(&d->mutex);
    ICCSettingsContainer s(d->settings);
    return s;
}

IccProfile IccSettings::monitorProfile(QWidget *widget)
{
    // system-wide profile set?
    IccProfile profile = d->profileFromWindowSystem(widget);
    if (!profile.isNull())
        return profile;

    QMutexLocker lock(&d->mutex);
    if (!d->settings.monitorProfile.isNull())
        return d->settings.monitorProfile;
    else
        return IccProfile::sRGB();
}

bool IccSettings::monitorProfileFromSystem()
{
    // First, look into cache
    {
        QMutexLocker lock(&d->mutex);
        foreach (const IccProfile &profile, d->screenProfiles)
            if (!profile.isNull())
                return true;
    }

    // Second, check all toplevel widgets
    QList<QWidget*> topLevels = qApp->topLevelWidgets();
    foreach (QWidget *widget, topLevels)
        if (!d->profileFromWindowSystem(widget).isNull())
            return true;

    return false;
}

/*
 * From koffice/libs/pigment/colorprofiles/KoLcmsColorProfileContainer.cpp
 * Copyright (c) 2000 Matthias Elter <elter@kde.org>
 *                2001 John Califf
 *                2004 Boudewijn Rempt <boud@valdyas.org>
 *  Copyright (c) 2007 Thomas Zander <zander@kde.org>
 *  Copyright (c) 2007 Adrian Page <adrian@pagenet.plus.com>IccProfile IccSettingsPriv::profileForScreen(QWidget *widget)
*/
IccProfile IccSettingsPriv::profileFromWindowSystem(QWidget *widget)
{
#ifdef Q_WS_X11

    Qt::HANDLE appRootWindow;
    QString atomName;

    QDesktopWidget *desktop = QApplication::desktop();
    int screenNumber = desktop->screenNumber(widget);

    IccProfile profile;
    {
        QMutexLocker lock(&mutex);
        if (screenProfiles.contains(screenNumber))
            return screenProfiles.value(screenNumber);
    }

    if (desktop->isVirtualDesktop())
    {
        appRootWindow = QX11Info::appRootWindow(QX11Info::appScreen());
        atomName = QString("_ICC_PROFILE_%1").arg(screenNumber);
    }
    else
    {
        appRootWindow = QX11Info::appRootWindow(screenNumber);
        atomName = "_ICC_PROFILE";
    }

    Atom          type;
    int           format;
    unsigned long nitems;
    unsigned long bytes_after;
    quint8 *      str;

    static Atom icc_atom = XInternAtom( QX11Info::display(), atomName.toLatin1(), True );

    if  ( icc_atom != None &&
          XGetWindowProperty ( QX11Info::display(),
                    appRootWindow,
                    icc_atom,
                    0,
                    INT_MAX,
                    False,
                    XA_CARDINAL,
                    &type,
                    &format,
                    &nitems,
                    &bytes_after,
                    (unsigned char **) &str) == Success
                ) {
        QByteArray bytes = QByteArray::fromRawData((char*)str, (quint32)nitems);

        profile = bytes;
        kDebug(digiKamAreaCode) << "Found X.org XICC monitor profile" << profile.description();
    }
    //else
      //  kDebug(digiKamAreaCode) << "No X.org XICC profile installed for screen" << screenNumber;

    // insert to cache even if null
    {
        QMutexLocker lock(&mutex);
        screenProfiles.insert(screenNumber, profile);
    }
    return profile;

#elif defined Q_WS_WIN
    //TODO
#elif defined Q_WS_MAC
    //TODO
#endif

    return IccProfile();
}

bool IccSettings::isEnabled()
{
    return d->settings.enableCM;
}

void IccSettings::readFromConfig()
{
    ICCSettingsContainer old, s;
    KSharedConfig::Ptr config = KGlobal::config();
    KConfigGroup group = config->group(QString("Color Management"));
    s.readFromConfig(group);
    {
        QMutexLocker lock(&d->mutex);
        old = d->settings;
        d->settings = s;
    }
    emit settingsChanged();
    emit settingsChanged(s, old);
}

void IccSettings::setSettings(const ICCSettingsContainer& settings)
{
    ICCSettingsContainer old;
    {
        QMutexLocker lock(&d->mutex);
        if (settings.iccFolder != d->settings.iccFolder)
            d->profiles.clear();
        old = d->settings;
        d->settings = settings;
    }
    KSharedConfig::Ptr config = KGlobal::config();
    KConfigGroup group = config->group(QString("Color Management"));
    settings.writeToConfig(group);
    emit settingsChanged();
    emit settingsChanged(settings, old);
}

void IccSettings::setUseManagedView(bool useManagedView)
{
    ICCSettingsContainer old, current;
    {
        QMutexLocker lock(&d->mutex);
        old = d->settings;
        d->settings.useManagedView = useManagedView;
        current = d->settings;
    }
    KSharedConfig::Ptr config = KGlobal::config();
    KConfigGroup group = config->group(QString("Color Management"));
    d->settings.writeManagedViewToConfig(group);
    emit settingsChanged();
    emit settingsChanged(current, old);
}

void IccSettings::setIccPath(const QString& path)
{
    ICCSettingsContainer old, current;
    {
        QMutexLocker lock(&d->mutex);
        if (path == d->settings.iccFolder)
            return;
        d->profiles.clear();
        old = d->settings;
        d->settings.iccFolder = path;
        current = d->settings;
    }
    KSharedConfig::Ptr config = KGlobal::config();
    KConfigGroup group = config->group(QString("Color Management"));
    d->settings.writeManagedViewToConfig(group);
    emit settingsChanged();
    emit settingsChanged(current, old);
}

QList<IccProfile> IccSettingsPriv::scanDirectories(const QStringList& dirs)
{
    QList<IccProfile> profiles;

    QStringList filters;
    filters << "*.icc" << "*.icm";
    kDebug() << dirs;
    foreach (const QString &dirPath, dirs)
    {
        QDir dir(dirPath);
        if (!dir.exists())
            continue;
        scanDirectory(dir.path(), filters, &profiles);
    }

    return profiles;
}

void IccSettingsPriv::scanDirectory(const QString& path, const QStringList& filter, QList<IccProfile> *profiles)
{
    QDir dir(path);
    QFileInfoList infos;
    infos << dir.entryInfoList(filter, QDir::Files | QDir::Readable);
    infos << dir.entryInfoList(QDir::Dirs | QDir::Readable | QDir::NoDotAndDotDot);
    foreach (const QFileInfo &info, infos)
    {
        if (info.isFile())
        {
            //kDebug(digiKamAreaCode) << info.filePath() << (info.exists() && info.isReadable());
            IccProfile profile(info.filePath());
            if (profile.open())
            {
                *profiles << profile;
                if (info.fileName() == "AdobeRGB1998.icc")
                    IccProfile::considerOriginalAdobeRGB(info.filePath());
            }
        }
        else if (info.isDir() && !info.isSymLink())
        {
            scanDirectory(info.filePath(), filter, profiles);
        }
    }
}

QList<IccProfile> IccSettings::allProfiles()
{
    QString extraPath;
    {
        QMutexLocker lock(&d->mutex);
        if (!d->profiles.isEmpty())
            return d->profiles;
        extraPath = d->settings.iccFolder;
    }

    QList<IccProfile> profiles;
    // get system paths, e.g. /usr/share/color/icc
    QStringList paths = IccProfile::defaultSearchPaths();
    // add user-specified path
    if (!extraPath.isEmpty() && !paths.contains(extraPath))
        paths << extraPath;
    // check search directories
    profiles << d->scanDirectories(paths);
    // load profiles that come with libkdcraw
    profiles << IccProfile::defaultProfiles();

    QMutexLocker lock(&d->mutex);
    d->profiles = profiles;

    return d->profiles;
}

QList<IccProfile> IccSettings::workspaceProfiles()
{
    QList<IccProfile> profiles;
    foreach (IccProfile profile, allProfiles())
    {
        switch (profile.type())
        {
            case IccProfile::Display:
            case IccProfile::ColorSpace:
                profiles << profile;
            default:
                break;
        }
    }
    return profiles;
}

QList<IccProfile> IccSettings::displayProfiles()
{
    QList<IccProfile> profiles;
    foreach (IccProfile profile, allProfiles())
    {
        if (profile.type() == IccProfile::Display)
            profiles << profile;
    }
    return profiles;
}

QList<IccProfile> IccSettings::inputProfiles()
{
    QList<IccProfile> profiles;
    foreach (IccProfile profile, allProfiles())
    {
        switch (profile.type())
        {
            case IccProfile::Input:
            case IccProfile::ColorSpace:
                profiles << profile;
            default:
                break;
        }
    }
    return profiles;
}

QList<IccProfile> IccSettings::outputProfiles()
{
    QList<IccProfile> profiles;
    foreach (IccProfile profile, allProfiles())
    {
        if (profile.type() == IccProfile::Output)
            profiles << profile;
    }
    return profiles;
}

void IccSettings::loadAllProfilesProperties()
{
    allProfiles();
    const int size = d->profiles.size();
    for (int i=0; i<size; ++i)
    {
        IccProfile& profile = d->profiles[i];
        if (!profile.isOpen())
        {
            profile.description();
            profile.type();
            profile.close();
        }
        else
        {
            profile.description();
            profile.type();
        }
    }
}

}  // namespace Digikam
