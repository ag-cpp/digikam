// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2015 Ilya Kowalewski <illya.kovalevskyy@gmail.com>
//

#include "MarbleLocale.h"
#include "MarbleLocale_p.h"
#include "MarbleGlobal.h"
#include "digikam_debug.h"

// Qt
#include <QLocale>

#include <klocalizedstring.h>

namespace Marble
{

MarbleLocalePrivate::MarbleLocalePrivate()
    : m_measurementSystem( MarbleLocale::MetricSystem )
{
}

MarbleLocalePrivate::~MarbleLocalePrivate()
{
}

MarbleLocale::MarbleLocale()
    : d ( new MarbleLocalePrivate )
{
}

MarbleLocale::~MarbleLocale()
{
    delete d;
}

void MarbleLocale::setMeasurementSystem( MarbleLocale::MeasurementSystem measurementSystem )
{
    d->m_measurementSystem = measurementSystem;
}

MarbleLocale::MeasurementSystem MarbleLocale::measurementSystem() const
{
    return d->m_measurementSystem;
}

void MarbleLocale::meterToTargetUnit(qreal meters, MeasurementSystem targetSystem,
                                     qreal &targetValue, MeasureUnit &targetUnit)
{
    targetValue = meters;

    switch (targetSystem) {
    case MetricSystem: {
        if (targetValue > 1000.0) {
            targetValue *= METER2KM;
            targetUnit = Kilometer;
            break;
        }

        if (targetValue < 1.0 && targetValue >= 0.01) {
            targetValue *= M2CM;
            targetUnit = Centimeter;
            break;
        }

        if (targetValue < 0.01) {
            targetValue *= M2MM;
            targetUnit = Milimeter;
            break;
        }

        targetUnit = Meter;
        break;
    }

    case ImperialSystem: {
        // meters into feet
        qreal ftValue = targetValue * M2FT;

        if (ftValue < 1) {
            targetValue *= M2IN;
            targetUnit = Inch;
            break;
        }

        if (ftValue >= 3 && ftValue < 5280.0) {
            targetValue *= M2YD;
            targetUnit = Yard;
            break;
        }

        if (ftValue >= 5280.0) {
            targetValue *= METER2KM * KM2MI;
            targetUnit = Mile;
            break;
        }

        targetValue *= M2FT;
        targetUnit = Foot;
        break;
    }

    case NauticalSystem: {
        targetValue *= METER2KM * KM2NM;
        targetUnit = NauticalMile;
        break;
    }

    default:
        qWarning() << Q_FUNC_INFO << "Unknown measurement system!";
    }
}

QString MarbleLocale::unitAbbreviation(MeasureUnit unit)
{
    switch (unit) {
    case Meter:
        return i18nc("means meter", "m");
    case Milimeter:
        return i18nc("means milimeters", "mm");
    case Kilometer:
        return i18nc("means kilometers", "km");
    case Centimeter:
        return i18nc("means centimeters", "cm");
    case Foot:
        return i18nc("means feet", "ft");
    case Inch:
        return i18nc("means inches", "in");
    case Yard:
        return i18nc("means yards", "yd");
    case Mile:
        return i18nc("means miles", "mi");
    case NauticalMile:
        return i18nc("means nautical miles", "nm");
    default:
        return QString::fromUtf8("");
    }
}

QString MarbleLocale::languageCode()
{
    const QString lang = QLocale::system().name();
    QString code;

    int index = lang.indexOf(QLatin1Char('_'));
    if (lang == QLatin1String("C")) {
        code = QString::fromUtf8("en");
    }
    else if ( index != -1 ) {
        code = lang.left ( index );
    }
    else {
        index = lang.indexOf(QLatin1Char('@'));
        if ( index != -1 )
            code = lang.left ( index );
        else
            code = lang;
    }

    return code;
}

}
