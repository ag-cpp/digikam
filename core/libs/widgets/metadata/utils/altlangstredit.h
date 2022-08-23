/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-06-15
 * Description : multi-languages string editor
 *
 * Copyright (C) 2009-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_ALT_LANG_STR_EDIT_H
#define DIGIKAM_ALT_LANG_STR_EDIT_H

// Qt includes

#include <QWidget>
#include <QString>
#include <QStringList>
#include <QMap>

// Local includes

#include "digikam_export.h"
#include "dmetadata.h"
#include "dtextedit.h"

class QListWidgetItem;

namespace Digikam
{

class DIGIKAM_EXPORT AltLangStrEdit : public QWidget
{
    Q_OBJECT

public:

    /**
     * Default contructor. Use lines to use a specific number of lines with text editor.
     */
    explicit AltLangStrEdit(QWidget* const parent, unsigned int lines = 3);
    ~AltLangStrEdit()                               override;

    /**
     * Create a title widget with a QLabel and relevant text.
     * If a title widget already exists, it's remplaced.
     */
    void setTitle(const QString& title);

    /**
     * Create a title with a specific widget instance (aka a QCheckBox for ex).
     * If a title widget already exists, it's remplaced.
     */
    void setTitleWidget(QWidget* const twdg);

    /**
     * Return the current title widget instance.
     * If no previous call of setTitle() or setWidgetTitle(), this function will return nullptr.
     */
    QWidget* titleWidget()                   const;

    void setPlaceholderText(const QString& msg);

    void    setCurrentLanguageCode(const QString& lang);
    QString currentLanguageCode()            const;

    QString languageCode(int index)          const;

    /**
     * Fix lines visibile in text editor to lines. If zero, do not fix layout to number of lines visible.
     */
    void setLinesVisible(uint lines);
    uint linesVisible()                      const;

    QString defaultAltLang()                 const;
    bool    asDefaultAltLang()               const;

    /**
     * Reset widget, clear all entries
     */
    void reset();

    /**
     * Ensure that the current language is added to the list of entries,
     * even if the text is empty.
     * signalValueAdded() will be emitted.
     */
    void addCurrent();

    DTextEdit* textEdit()                    const;

    MetaEngine::AltLangMap& values()         const;

    virtual void setValues(const MetaEngine::AltLangMap& values);

    /**
     * Return the litteral name of RFC 3066 language code (format FR-fr for ex).
     */
    static QString languageNameRFC3066(const QString& code);

    /**
     * Return all language codes available following the RFC 3066.
     */
    static QStringList allLanguagesRFC3066();

Q_SIGNALS:

    /**
     * Emitted when the user changes the text for the current language.
     */
    void signalModified(const QString& lang, const QString& text);

    /**
     * Emitted when the current language changed.
     */
    void signalSelectionChanged(const QString& lang);

    /**
     * Emitted when an entry for a new language is added.
     */
    void signalValueAdded(const QString& lang, const QString& text);

    /**
     * Emitted when the entry for a language is removed.
     */
    void signalValueDeleted(const QString& lang);

protected Q_SLOTS:

    void slotTextChanged();
    void slotSelectionChanged();
    void slotDeleteValue();

    /**
     * Can be used to turn on/off visibility of internal widgets.
     * This do not includes the title widget.
     */
    void slotEnabledInternalWidgets(bool);

private Q_SLOTS:

    /**
     * Perform text translation with Web-service.
     */
    void slotTranslate(QListWidgetItem*);

    void slotTranslationFinished();

    void slotLocalizeChanged();

    void slotOpenLocalizeSetup();

protected:

    void populateLangAltListEntries();
    void populateTranslationEntries();

    void changeEvent(QEvent* e)                 override;

private:

    typedef QMap<QString, QString> LanguageCodeMap;
    static const LanguageCodeMap s_rfc3066ForXMP;

    class Private;
    Private* const d;

    friend class Private;
};

} // namespace Digikam

#endif // DIGIKAM_ALT_LANG_STR_EDIT_H
