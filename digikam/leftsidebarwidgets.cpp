/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2000-12-05
 * Description : left sidebar widgets
 *
 * Copyright (C) 2009 by Johannes Wienke <languitar at semipol dot de>
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

#include "leftsidebarwidgets.moc"

// QT includes
#include <qbuttongroup.h>
#include <qlabel.h>
#include <qlayout.h>

// KDE includes
#include <kcombobox.h>
#include <kdebug.h>
#include <kdialog.h>
#include <kicon.h>
#include <kiconloader.h>
#include <kinputdialog.h>
#include <klocale.h>
#include <ksqueezedtextlabel.h>

// Local includes
#include "albummanager.h"
#include "albummodificationhelper.h"
#include "albumselectiontreeview.h"
#include "albumsettings.h"
#include "datefolderview.h"
#include "editablesearchtreeview.h"
#include "fuzzysearchfolderview.h"
#include "fuzzysearchview.h"
#include "searchfolderview.h"
#include "searchtabheader.h"
#include "searchtextbar.h"
#include "searchxml.h"
#include "tagfolderview.h"
#include "timelinewidget.h"

namespace Digikam
{

class AlbumFolderViewSideBarWidgetPriv
{
public:
    AlbumFolderViewSideBarWidgetPriv() :
        albumFolderView(0),
        searchTextBar(0)
    {
    }

    AlbumModificationHelper *albumModificationHelper;
    AlbumSelectionTreeView *albumFolderView;
    SearchTextBar *searchTextBar;
};

AlbumFolderViewSideBarWidget::AlbumFolderViewSideBarWidget(QWidget *parent,
                AlbumModel *model, AlbumModificationHelper *albumModificationHelper) :
    SideBarWidget(parent), d(new AlbumFolderViewSideBarWidgetPriv)
{

    d->albumModificationHelper = albumModificationHelper;

    QVBoxLayout *layout = new QVBoxLayout(this);

    d->albumFolderView = new AlbumSelectionTreeView(this, model, d->albumModificationHelper);
    d->searchTextBar   = new SearchTextBar(this, "DigikamViewFolderSearchBar");
    d->searchTextBar->setHighlightOnCompletion(true);
    d->searchTextBar->setModel(model, AbstractAlbumModel::AlbumIdRole, AbstractAlbumModel::AlbumTitleRole);

    layout->addWidget(d->albumFolderView);
    layout->addWidget(d->searchTextBar);

    // setup connection
    connect(d->searchTextBar, SIGNAL(signalSearchTextSettings(const SearchTextSettings&)),
            d->albumFolderView->albumFilterModel(), SLOT(setSearchTextSettings(const SearchTextSettings&)));

    connect(d->albumFolderView, SIGNAL(signalFindDuplicatesInAlbum(Album*)),
            this, SIGNAL(signalFindDuplicatesInAlbum(Album*)));

}

AlbumFolderViewSideBarWidget::~AlbumFolderViewSideBarWidget()
{
    delete d;
}

void AlbumFolderViewSideBarWidget::setActive(bool active)
{
    if (active)
    {
        AlbumManager::instance()->setCurrentAlbum(
                        d->albumFolderView->currentAlbum());
    }
}

void AlbumFolderViewSideBarWidget::loadViewState(KConfigGroup &group)
{
    d->albumFolderView->loadViewState(group, "AlbumFolderView");
}

void AlbumFolderViewSideBarWidget::saveViewState(KConfigGroup &group)
{
    d->albumFolderView->saveViewState(group, "AlbumFolderView");
}

void AlbumFolderViewSideBarWidget::applySettings()
{
    kDebug() << "applying settings";
    AlbumSettings *settings = AlbumSettings::instance();
    d->albumFolderView->setEnableToolTips(settings->getShowAlbumToolTips());
}

void AlbumFolderViewSideBarWidget::changeAlbumFromHistory(Album *album)
{
    d->albumFolderView->slotSelectAlbum(album);
}

void AlbumFolderViewSideBarWidget::slotSelectAlbum(Album *album)
{

    kDebug() << "received request to go to album and item";

    // Change the current album in list view.
    d->albumFolderView->slotSelectAlbum(album);

}

QPixmap AlbumFolderViewSideBarWidget::getIcon()
{
    return SmallIcon("folder-image");
}

QString AlbumFolderViewSideBarWidget::getCaption()
{
    return i18n("Albums");
}

// -----------------------------------------------------------------------------

class TagViewSideBarWidgetPriv
{
public:
    TagViewSideBarWidgetPriv() :
        tagModel(0)
    {
    }

    TagModel *tagModel;
    SearchTextBar *tagSearchBar;
    TagFolderViewNew *tagFolderView;
    TagModificationHelper *tagModificationHelper;
};

TagViewSideBarWidget::TagViewSideBarWidget(QWidget *parent,
                TagModel *model, TagModificationHelper *tagModificationHelper) :
    SideBarWidget(parent), d(new TagViewSideBarWidgetPriv)
{

    d->tagModel = model;
    d->tagModificationHelper = tagModificationHelper;

    QVBoxLayout *layout = new QVBoxLayout(this);

    d->tagFolderView = new TagFolderViewNew(this, model, tagModificationHelper);
    d->tagSearchBar  = new SearchTextBar(this, "DigikamViewTagSearchBar");
    d->tagSearchBar->setHighlightOnCompletion(true);
    d->tagSearchBar->setModel(model, AbstractAlbumModel::AlbumIdRole, AbstractAlbumModel::AlbumTitleRole);

    layout->addWidget(d->tagFolderView);
    layout->addWidget(d->tagSearchBar);

    connect(d->tagSearchBar, SIGNAL(signalSearchTextSettings(const SearchTextSettings&)),
            d->tagFolderView, SLOT(setSearchTextSettings(const SearchTextSettings&)));
    connect(d->tagFolderView, SIGNAL(signalFindDuplicatesInAlbum(Album*)),
            this, SIGNAL(signalFindDuplicatesInAlbum(Album*)));

}

TagViewSideBarWidget::~TagViewSideBarWidget()
{
    delete d;
}

void TagViewSideBarWidget::setActive(bool active)
{
    if (active)
    {
        AlbumManager::instance()->setCurrentAlbum(
                        d->tagFolderView->currentAlbum());
    }
}

void TagViewSideBarWidget::loadViewState(KConfigGroup &group)
{
    d->tagFolderView->loadViewState(group, "TagFolderView");
}

void TagViewSideBarWidget::saveViewState(KConfigGroup &group)
{
    d->tagFolderView->saveViewState(group, "TagFolderView");
}

void TagViewSideBarWidget::applySettings()
{
}

void TagViewSideBarWidget::changeAlbumFromHistory(Album *album)
{
    d->tagFolderView->slotSelectAlbum(album);
}

QPixmap TagViewSideBarWidget::getIcon()
{
    return SmallIcon("tag");
}

QString TagViewSideBarWidget::getCaption()
{
    return i18n("Tags");
}

void TagViewSideBarWidget::slotSelectAlbum(Album *album)
{
    d->tagFolderView->slotSelectAlbum(album);
}

// -----------------------------------------------------------------------------

class DateFolderViewSideBarWidgetPriv
{
public:
    DateFolderViewSideBarWidgetPriv() :
        dateFolderView(0)
    {
    }

    DateFolderView *dateFolderView;
};

DateFolderViewSideBarWidget::DateFolderViewSideBarWidget(QWidget *parent,
                DateAlbumModel *model, ImageAlbumFilterModel *imageFilterModel) :
    SideBarWidget(parent), d(new DateFolderViewSideBarWidgetPriv)
{

    QVBoxLayout *layout = new QVBoxLayout(this);

    d->dateFolderView = new DateFolderView(this);
    d->dateFolderView->setImageModel(imageFilterModel);

    layout->addWidget(d->dateFolderView);

}

DateFolderViewSideBarWidget::~DateFolderViewSideBarWidget()
{
    delete d;
}

void DateFolderViewSideBarWidget::setActive(bool active)
{
    d->dateFolderView->setActive(active);
}

void DateFolderViewSideBarWidget::loadViewState(KConfigGroup &group)
{
}

void DateFolderViewSideBarWidget::saveViewState(KConfigGroup &group)
{
}

void DateFolderViewSideBarWidget::applySettings()
{
}

void DateFolderViewSideBarWidget::changeAlbumFromHistory(Album *album)
{
    Q3ListViewItem *item = (Q3ListViewItem*) album->extraData(d->dateFolderView);
    if (!item)
        return;
    d->dateFolderView->setSelected(item);
}

void DateFolderViewSideBarWidget::gotoDate(const QDate &date)
{
    d->dateFolderView->gotoDate(date);
}

void DateFolderViewSideBarWidget::refresh()
{
    d->dateFolderView->refresh();
}

QPixmap DateFolderViewSideBarWidget::getIcon()
{
    return SmallIcon("view-calendar-list");
}

QString DateFolderViewSideBarWidget::getCaption()
{
    return i18n("Calendar");
}

// -----------------------------------------------------------------------------

class TimelineSideBarWidgetPriv
{
public:
    TimelineSideBarWidgetPriv() :
        configGroupName("TimeLine SideBar"),
        configHistogramTimeUnitEntry("Histogram TimeUnit"),
        configHistogramScaleEntry("Histogram Scale"),
        configCursorPositionEntry("Cursor Position"),

        scaleBG(0),
        cursorCountLabel(0),
        scrollBar(0),
        timer(0),
        resetButton(0),
        saveButton(0),
        timeUnitCB(0),
        nameEdit(0),
        cursorDateLabel(0),
        searchDateBar(0),
        timeLineFolderView(0),
        timeLineWidget(0),
        searchModel(0),
        searchModificationHelper(0)
    {}


    const QString       configGroupName;
    const QString       configHistogramTimeUnitEntry;
    const QString       configHistogramScaleEntry;
    const QString       configCursorPositionEntry;

    QButtonGroup*       scaleBG;
    QLabel*             cursorCountLabel;
    QScrollBar*         scrollBar;
    QTimer*             timer;
    QToolButton*        resetButton;
    QToolButton*        saveButton;

    KComboBox*          timeUnitCB;
    KLineEdit*          nameEdit;
    KSqueezedTextLabel* cursorDateLabel;

    SearchTextBar*      searchDateBar;
    EditableSearchTreeView* timeLineFolderView;
    TimeLineWidget*     timeLineWidget;

    SearchModel*        searchModel;

    SearchModificationHelper *searchModificationHelper;
};

TimelineSideBarWidget::TimelineSideBarWidget(QWidget *parent, SearchModel *searchModel,
                                             SearchModificationHelper *searchModificationHelper) :
    SideBarWidget(parent), d(new TimelineSideBarWidgetPriv)
{

    d->searchModificationHelper = searchModificationHelper;

    d->timer = new QTimer(this);
    setAttribute(Qt::WA_DeleteOnClose);

    QVBoxLayout *vlay = new QVBoxLayout(this);
    QFrame *panel     = new QFrame(this);
    panel->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    panel->setLineWidth(1);

    QGridLayout *grid = new QGridLayout(panel);

    // ---------------------------------------------------------------

    QWidget *hbox1    = new QWidget(panel);
    QHBoxLayout *hlay = new QHBoxLayout(hbox1);

    QLabel *label1 = new QLabel(i18n("Time Unit:"), hbox1);
    d->timeUnitCB  = new KComboBox(hbox1);
    d->timeUnitCB->addItem(i18n("Day"),   TimeLineWidget::Day);
    d->timeUnitCB->addItem(i18n("Week"),  TimeLineWidget::Week);
    d->timeUnitCB->addItem(i18n("Month"), TimeLineWidget::Month);
    d->timeUnitCB->addItem(i18n("Year"),  TimeLineWidget::Year);
    d->timeUnitCB->setCurrentIndex((int)TimeLineWidget::Month);
    d->timeUnitCB->setFocusPolicy(Qt::NoFocus);
    d->timeUnitCB->setWhatsThis(i18n("<p>Select the histogram time unit.</p>"
                                     "<p>You can change the graph decade to zoom in or zoom out over time.</p>"));

    QWidget *scaleBox  = new QWidget(hbox1);
    QHBoxLayout *hlay2 = new QHBoxLayout(scaleBox);
    d->scaleBG         = new QButtonGroup(scaleBox);
    d->scaleBG->setExclusive(true);
    scaleBox->setWhatsThis( i18n("<p>Select the histogram scale.</p>"
                                  "<p>If the date's maximal counts are small, you can use the linear scale.</p>"
                                  "<p>Logarithmic scale can be used when the maximal counts are big; "
                                  "if it is used, all values (small and large) will be visible on the "
                                  "graph.</p>"));

    QToolButton *linHistoButton = new QToolButton(scaleBox);
    linHistoButton->setToolTip( i18n( "Linear" ) );
    linHistoButton->setIcon(KIcon("view-object-histogram-linear"));
    linHistoButton->setCheckable(true);
    d->scaleBG->addButton(linHistoButton, TimeLineWidget::LinScale);

    QToolButton *logHistoButton = new QToolButton(scaleBox);
    logHistoButton->setToolTip( i18n( "Logarithmic" ) );
    logHistoButton->setIcon(KIcon("view-object-histogram-logarithmic"));
    logHistoButton->setCheckable(true);
    d->scaleBG->addButton(logHistoButton, TimeLineWidget::LogScale);

    hlay2->setMargin(0);
    hlay2->setSpacing(0);
    hlay2->addWidget(linHistoButton);
    hlay2->addWidget(logHistoButton);

    hlay->setMargin(0);
    hlay->setSpacing(KDialog::spacingHint());
    hlay->addWidget(label1);
    hlay->addWidget(d->timeUnitCB);
    hlay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum));
    hlay->addWidget(scaleBox);

    // ---------------------------------------------------------------

    d->timeLineWidget = new TimeLineWidget(panel);
    d->scrollBar      = new QScrollBar(panel);
    d->scrollBar->setOrientation(Qt::Horizontal);
    d->scrollBar->setMinimum(0);
    d->scrollBar->setSingleStep(1);

    d->cursorDateLabel  = new KSqueezedTextLabel(0, panel);
    d->cursorCountLabel = new QLabel(panel);
    d->cursorCountLabel->setAlignment(Qt::AlignRight);

    // ---------------------------------------------------------------

    KHBox *hbox2 = new KHBox(panel);
    hbox2->setMargin(0);
    hbox2->setSpacing(KDialog::spacingHint());

    d->resetButton = new QToolButton(hbox2);
    d->resetButton->setIcon(SmallIcon("document-revert"));
    d->resetButton->setToolTip(i18n("Clear current selection"));
    d->resetButton->setWhatsThis(i18n("If you press this button, the current date selection on the time-line will be cleared."));
    d->nameEdit    = new KLineEdit(hbox2);
    d->nameEdit->setClearButtonShown(true);
    d->nameEdit->setWhatsThis(i18n("Enter the name of the current dates search to save in the "
                                   "\"My Date Searches\" view"));

    d->saveButton  = new QToolButton(hbox2);
    d->saveButton->setIcon(SmallIcon("document-save"));
    d->saveButton->setEnabled(false);
    d->saveButton->setToolTip(i18n("Save current selection to a new virtual Album"));
    d->saveButton->setWhatsThis(i18n("If you press this button, the dates selected on the time-line will be "
                                     "saved to a new search virtual Album using the name set on the left."));

    // ---------------------------------------------------------------

    grid->addWidget(hbox1,               0, 0, 1, 4);
    grid->addWidget(d->cursorDateLabel,  1, 0, 1, 3);
    grid->addWidget(d->cursorCountLabel, 1, 3, 1, 1);
    grid->addWidget(d->timeLineWidget,   2, 0, 1, 4);
    grid->addWidget(d->scrollBar,        3, 0, 1, 4);
    grid->addWidget(hbox2,               4, 0, 1, 4);
    grid->setColumnStretch(2, 10);
    grid->setMargin(KDialog::spacingHint());
    grid->setSpacing(KDialog::spacingHint());

    // ---------------------------------------------------------------

    d->timeLineFolderView = new EditableSearchTreeView(this, searchModel,
                                                      searchModificationHelper);
    d->timeLineFolderView->albumFilterModel()->listTimelineSearches();
    d->searchDateBar      = new SearchTextBar(this, "TimeLineViewSearchDateBar");
    d->searchDateBar->setModel(searchModel, AbstractAlbumModel::AlbumIdRole,
                               AbstractAlbumModel::AlbumTitleRole);

    vlay->addWidget(panel);
    vlay->addWidget(d->timeLineFolderView);
    vlay->addItem(new QSpacerItem(KDialog::spacingHint(), KDialog::spacingHint(),
                                  QSizePolicy::Minimum, QSizePolicy::Minimum));
    vlay->addWidget(d->searchDateBar);
    vlay->setMargin(0);
    vlay->setSpacing(0);

    // ---------------------------------------------------------------

    connect(AlbumManager::instance(), SIGNAL(signalDatesMapDirty(const QMap<QDateTime, int>&)),
            d->timeLineWidget, SLOT(slotDatesMap(const QMap<QDateTime, int>&)));

    // TODO update
    //connect(d->timeLineFolderView, SIGNAL(signalAlbumSelected(SAlbum*)),
    //        this, SLOT(slotAlbumSelected(SAlbum*)));

    connect(d->searchDateBar, SIGNAL(signalSearchTextSettings(const SearchTextSettings&)),
            d->timeLineFolderView, SLOT(setSearchTextSettings(const SearchTextSettings&)));

    connect(d->timeUnitCB, SIGNAL(activated(int)),
            this, SLOT(slotTimeUnitChanged(int)));

    connect(d->scaleBG, SIGNAL(buttonReleased(int)),
            this, SLOT(slotScaleChanged(int)));

    connect(d->timeLineWidget, SIGNAL(signalDateMapChanged()),
            this, SLOT(slotInit()));

    connect(d->timeLineWidget, SIGNAL(signalCursorPositionChanged()),
            this, SLOT(slotCursorPositionChanged()));

    connect(d->timeLineWidget, SIGNAL(signalSelectionChanged()),
            this, SLOT(slotSelectionChanged()));

    connect(d->timeLineWidget, SIGNAL(signalRefDateTimeChanged()),
            this, SLOT(slotRefDateTimeChanged()));

    connect(d->timer, SIGNAL(timeout()),
            this, SLOT(slotUpdateCurrentDateSearchAlbum()));

    connect(d->resetButton, SIGNAL(clicked()),
            this, SLOT(slotResetSelection()));

    connect(d->saveButton, SIGNAL(clicked()),
            this, SLOT(slotSaveSelection()));

    connect(d->scrollBar, SIGNAL(valueChanged(int)),
            this, SLOT(slotScrollBarValueChanged(int)));

    connect(d->nameEdit, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotCheckAboutSelection()));

    connect(d->nameEdit, SIGNAL(returnPressed(const QString&)),
            d->saveButton, SLOT(animateClick()));

}

TimelineSideBarWidget::~TimelineSideBarWidget()
{
    delete d;
}

void TimelineSideBarWidget::slotInit()
{
    // Date Maps are loaded from AlbumManager to TimeLineWidget after than GUI is initialized.
    // AlbumManager query Date KIO slave to stats items from database and it can take a while.
    // We waiting than TimeLineWidget is ready before to set last config from users.

    // TODO update...
    KSharedConfig::Ptr config = KGlobal::config();
    KConfigGroup group        = config->group(d->configGroupName);
    loadViewState(group);

    disconnect(d->timeLineWidget, SIGNAL(signalDateMapChanged()),
               this, SLOT(slotInit()));

    connect(d->timeLineWidget, SIGNAL(signalDateMapChanged()),
            this, SLOT(slotCursorPositionChanged()));
}

void TimelineSideBarWidget::setActive(bool active)
{
    if (active)
    {
        AlbumManager::instance()->setCurrentAlbum(
                        d->timeLineFolderView->currentAlbum());
    }
}

// TODO update, what about these groups...
void TimelineSideBarWidget::loadViewState(KConfigGroup& /*group*/)
{
    KSharedConfig::Ptr config = KGlobal::config();
    KConfigGroup group        = config->group(d->configGroupName);

    d->timeUnitCB->setCurrentIndex(group.readEntry(d->configHistogramTimeUnitEntry, (int)TimeLineWidget::Month));
    slotTimeUnitChanged(d->timeUnitCB->currentIndex());

    int id = group.readEntry(d->configHistogramScaleEntry, (int)TimeLineWidget::LinScale);
    if ( d->scaleBG->button( id ) )
       d->scaleBG->button( id )->setChecked(true);
    slotScaleChanged(d->scaleBG->checkedId());

    QDateTime now = QDateTime::currentDateTime();
    d->timeLineWidget->setCursorDateTime(group.readEntry(d->configCursorPositionEntry, now));
    d->timeLineWidget->setCurrentIndex(d->timeLineWidget->indexForCursorDateTime());
}

void TimelineSideBarWidget::saveViewState(KConfigGroup& /*group*/)
{
    KSharedConfig::Ptr config = KGlobal::config();
    KConfigGroup group        = config->group(d->configGroupName);

    group.writeEntry(d->configHistogramTimeUnitEntry, d->timeUnitCB->currentIndex());
    group.writeEntry(d->configHistogramScaleEntry,    d->scaleBG->checkedId());
    group.writeEntry(d->configCursorPositionEntry,    d->timeLineWidget->cursorDateTime());
    group.sync();
}

void TimelineSideBarWidget::applySettings()
{
    // nothing to do here right now
}

void TimelineSideBarWidget::changeAlbumFromHistory(Album *album)
{
    d->timeLineFolderView->slotSelectAlbum(album);
}

QPixmap TimelineSideBarWidget::getIcon()
{
    return SmallIcon("player-time");
}

QString TimelineSideBarWidget::getCaption()
{
    return i18n("Timeline");
}

void TimelineSideBarWidget::slotRefDateTimeChanged()
{
    d->scrollBar->blockSignals(true);
    d->scrollBar->setMaximum(d->timeLineWidget->totalIndex()-1);
    d->scrollBar->setValue(d->timeLineWidget->indexForRefDateTime()-1);
    d->scrollBar->blockSignals(false);
}

void TimelineSideBarWidget::slotTimeUnitChanged(int mode)
{
    d->timeLineWidget->setTimeUnit((TimeLineWidget::TimeUnit)mode);
}

void TimelineSideBarWidget::slotScrollBarValueChanged(int val)
{
    d->timeLineWidget->setCurrentIndex(val);
}

void TimelineSideBarWidget::slotScaleChanged(int mode)
{
    d->timeLineWidget->setScaleMode((TimeLineWidget::ScaleMode)mode);
}

void TimelineSideBarWidget::slotCursorPositionChanged()
{
    QString txt;
    int val = d->timeLineWidget->cursorInfo(txt);
    d->cursorDateLabel->setText(txt);
    d->cursorCountLabel->setText(QString::number(val));
}

void TimelineSideBarWidget::slotSelectionChanged()
{
    d->timer->setSingleShot(true);
    d->timer->start(100);
}

/** Called from d->timer event.*/
void TimelineSideBarWidget::slotUpdateCurrentDateSearchAlbum()
{
    slotCheckAboutSelection();
    int totalCount = 0;
    DateRangeList dateRanges = d->timeLineWidget->selectedDateRange(totalCount);
    d->searchModificationHelper->slotCreateTimeLineSearch(SAlbum::getTemporaryTitle(DatabaseSearch::TimeLineSearch),
                                                          dateRanges, true);
}

void TimelineSideBarWidget::slotSaveSelection()
{
    QString name = d->nameEdit->text();
    int totalCount = 0;
    DateRangeList dateRanges = d->timeLineWidget->selectedDateRange(totalCount);
    d->searchModificationHelper->slotCreateTimeLineSearch(name, dateRanges);
}

void TimelineSideBarWidget::slotAlbumSelected(SAlbum* salbum)
{
    if (!salbum)
    {
        slotResetSelection();
        return;
    }

    SearchXmlReader reader(salbum->query());

    // The timeline query consists of groups, with two date time fields each
    DateRangeList list;
    while (!reader.atEnd())
    {
        // read groups
        if (reader.readNext() == SearchXml::Group)
        {
            QDateTime start, end;
            int numberOfFields = 0;
            while (!reader.atEnd())
            {
                // read fields
                reader.readNext();

                if (reader.isEndElement())
                    break;

                if (reader.isFieldElement())
                {
                    if (numberOfFields == 0)
                        start = reader.valueToDateTime();
                    else if (numberOfFields == 1)
                        end = reader.valueToDateTime();
                    ++numberOfFields;
                }
            }
            if (numberOfFields)
                list << DateRange(start, end);
        }
    }

    d->timeLineWidget->setSelectedDateRange(list);
    AlbumManager::instance()->setCurrentAlbum(salbum);
}

void TimelineSideBarWidget::slotResetSelection()
{
    d->timeLineWidget->slotResetSelection();
    slotCheckAboutSelection();
    AlbumManager::instance()->setCurrentAlbum(0);
}

void TimelineSideBarWidget::slotCheckAboutSelection()
{
    int totalCount     = 0;
    DateRangeList list = d->timeLineWidget->selectedDateRange(totalCount);
    if (!list.isEmpty())
    {
        d->nameEdit->setEnabled(true);

        if (!d->nameEdit->text().isEmpty())
            d->saveButton->setEnabled(true);
    }
    else
    {
        d->nameEdit->setEnabled(false);
        d->saveButton->setEnabled(false);
    }
}

// -----------------------------------------------------------------------------

class SearchSideBarWidgetPriv
{
public:
    SearchSideBarWidgetPriv() :
        searchSearchBar(0),
        searchFolderView(0),
        searchTabHeader(0)
    {
    }

    SearchTextBar*            searchSearchBar;
    SearchFolderView*         searchFolderView;
    SearchTabHeader*          searchTabHeader;
    SearchModel *searchModel;
};

SearchSideBarWidget::SearchSideBarWidget(QWidget *parent, SearchModel *searchModel) :
    SideBarWidget(parent), d(new SearchSideBarWidgetPriv)
{

    d->searchModel = searchModel;

    QVBoxLayout *layout = new QVBoxLayout(this);

    d->searchTabHeader  = new SearchTabHeader(this);
    d->searchFolderView = new SearchFolderView(this);
    d->searchSearchBar  = new SearchTextBar(this, "DigikamViewSearchSearchBar");
    d->searchSearchBar->setModel(searchModel, AbstractAlbumModel::AlbumIdRole, AbstractAlbumModel::AlbumTitleRole);

    layout->addWidget(d->searchTabHeader);
    layout->addWidget(d->searchFolderView);
    layout->addWidget(d->searchSearchBar);

    connect(d->searchSearchBar, SIGNAL(signalSearchTextSettings(const SearchTextSettings&)),
            d->searchFolderView, SLOT(slotTextSearchFilterChanged(const SearchTextSettings&)));

    connect(d->searchFolderView, SIGNAL(newSearch()),
            d->searchTabHeader, SLOT(newAdvancedSearch()));

    connect(d->searchFolderView, SIGNAL(editSearch(SAlbum *)),
            d->searchTabHeader, SLOT(editSearch(SAlbum *)));

    connect(d->searchFolderView, SIGNAL(selectedSearchChanged(SAlbum *)),
            d->searchTabHeader, SLOT(selectedSearchChanged(SAlbum *)));

    connect(d->searchTabHeader, SIGNAL(searchShallBeSelected(SAlbum *)),
            d->searchFolderView, SLOT(slotSelectSearch(SAlbum *)));

}

SearchSideBarWidget::~SearchSideBarWidget()
{
    delete d;
}

void SearchSideBarWidget::setActive(bool active)
{
}

void SearchSideBarWidget::loadViewState(KConfigGroup &group)
{
}

void SearchSideBarWidget::saveViewState(KConfigGroup &group)
{
}

void SearchSideBarWidget::applySettings()
{
}

void SearchSideBarWidget::changeAlbumFromHistory(Album *album)
{
    Q3ListViewItem *item = (Q3ListViewItem*) album->extraData(d->searchFolderView);
    if (!item)
        return;

    d->searchFolderView->setSelected(item, true);
    d->searchFolderView->ensureItemVisible(item);
}

QPixmap SearchSideBarWidget::getIcon()
{
    return SmallIcon("edit-find");
}

QString SearchSideBarWidget::getCaption()
{
    return i18n("Searches");
}

void SearchSideBarWidget::newKeywordSearch()
{
    d->searchTabHeader->newKeywordSearch();
}

void SearchSideBarWidget::newAdvancedSearch()
{
    d->searchTabHeader->newAdvancedSearch();
}

// -----------------------------------------------------------------------------

class FuzzySearchSideBarWidgetPriv
{
public:
    FuzzySearchSideBarWidgetPriv() :
        fuzzySearchView(0)
    {
    }

    FuzzySearchView*          fuzzySearchView;
    SearchModel *searchModel;
};

FuzzySearchSideBarWidget::FuzzySearchSideBarWidget(QWidget *parent, SearchModel *searchModel) :
    SideBarWidget(parent), d(new FuzzySearchSideBarWidgetPriv)
{

    d->searchModel = searchModel;

    d->fuzzySearchView  = new FuzzySearchView(this);
    d->fuzzySearchView->searchBar()->setModel(searchModel, AbstractAlbumModel::AlbumIdRole, AbstractAlbumModel::AlbumTitleRole);

    QVBoxLayout *layout = new QVBoxLayout(this);

    layout->addWidget(d->fuzzySearchView);

    connect(d->fuzzySearchView, SIGNAL(signalUpdateFingerPrints()),
            this, SIGNAL(signalUpdateFingerPrints()));
    connect(d->fuzzySearchView, SIGNAL(signalGenerateFingerPrintsFirstTime()),
            this, SIGNAL(signalGenerateFingerPrintsFirstTime()));

}

FuzzySearchSideBarWidget::~FuzzySearchSideBarWidget()
{
    delete d;
}

void FuzzySearchSideBarWidget::setActive(bool active)
{
}

void FuzzySearchSideBarWidget::loadViewState(KConfigGroup &group)
{
}

void FuzzySearchSideBarWidget::saveViewState(KConfigGroup &group)
{
}

void FuzzySearchSideBarWidget::applySettings()
{
}

void FuzzySearchSideBarWidget::changeAlbumFromHistory(Album *album)
{
    Q3ListViewItem *item = (Q3ListViewItem*) album->extraData(
                    d->fuzzySearchView->folderView());
    if (!item)
        return;

    d->fuzzySearchView->folderView()->setSelected(item, true);
    d->fuzzySearchView->folderView()->ensureItemVisible(item);
}

QPixmap FuzzySearchSideBarWidget::getIcon()
{
    return SmallIcon("tools-wizard");
}

QString FuzzySearchSideBarWidget::getCaption()
{
    return i18n("Fuzzy Searches");
}

void FuzzySearchSideBarWidget::newDuplicatesSearch(Album *album)
{
    d->fuzzySearchView->newDuplicatesSearch(album);
}

void FuzzySearchSideBarWidget::newSimilarSearch(const ImageInfo &imageInfo)
{

    if (imageInfo.isNull())
    {
        return;
    }

    d->fuzzySearchView->setImageInfo(imageInfo);

}

// -----------------------------------------------------------------------------

#ifdef HAVE_MARBLEWIDGET

class GPSSearchSideBarWidgetPriv
{
public:
    GPSSearchSideBarWidgetPriv() :
        gpsSearchView(0)
    {
    }

    GPSSearchView*            gpsSearchView;
    SearchModel *searchModel;
};

GPSSearchSideBarWidget::GPSSearchSideBarWidget(QWidget *parent, SearchModel *searchModel) :
    SideBarWidget(parent), d(new GPSSearchSideBarWidgetPriv)
{

    d->searchModel = searchModel;

    d->gpsSearchView    = new GPSSearchView(this);
    d->gpsSearchView->searchBar()->setModel(searchModel, AbstractAlbumModel::AlbumIdRole, AbstractAlbumModel::AlbumTitleRole);

    QVBoxLayout *layout = new QVBoxLayout(this);

    layout->addWidget(d->gpsSearchView);

    connect(d->gpsSearchView, SIGNAL(signalMapSelectedItems(const KUrl::List)),
            this, SIGNAL(signalMapSelectedItems(const KUrl::List&)));

    connect(d->gpsSearchView, SIGNAL(signalMapSoloItems(const KUrl::List, const QString&)),
            this, SIGNAL(signalMapSoloItems(const KUrl::List, const QString&)));

}

GPSSearchSideBarWidget::~GPSSearchSideBarWidget()
{
    delete d;
}

void GPSSearchSideBarWidget::setActive(bool active)
{
}

void GPSSearchSideBarWidget::loadViewState(KConfigGroup &group)
{
}

void GPSSearchSideBarWidget::saveViewState(KConfigGroup &group)
{
}

void GPSSearchSideBarWidget::applySettings()
{
}

void GPSSearchSideBarWidget::changeAlbumFromHistory(Album *album)
{
}

QPixmap GPSSearchSideBarWidget::getIcon()
{
    return SmallIcon("applications-internet");
}

QString GPSSearchSideBarWidget::getCaption()
{
    return i18n("Map Searches");
}

void GPSSearchSideBarWidget::slotDigikamViewNoCurrentItem()
{
    d->gpsSearchView->slotDigikamViewNoCurrentItem();
}

// TODO what are the variable names?
void GPSSearchSideBarWidget::slotDigikamViewImageSelected(const ImageInfoList &selectedImage, bool hasPrevious, bool hasNext, const ImageInfoList &allImages)
{
    d->gpsSearchView->slotDigikamViewImageSelected(selectedImage, hasPrevious, hasNext, allImages);
}

#endif

}
