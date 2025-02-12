/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-01-20
 * Description : User interface for searches
 *
 * SPDX-FileCopyrightText: 2008-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2012-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "searchfieldgroup.h"

// Qt includes

#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>

// Local includes

#include "searchfields.h"
#include "searchgroup.h"
#include "searchutilities.h"
#include "thememanager.h"

namespace Digikam
{

SearchFieldGroup::SearchFieldGroup(SearchGroup* const parent)
    : QWidget(parent)
{
    m_layout     = new QGridLayout;
    setLayout(m_layout);
    m_controller = new VisibilityController(this);
    m_controller->setContainerWidget(this);
}

void SearchFieldGroup::addField(SearchField* const field)
{
    // FIXME: When all fields added in searchgroup.cpp are implemented, remove

    if (!field)
    {
        return;
    }

    // initialize widgets in field

    field->setup(m_layout);

    // take care that category labels are not duplicated in two subsequent rows

    if (!m_fields.isEmpty())
    {
        field->setCategoryLabelVisibleFromPreviousField(m_fields.last());
    }

    // add to our list

    m_fields << field;

    // add to visibility controller

    m_controller->addObject(field);
}

void SearchFieldGroup::setLabel(SearchFieldGroupLabel* const label)
{
    m_label = label;

    connect(m_label, SIGNAL(clicked()),
            this, SLOT(slotLabelClicked()));
}

SearchField* SearchFieldGroup::fieldForName(const QString& fieldName) const
{
    for (SearchField* const field : std::as_const(m_fields))
    {
        if (field->supportsField(fieldName))
        {    // cppcheck-suppress useStlAlgorithm
            return field;
        }
    }

    return nullptr;
}

void SearchFieldGroup::write(SearchXmlWriter& writer)
{
    for (SearchField* const field : std::as_const(m_fields))
    {
        field->write(writer);
    }
}

void SearchFieldGroup::reset()
{
    clearMarkedFields();

    for (SearchField* const field : std::as_const(m_fields))
    {
        field->reset();
    }
}

void SearchFieldGroup::setFieldsVisible(bool visible)
{
    m_controller->setVisible(visible);
}

void SearchFieldGroup::slotLabelClicked()
{
    m_controller->triggerVisibility();
}

void SearchFieldGroup::markField(SearchField* const field)
{
    m_markedFields << field;
}

void SearchFieldGroup::clearMarkedFields()
{
    m_markedFields.clear();
}

QList<QRect> SearchFieldGroup::areaOfMarkedFields() const
{
    QList<QRect> rects;

    if (!m_controller->isVisible())
    {
        return rects;
    }

    for (SearchField* const field : std::as_const(m_markedFields))
    {
        if (field->isVisible())
        {
            // cppcheck-suppress useStlAlgorithm
            rects += field->widgetRects(SearchField::ValueWidgetRectsOnly);
        }
    }

    // adjust position relative to parent

    for (QList<QRect>::iterator it = rects.begin() ; it != rects.end() ; ++it)
    {
        (*it).translate(pos());
    }

    return rects;
}

// -------------------------------------------------------------------------

SearchFieldGroupLabel::SearchFieldGroupLabel(QWidget* const parent)
    : QWidget(parent)
{
    QGridLayout* const layout = new QGridLayout;

    m_titleLabel              = new DClickLabel;
    m_titleLabel->setObjectName(QLatin1String("SearchFieldGroupLabel_Label"));
    m_expandLabel             = new QLabel;
    QFrame* const hline       = new QFrame;
    hline->setFrameStyle(QFrame::HLine | QFrame::Raised);

    layout->addWidget(m_titleLabel,  0, 0);
    layout->addWidget(m_expandLabel, 0, 1);
    layout->addWidget(hline,         1, 0, 1, 3);
    layout->setColumnStretch(2, 1);
    layout->setSpacing(2);
    setLayout(layout);

    connect(m_titleLabel, SIGNAL(activated()),
            this, SIGNAL(clicked()));
}

void SearchFieldGroupLabel::setTitle(const QString& title)
{
    m_title = title;
    m_titleLabel->setText(title);
}

void SearchFieldGroupLabel::displayExpanded()
{
}

void SearchFieldGroupLabel::displayFolded()
{
}

} // namespace Digikam

#include "moc_searchfieldgroup.cpp"
