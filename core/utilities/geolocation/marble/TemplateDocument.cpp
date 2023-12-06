// SPDX-FileCopyrightText: 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "TemplateDocument.h"

#include <QMap>
#include <QString>
#include <QFile>
#include <QRegExp>

#include "digikam_debug.h"

namespace Marble
{

class TemplateDocumentPrivate
{
public:
    TemplateDocumentPrivate()
    {
    }
    QString templateText;
    QMap<QString, QString> templateEntries;
    static void processTemplateIncludes(QString &input);
};

void TemplateDocumentPrivate::processTemplateIncludes(QString &input)
{
    QRegExp rx(QString::fromUtf8("%!\\{([^}]*)\\}%"));

    QStringList includes;
    int pos = 0;

    while ((pos = rx.indexIn(input, pos)) != -1) {
        includes << rx.cap(1);
        pos += rx.matchedLength();
    }

    for (const QString &include: includes) {
        QFile includeFile(QLatin1String(":/htmlfeatures/includes/") + include + QLatin1String(".inc"));
        if (includeFile.open(QIODevice::ReadOnly)) {
            input.replace(QLatin1String("%!{") + include + QLatin1String("}%"), QString::fromUtf8(includeFile.readAll()));
        } else {
            qCDebug(DIGIKAM_MARBLE_LOG) << "[WARNING] Can't process template include" << include;
        }
    }
}

TemplateDocument::TemplateDocument() :
    d(new TemplateDocumentPrivate)
{
}

TemplateDocument::TemplateDocument(const QString &templateText) :
    d(new TemplateDocumentPrivate)
{
    setTemplate(templateText);
}

TemplateDocument::~TemplateDocument()
{
    delete d;
}

void TemplateDocument::setTemplate(const QString &newTemplateText)
{
    d->templateText = newTemplateText;
}

QString TemplateDocument::value(const QString &key) const
{
    return d->templateEntries[key];
}

void TemplateDocument::setValue(const QString &key, const QString &value)
{
    d->templateEntries[key] = value;
}

QString& TemplateDocument::operator[](const QString &key)
{
    return d->templateEntries[key];
}

QString TemplateDocument::finalText() const
{
    QString ready = d->templateText;
    typedef QMap<QString, QString>::ConstIterator ConstIterator;
    ConstIterator end = d->templateEntries.constEnd();
    for (ConstIterator i = d->templateEntries.constBegin(); i != end; i++) {
        ready.replace(QLatin1Char('%') + i.key() + QLatin1Char('%'), i.value());
    }
    d->processTemplateIncludes(ready);
    return ready;
}

} // namespace Marble
