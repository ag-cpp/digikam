/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-11-25
 * Description : a bar used to search a string - version not based on database models
 *
 * Copyright (C) 2007-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_SEARCH_TEXT_BAR_H
#define DIGIKAM_SEARCH_TEXT_BAR_H

// Qt includes

#include <QAbstractItemModel>
#include <QStringList>
#include <QLineEdit>

// Local includes

#include "digikam_export.h"
#include "modelcompleter.h"
#include "statesavingobject.h"

namespace Digikam
{

class DIGIKAM_EXPORT SearchTextSettings
{

public:

    SearchTextSettings()
      : caseSensitive(Qt::CaseInsensitive)
    {
    }

    Qt::CaseSensitivity caseSensitive;

    QString             text;
};

bool DIGIKAM_EXPORT operator==(const SearchTextSettings& a, const SearchTextSettings& b);

/**
 * A text input for searching entries with visual feedback.
 * Can be used on QAbstractItemModels.
 */
class DIGIKAM_EXPORT SearchTextBar : public QLineEdit,
                                     public StateSavingObject
{
    Q_OBJECT

public:

    /**
     * Possible highlighting states a SearchTextBar can have.
     */
    enum HighlightState
    {
        /**
         * No highlighting at all. Background is colored in a neutral way
         * according to the theme.
         */
        NEUTRAL,

        /**
         * The background color of the text input indicates that a result was
         * found.
         */
        HAS_RESULT,

        /**
         * The background color indicates that no result was found.
         */
        NO_RESULT
    };

public:

    explicit SearchTextBar(QWidget* const parent,
                           const QString& name,
                           const QString& msg = QString());
    ~SearchTextBar()                                  override;

    void setTextQueryCompletion(bool b);
    bool hasTextQueryCompletion()               const;

    /**
     * Tells whether highlighting for found search results shall be used or not
     * (green and red).
     *
     * Default behavior has highlighting enabled.
     *
     * @param highlight <code>true</code> activates green and red highlighting,
     *                  with <code>false</code> the normal widget background
     *                  color will be displayed always
     */
    void setHighlightOnResult(bool highlight);

    /**
     * Tells the current highlighting state of the text input indicated via the
     * background color.
     *
     * @return current highlight state
     */
    HighlightState getCurrentHighlightState()   const;

    /**
     * Indicate whether this search text bar can be toggled to between case-
     * sensitive and -insensitive or if always case-insensitive shall be
     * used.
     *
     * @param b if <code>true</code> the user can decide the toggle between
     *          case sensitivity, on <code>false</code> every search is case-
     *          insensitive
     */
    void setCaseSensitive(bool b);
    bool hasCaseSensitive()                     const;

    void setSearchTextSettings(const SearchTextSettings& settings);
    SearchTextSettings searchTextSettings()     const;
    ModelCompleter*  completerModel()           const;

Q_SIGNALS:

    void signalSearchTextSettings(const SearchTextSettings& settings);
    void completerHighlighted(int albumId);
    void completerActivated();

public Q_SLOTS:

    void slotSearchResult(bool match);

private Q_SLOTS:

    void slotTextChanged();

protected:

    void doLoadState()                                override;
    void doSaveState()                                override;

private:

    void contextMenuEvent(QContextMenuEvent* e)       override;

    /**
     * If hasCaseSensitive returns <code>true</code> this tells the search
     * text bar whether to ignore case or not.
     *
     * @param ignore if <code>true</code>, case is ignored in the emitted
     *               search text settings and the completion
     */
    void setIgnoreCase(bool ignore);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_SEARCH_TEXT_BAR_H
