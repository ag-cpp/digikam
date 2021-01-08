/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-10-15
 * Description : IPTC subjects editor.
 *
 * Copyright (C) 2006-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009-2012 by Andi Clemens <andi dot clemens at googlemail dot com>
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

#ifndef DIGIKAM_SUBJECT_WIDGET_H
#define DIGIKAM_SUBJECT_WIDGET_H

// Qt includes

#include <QByteArray>
#include <QMap>
#include <QStringList>
#include <QUrl>
#include <QWidget>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT SubjectData
{
public:

    SubjectData(const QString& n, const QString& m, const QString& d)
      : name(n),
        matter(m),
        detail(d)
    {
    }

    QString name;         ///< English and Name of subject.
    QString matter;       ///< English and Matter Name of subject.
    QString detail;       ///< English and Detail Name of subject.
};

// --------------------------------------------------------------------------------

class DIGIKAM_EXPORT SubjectWidget : public QWidget
{
    Q_OBJECT

public:

    explicit SubjectWidget(QWidget* const parent, bool sizesLimited = false);
    ~SubjectWidget() override;

    void setSubjectsList(const QStringList& list);
    QStringList subjectsList() const;

Q_SIGNALS:

    void signalModified();

protected Q_SLOTS:

    virtual void slotSubjectsToggled(bool);
    virtual void slotRefChanged();
    virtual void slotEditOptionChanged(int);
    virtual void slotSubjectSelectionChanged();
    virtual void slotAddSubject();
    virtual void slotDelSubject();
    virtual void slotRepSubject();

protected:

    virtual bool loadSubjectCodesFromXML(const QUrl& url);
    virtual QString buildSubject() const;

protected:

    QLabel*    m_note;

    QCheckBox* m_subjectsCheck;

    QLineEdit* m_iprEdit;
    QLineEdit* m_refEdit;
    QLineEdit* m_nameEdit;
    QLineEdit* m_matterEdit;
    QLineEdit* m_detailEdit;

    QString    m_iprDefault;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_SUBJECT_WIDGET_H
