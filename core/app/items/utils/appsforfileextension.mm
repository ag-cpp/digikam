/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-01-28
 * Description : Objective-C wrapper for open-with operations under MacOS
 *
 * Copyright (C) 2021      by Robert Lindsay <robert dot lindsay at gmail dot com>
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt include

#include <QString>
#include <QList>
#include <QUrl>

// MacOS header

#import <Foundation/Foundation.h>

#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_export.h"

/**
 * Given a filename extension 'suffix', here's how to find all of the
 * applications known to the MacOS who can open files of that type.
 * Return a list of suitable MacOS  bundle urls for 'suffix'.
 */
DIGIKAM_GUI_EXPORT QList<QUrl> MacApplicationForFileExtension(const QString& suffix)
{
    QList<QUrl> appUrls;

    if (suffix.isEmpty())
    {
        qCWarning(DIGIKAM_GENERAL_LOG) << "Suffix is empty";
        return appUrls;
    }

    // Make a Uniform Type Identifier from a filename extension.

    CFArrayRef  bundleIDs             = nullptr;
    CFStringRef extensionRef          = suffix.toCFString();
    CFStringRef uniformTypeIdentifier = UTTypeCreatePreferredIdentifierForTag(kUTTagClassFilenameExtension, extensionRef, nullptr);

    if (!uniformTypeIdentifier)
    {
        qCWarning(DIGIKAM_GENERAL_LOG) << "Cannot get the Uniform Type Identifier for" << suffix;
        return appUrls;
    }

    // Get a list of all of the application bundle IDs that know how to handle this.

    bundleIDs = LSCopyAllRoleHandlersForContentType(uniformTypeIdentifier, kLSRolesViewer | kLSRolesEditor);

    if (bundleIDs)
    {
        // Find all the available applications with this bundle ID.
        // We can also get the display name and version if necessary.

        const CFIndex count = CFArrayGetCount(bundleIDs);

        for (CFIndex i = 0 ; i < count ; ++i)
        {
            CFStringRef val            = (CFStringRef)(CFArrayGetValueAtIndex(bundleIDs, i));
            CFArrayRef appsForBundleID = LSCopyApplicationURLsForBundleIdentifier(val, nullptr);

            if (appsForBundleID)
            {
                // TODO: call CFURLResourceIsReachable() on each item before to use it.

                QList<QUrl> urls;
                CFIndex size = CFArrayGetCount(appsForBundleID);

                for (CFIndex j = 0 ; j < size ; ++j)
                {
                    CFPropertyListRef prop = CFArrayGetValueAtIndex(appsForBundleID, j);

                    if (prop)
                    {
                        CFTypeID typeId = CFGetTypeID(prop);

                        if (typeId == CFURLGetTypeID())
                        {
                            urls << QUrl::fromCFURL(static_cast<CFURLRef>(prop));
                        }
                        else
                        {
                            qCWarning(DIGIKAM_GENERAL_LOG) << "Application Bundle Property type is not CFURL:" << typeId << "(" << QString::fromCFString(CFCopyTypeIDDescription(typeId)) << ")";
                        }
                    }
                    else
                    {
                        qCWarning(DIGIKAM_GENERAL_LOG) << "Cannot get url" << j << "for application" << i;
                    }
                }

                appUrls << urls;
                CFRelease(appsForBundleID);
            }
        }
    }
    else
    {
        qCWarning(DIGIKAM_GENERAL_LOG) << "Cannot get the Application urls list for" << suffix;
    }

    // "applications" now has an array of ALL of the possible applications
    // when finished. Given a UI to choose one, you can then call
    // LSOpenFromURLSpec() to open your file with a specific application.

    // TODO: host applications properties to a class with Qt containers and return it.
    // Look how it's done in QtCore implementation:
    // qtbase/src/plugins/platforms/cocoa/qcocoanativeinterface.mm : QCocoaNativeInterface::defaultBackgroundPixmapForQWizard()
    // qtbase/src/corelib/io/qfilesystemengine_unix.cpp            : isPackage()
    // qtbase/src/corelib/global/qlibraryinfo.cpp                  : getRelocatablePrefix()
    // qtbase/src/corelib/plugin/qlibrary_unix.cpp                 : load_sys()

    // Release the resources.

    CFRelease(uniformTypeIdentifier);
    CFRelease(bundleIDs);

    return appUrls;
}
