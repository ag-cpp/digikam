/* ============================================================
 * File  : imageproperties.cpp
 * Author: Caulier Gilles <caulier dot gilles at free.fr>
 * Date  : 2004-11-17
 * Description :
 *
 * Copyright 2003 by Gilles Caulier
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

// Qt includes. 
 
#include <qlayout.h>
#include <qcolor.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qvgroupbox.h>
#include <qlabel.h>
#include <qtextedit.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qwhatsthis.h>
#include <qtooltip.h>
#include <qdatetime.h>

// KDE includes.

#include <kmimetype.h>
#include <kfileitem.h>
#include <klocale.h>
#include <kselect.h>
#include <kdialogbase.h>
#include <kdeversion.h>
#include <kapplication.h>
#include <kcursor.h>
#include <kseparator.h> 
#include <ksqueezedtextlabel.h>
#include <kglobal.h>

// LibKexif includes.

#include <libkexif/kexififd.h>
#include <libkexif/kexifentry.h>
#include <libkexif/kexifdata.h>
#include <libkexif/kexiflistview.h>
#include <libkexif/kexiflistviewitem.h>

// Local includes.

#include "imagehistogram.h"
#include "histogramwidget.h"
#include "colorgradientwidget.h"

#include "albumfolderview.h"
#include "albumfolderitem.h"
#include "albumiconview.h"
#include "albumiconitem.h"
#include "albumlister.h"
#include "albummanager.h"
#include "albumdb.h"
#include "album.h"
#include "albumsettings.h"

#include "imageproperties.h"


ImageProperties::ImageProperties(AlbumIconView* view, AlbumIconItem* currItem)
               : KDialogBase(Tabbed, i18n("Image Properties and Meta-Data"), 
                             Help|User1|User2|Stretch|Close,
                             Ok, view, 0, true, true, 
                             KStdGuiItem::guiItem(KStdGuiItem::Forward), 
                             KStdGuiItem::guiItem(KStdGuiItem::Back))
{
    parentWidget()->setCursor( KCursor::waitCursor() );
    
    // Main init.
    
    setHelp("propertiesmetadatahistogram.anchor", "digikam");
    m_view     = view;
    m_currItem = currItem;
    m_lister   = view->albumLister();
    KURL fileURL(m_currItem->fileItem()->url());
    
    //General tab init.
    
    setupGeneralTab(fileURL.path());
    
    // Exif Viewer init.
    
    mExifData = new KExifData;

    if (mExifData->readFromFile(fileURL.path()))
        setupExifViewer();
        
    // HistoGramViewer init.
    
    m_histogramWidget = 0L;
    m_hGradient       = 0L;
    m_image.load(fileURL.path());
    
    if (m_image.isNull() == false)
       {
       if(m_image.depth() < 32)                 // we works always with 32bpp.
          m_image = m_image.convertDepth(32);
       
       m_image.setAlphaBuffer(true);
       setupHistogramViewer((uint *)m_image.bits(), m_image.width(), m_image.height(), fileURL);
       }

    enableButton(User1, m_currItem->nextItem() != 0);
    enableButton(User2, m_currItem->prevItem() != 0);
       
    parentWidget()->setCursor( KCursor::arrowCursor() );       
}

ImageProperties::~ImageProperties()
{
   
    kapp->config()->setGroup("Image Properties Dialog");
//    kapp->config()->writeEntry("Auto Save", m_autoSaveBox->isChecked());

    saveDialogSize("Image Properties Dialog");
    
    // For Exif viewer.
    
    if (mExifData)
        delete mExifData;
    
    // For histogram viever.
    
    if ( m_histogramWidget )
       delete m_histogramWidget;
    
    if ( m_hGradient )        
       delete m_hGradient;
}

void ImageProperties::slotUser1()
{
    if (!m_currItem)
        return;

    m_currItem = dynamic_cast<AlbumIconItem*>(m_currItem->nextItem());
    slotItemChanged();
}

void ImageProperties::slotUser2()
{
    if (!m_currItem)
        return;
    
    m_currItem = dynamic_cast<AlbumIconItem*>(m_currItem->prevItem());
    slotItemChanged();
}

void ImageProperties::slotItemChanged()
{
    parentWidget()->setCursor( KCursor::waitCursor() );
    
    if (!m_currItem)
        return;

    if (!m_HistogramThumbJob.isNull())
    {
        m_HistogramThumbJob->kill();
    }

    if (!m_HistogramThumbJob.isNull())
    {
        delete m_HistogramThumbJob;
    }

    if (!m_generalThumbJob.isNull())
    {
        m_generalThumbJob->kill();
    }

    if (!m_generalThumbJob.isNull())
    {
        delete m_generalThumbJob;
    }

    KURL fileURL(m_currItem->fileItem()->url());    
 
    // Update General tab.
    
    m_generalThumbJob = new ThumbnailJob(fileURL, 128);
    
    connect(m_generalThumbJob,
            SIGNAL(signalThumbnailMetaInfo(const KURL&,
                                           const QPixmap&,
                                           const KFileMetaInfo*)),
            SLOT(slotGotGeneralThumbnail(const KURL&,
                                         const QPixmap&,
                                         const KFileMetaInfo*)));
                                         
    // Update Exif Viewer tab.
       
    m_listview->clear();
    
    if (mExifData)
        delete mExifData;
    
    mExifData = new KExifData;
    
    if (mExifData->readFromFile(fileURL.path()))
        {
        QPtrList<KExifIfd> ifdList(mExifData->ifdList());

        for (KExifIfd* ifd = ifdList.first(); ifd; ifd = ifdList.next())
            {
            KExifListViewItem *item = new KExifListViewItem(m_listview, m_listview->lastItem(),
                                                            ifd->getName());
            m_listview->addItems(ifd->entryList(), item );
            }
        
        // Exif embedded thumbnails creation.
    
        QImage thumbnail(mExifData->getThumbnail());
        
        if (!thumbnail.isNull()) 
            {
            m_exifThumb->setFixedSize(thumbnail.size());
            m_exifThumb->setPixmap(QPixmap(thumbnail));
            }        
    
        QToolTip::add( m_listview, i18n("Select an item to see its description"));
        }
       
    // Update Histogram Viewer tab.
    
    m_HistogramThumbJob = new ThumbnailJob(fileURL, 48);
    
    connect(m_HistogramThumbJob,
            SIGNAL(signalThumbnailMetaInfo(const KURL&,
                                           const QPixmap&,
                                           const KFileMetaInfo*)),
            SLOT(slotGotHistogramThumbnail(const KURL&,
                                           const QPixmap&,
                                           const KFileMetaInfo*)));
    
    m_image.load(fileURL.path());
    
    if (m_image.isNull() == false)
       {
       if(m_image.depth() < 32)                 // we works always with 32bpp.
          m_image = m_image.convertDepth(32);
       
       m_image.setAlphaBuffer(true);
       m_histogramWidget->updateData((uint *)m_image.bits(), m_image.width(), m_image.height());
       }
        
    /*PAlbum *album = m_lister->findParentAlbum(m_currItem->fileItem());
    if (!album)
    {
        kdWarning() << "Failed to find parent album for"
                    << m_currItem->fileItem()->url().prettyURL()
                    << endl;
        return;
    }
    
    AlbumDB* db  = AlbumManager::instance()->albumDB();

    m_nameLabel->setText(fileURL.fileName());
    m_thumbLabel->setPixmap(QPixmap());
    m_commentsEdit->setText(db->getItemCaption(album, fileURL.fileName()));*/

    enableButton(User1, m_currItem->nextItem() != 0);
    enableButton(User2, m_currItem->prevItem() != 0);
    parentWidget()->setCursor( KCursor::arrowCursor() );       
}


//-----------------------------------------------------------------------------------------------------------
// Histogram Viewer implementation methods

void ImageProperties::setupHistogramViewer(uint *imageData, uint width, uint height, KURL fileURL)
{
    QFrame *page = addPage( i18n("&Histogram"));
   
    QVBoxLayout *topLayout = new QVBoxLayout( page, 0, spacingHint() );

    // -------------------------------------------------------------
                                              
    QHBoxLayout *hlay = new QHBoxLayout(topLayout);
    
    m_histogramThumb = new QLabel( page );
    m_histogramThumb->setFixedHeight( 48 );
    hlay->addWidget(m_histogramThumb);
    
    m_HistogramThumbJob = new ThumbnailJob(fileURL, 48);
    
    connect(m_HistogramThumbJob,
            SIGNAL(signalThumbnailMetaInfo(const KURL&,
                                           const QPixmap&,
                                           const KFileMetaInfo*)),
            SLOT(slotGotHistogramThumbnail(const KURL&,
                                           const QPixmap&,
                                           const KFileMetaInfo*)));
    
    QGridLayout *grid = new QGridLayout(hlay, 2, 4);
    
    QLabel *label1 = new QLabel(i18n("Channel:"), page);
    label1->setAlignment ( Qt::AlignRight | Qt::AlignVCenter );
    m_channelCB = new QComboBox( false, page );
    m_channelCB->insertItem( i18n("Luminosity") );
    m_channelCB->insertItem( i18n("Red") );
    m_channelCB->insertItem( i18n("Green") );
    m_channelCB->insertItem( i18n("Blue") );
    m_channelCB->insertItem( i18n("Alpha") );
    m_channelCB->insertItem( i18n("Colors") );
    m_channelCB->setCurrentText( i18n("Luminosity") );
    QWhatsThis::add( m_channelCB, i18n("<p>Select here the histogram channel to display:<p>"
                                       "<b>Luminosity</b>: drawing the image luminosity values.<p>"
                                       "<b>Red</b>: drawing the red image channel values.<p>"
                                       "<b>Green</b>: drawing the green image channel values.<p>"
                                       "<b>Blue</b>: drawing the blue image channel values.<p>"
                                       "<b>Alpha</b>: drawing the alpha image channel values. " 
                                       "This channel corresponding to the transparency value and "
                                       "is supported by some image formats such as PNG or GIF.<p>"
                                       "<b>Colors</b>: drawing all color channels values at the same time."));
    
    QLabel *label2 = new QLabel(i18n("Scale:"), page);
    label2->setAlignment ( Qt::AlignRight | Qt::AlignVCenter);
    m_scaleCB = new QComboBox( false, page );
    m_scaleCB->insertItem( i18n("Linear") );
    m_scaleCB->insertItem( i18n("Logarithmic") );
    m_scaleCB->setCurrentText( i18n("Logarithmic") );
    QWhatsThis::add( m_scaleCB, i18n("<p>Select here the histogram scale.<p>"
                                     "If the image maximal counts is small, you can use the linear scale.<p>"
                                     "Logarithmic scale can be used when the maximal counts is big. "
                                     "Like this all values (small and big) will be visible on the graph."));
    
    QLabel *label10 = new QLabel(i18n("Colors:"), page);
    label10->setAlignment ( Qt::AlignRight | Qt::AlignVCenter );
    m_colorsCB = new QComboBox( false, page );
    m_colorsCB->insertItem( i18n("Red") );
    m_colorsCB->insertItem( i18n("Green") );
    m_colorsCB->insertItem( i18n("Blue") );
    m_colorsCB->setCurrentText( i18n("Red") );
    m_colorsCB->setEnabled( false );
    QWhatsThis::add( m_colorsCB, i18n("<p>Select here the main color displayed with Colors Channel mode:<p>"
                                       "<b>Red</b>: drawing the red image channel on the foreground.<p>"
                                       "<b>Green</b>: drawing the green image channel on the foreground.<p>"
                                       "<b>Blue</b>: drawing the blue image channel on the foreground.<p>"));
                                     
    grid->addWidget(label1, 0, 0);
    grid->addWidget(m_channelCB, 0, 1);
    grid->addWidget(label2, 0, 2);
    grid->addWidget(m_scaleCB, 0, 3);
    grid->addWidget(label10, 1, 0);
    grid->addWidget(m_colorsCB, 1, 1);
    
    // -------------------------------------------------------------
    
    QFrame *frame = new QFrame( page );
    frame->setFrameStyle(QFrame::Panel|QFrame::Sunken);
    QVBoxLayout* l = new QVBoxLayout(frame, 5, 0);
    
    m_histogramWidget = new Digikam::HistogramWidget(256, 140, 
                                                     imageData,
                                                     width,
                                                     height,
                                                     frame);
    QWhatsThis::add( m_histogramWidget, i18n("<p>This is the histogram drawing of the "
                                             "selected image channel"));
    l->addWidget(m_histogramWidget, 0);
        
    m_hGradient = new Digikam::ColorGradientWidget( KSelector::Horizontal, 20, page );
    m_hGradient->setColors( QColor( "black" ), QColor( "white" ) );
    topLayout->addWidget(frame, 4);
    topLayout->addWidget(m_hGradient, 0);

    // -------------------------------------------------------------

    QHBoxLayout *hlay2 = new QHBoxLayout(topLayout);
    QLabel *label3 = new QLabel(i18n("Intensity range:"), page);
    label3->setAlignment ( Qt::AlignLeft | Qt::AlignVCenter);
    m_minInterv = new QSpinBox(0, 255, 1, page);
    m_minInterv->setValue(0);
    QWhatsThis::add( m_minInterv, i18n("<p>Select here the minimal intensity "
                                       "value of the histogram selection."));    
    m_maxInterv = new QSpinBox(0, 255, 1, page);
    m_maxInterv->setValue(255);
    QWhatsThis::add( m_minInterv, i18n("<p>Select here the maximal intensity value "
                                       "of the histogram selection."));
    hlay2->addWidget(label3);
    hlay2->addWidget(m_minInterv);
    hlay2->addWidget(m_maxInterv);
    
    // -------------------------------------------------------------
    
    QGroupBox *gbox = new QGroupBox(4, Qt::Horizontal, i18n("Statistics"), page);
    QWhatsThis::add( gbox, i18n("<p>You can see here the statistic results calculated with the "
                                "selected histogram part. These values are available for all "
                                "channels."));
                                
    QLabel *label4 = new QLabel(i18n("Mean:"), gbox);
    label4->setAlignment ( Qt::AlignRight | Qt::AlignVCenter);
    m_labelMeanValue = new QLabel(gbox);
    m_labelMeanValue->setAlignment ( Qt::AlignLeft | Qt::AlignVCenter);
    
    QLabel *label5 = new QLabel(i18n("Pixels:"), gbox);
    label5->setAlignment ( Qt::AlignRight | Qt::AlignVCenter);
    m_labelPixelsValue = new QLabel(gbox);
    m_labelPixelsValue->setAlignment ( Qt::AlignLeft | Qt::AlignVCenter);
    
    QLabel *label6 = new QLabel(i18n("Standard deviation:"), gbox);
    label6->setAlignment ( Qt::AlignRight | Qt::AlignVCenter);
    m_labelStdDevValue = new QLabel(gbox);
    m_labelStdDevValue->setAlignment ( Qt::AlignLeft | Qt::AlignVCenter);

    QLabel *label7 = new QLabel(i18n("Count:"), gbox);
    label7->setAlignment ( Qt::AlignRight | Qt::AlignVCenter);
    m_labelCountValue = new QLabel(gbox);
    m_labelCountValue->setAlignment ( Qt::AlignLeft | Qt::AlignVCenter);
    
    QLabel *label8 = new QLabel(i18n("Median:"), gbox);
    label8->setAlignment ( Qt::AlignRight | Qt::AlignVCenter);
    m_labelMedianValue = new QLabel(gbox);
    m_labelMedianValue->setAlignment ( Qt::AlignLeft | Qt::AlignVCenter);
    
    QLabel *label9 = new QLabel(i18n("Percentile:"), gbox);
    label9->setAlignment ( Qt::AlignRight | Qt::AlignVCenter);
    m_labelPercentileValue = new QLabel(gbox);
    m_labelPercentileValue->setAlignment ( Qt::AlignLeft | Qt::AlignVCenter);
    
    topLayout->addWidget(gbox);
    topLayout->addStretch();

    // -------------------------------------------------------------
    
    connect(m_channelCB, SIGNAL(activated(int)),
            this, SLOT(slotChannelChanged(int)));
    
    connect(m_scaleCB, SIGNAL(activated(int)),
            this, SLOT(slotScaleChanged(int)));
    
    connect(m_colorsCB, SIGNAL(activated(int)),
            this, SLOT(slotColorsChanged(int)));            
 
    connect(m_histogramWidget, SIGNAL(signalMousePressed( int )),
            this, SLOT(slotUpdateMinInterv(int)));
       
    connect(m_histogramWidget, SIGNAL(signalMouseReleased( int )),
            this, SLOT(slotUpdateMaxInterv(int)));

    connect(m_minInterv, SIGNAL(valueChanged (int)),
            m_histogramWidget, SLOT(slotMinValueChanged(int)));

    connect(m_minInterv, SIGNAL(valueChanged (int)),
            this, SLOT(slotIntervChanged(int)));

    connect(m_maxInterv, SIGNAL(valueChanged (int)),
            m_histogramWidget, SLOT(slotMaxValueChanged(int)));

    connect(m_maxInterv, SIGNAL(valueChanged (int)),
            this, SLOT(slotIntervChanged(int)));
    
    updateInformations(); 
}

void ImageProperties::slotChannelChanged(int channel)
{
    switch(channel)
       {
       case 1:           // Red.
          m_histogramWidget->m_channelType = Digikam::HistogramWidget::RedChannelHistogram;
          m_hGradient->setColors( QColor( "black" ), QColor( "red" ) );
          m_colorsCB->setEnabled(false);
          break;
       
       case 2:           // Green.
          m_histogramWidget->m_channelType = Digikam::HistogramWidget::GreenChannelHistogram;
          m_hGradient->setColors( QColor( "black" ), QColor( "green" ) );
          m_colorsCB->setEnabled(false);
          break;
          
       case 3:           // Blue.
          m_histogramWidget->m_channelType = Digikam::HistogramWidget::BlueChannelHistogram;
          m_hGradient->setColors( QColor( "black" ), QColor( "blue" ) );
          m_colorsCB->setEnabled(false);
          break;

       case 4:           // Alpha.
          m_histogramWidget->m_channelType = Digikam::HistogramWidget::AlphaChannelHistogram;
          m_hGradient->setColors( QColor( "black" ), QColor( "white" ) );
          m_colorsCB->setEnabled(false);
          break;
          
       case 5:           // All color channels.
          m_histogramWidget->m_channelType = Digikam::HistogramWidget::ColorChannelsHistogram;
          m_hGradient->setColors( QColor( "black" ), QColor( "white" ) );
          m_colorsCB->setEnabled(true);
          break;
                              
       default:          // Luminosity.
          m_histogramWidget->m_channelType = Digikam::HistogramWidget::ValueHistogram;
          m_hGradient->setColors( QColor( "black" ), QColor( "white" ) );
          m_colorsCB->setEnabled(false);
          break;
       }
   
    m_histogramWidget->repaint(false);
    updateInformations();
}

void ImageProperties::slotScaleChanged(int scale)
{
    switch(scale)
       {
       case 1:           // Log.
          m_histogramWidget->m_scaleType = Digikam::HistogramWidget::LogScaleHistogram;
          break;
          
       default:          // Lin.
          m_histogramWidget->m_scaleType = Digikam::HistogramWidget::LinScaleHistogram;
          break;
       }
   
    m_histogramWidget->repaint(false);
}

void ImageProperties::slotColorsChanged(int color)
{
    switch(color)
       {
       case 1:           // Green.
          m_histogramWidget->m_colorType = Digikam::HistogramWidget::GreenColor;
          break;
       
       case 2:           // Blue.
          m_histogramWidget->m_colorType = Digikam::HistogramWidget::BlueColor;
          break;

       default:          // Red.
          m_histogramWidget->m_colorType = Digikam::HistogramWidget::RedColor;
          break;
       }

    m_histogramWidget->repaint(false);
    updateInformations();
}

void ImageProperties::slotUpdateMinInterv(int min)
{
    m_minInterv->setValue(min);
}


void ImageProperties::slotUpdateMaxInterv(int max)
{
    m_maxInterv->setValue(max);
    updateInformations();
}

void ImageProperties::slotIntervChanged(int)
{
    m_maxInterv->setMinValue(m_minInterv->value());
    m_minInterv->setMaxValue(m_maxInterv->value());
    updateInformations();
}

void ImageProperties::updateInformations()
{
    QString value;
    int min = m_minInterv->value();
    int max = m_maxInterv->value();
    int channel = m_channelCB->currentItem();

    if ( channel == Digikam::HistogramWidget::ColorChannelsHistogram )
       channel = m_colorsCB->currentItem()+1;
               
    double mean = m_histogramWidget->m_imageHistogram->getMean(channel, min, max);
    m_labelMeanValue->setText(value.setNum(mean, 'f', 1));
    
    double pixels = m_histogramWidget->m_imageHistogram->getPixels();
    m_labelPixelsValue->setText(value.setNum((float)pixels, 'f', 0));
    
    double stddev = m_histogramWidget->m_imageHistogram->getStdDev(channel, min, max);
    m_labelStdDevValue->setText(value.setNum(stddev, 'f', 1));
      
    double counts = m_histogramWidget->m_imageHistogram->getCount(channel, min, max);
    m_labelCountValue->setText(value.setNum((float)counts, 'f', 0));
    
    double median = m_histogramWidget->m_imageHistogram->getMedian(channel, min, max);
    m_labelMedianValue->setText(value.setNum(median, 'f', 1)); 

    double percentile = (pixels > 0 ? (100.0 * counts / pixels) : 0.0);
    m_labelPercentileValue->setText(value.setNum(percentile, 'f', 1));
}


void ImageProperties::slotGotHistogramThumbnail(const KURL&, const QPixmap& pix,
                                                const KFileMetaInfo*)
{
    m_histogramThumb->setPixmap(pix);
}

//-----------------------------------------------------------------------------------------------------------
// Exif Viewer implementation methods

void ImageProperties::setupExifViewer(void)
{
    QFrame *page = addPage( i18n("Exif") );
    QGridLayout* layout = new QGridLayout(page);
    m_listview = new KExifListView(page, true);
    
    QGroupBox *box = new QVGroupBox(i18n("Embedded thumnail"), page);
    m_exifThumb = new QLabel(box);
    
    layout->addWidget(m_listview, 0, 0);
    layout->addWidget(box, 1, 0);
        
    QPtrList<KExifIfd> ifdList(mExifData->ifdList());

    for (KExifIfd* ifd = ifdList.first(); ifd; ifd = ifdList.next())
        {
        KExifListViewItem *item = new KExifListViewItem(m_listview, m_listview->lastItem(),
                                                        ifd->getName());
        m_listview->addItems(ifd->entryList(), item );
        }
        
    // Exif embedded thumbnails creation.
    
    QImage thumbnail(mExifData->getThumbnail());
        
    if (!thumbnail.isNull()) 
        {
        m_exifThumb->setFixedSize(thumbnail.size());
        m_exifThumb->setPixmap(QPixmap(thumbnail));
        }        
}

//-----------------------------------------------------------------------------------------------------------
// General Tab implementation methods

void ImageProperties::setupGeneralTab(KURL fileURL)
{
    QFrame *page = addPage( i18n("General") );
    QVBoxLayout *vlay = new QVBoxLayout( page, 0, spacingHint() );
    
    m_generalThumb = new QLabel( page );
    m_generalThumb->setFixedHeight( 128 );
    vlay->addWidget(m_generalThumb, 0, Qt::AlignHCenter);
    
    m_generalThumbJob = new ThumbnailJob(fileURL, 128);
    
    connect(m_generalThumbJob,
            SIGNAL(signalThumbnailMetaInfo(const KURL&,
                                           const QPixmap&,
                                           const KFileMetaInfo*)),
            SLOT(slotGotGeneralThumbnail(const KURL&,
                                         const QPixmap&,
                                         const KFileMetaInfo*)));
    
    KSeparator *sep1 = new KSeparator (Horizontal, page);
    vlay->addWidget(sep1);
    
    QHBoxLayout *hlay1 = new QHBoxLayout();
    vlay->addLayout( hlay1 );
    QLabel *name = new QLabel( i18n("Name:"), page);
    m_filename = new KSqueezedTextLabel( fileURL.fileName(), page);
    name->setBuddy( m_filename );
    hlay1->addWidget( name );
    hlay1->addStretch( 1 );
    hlay1->addWidget( m_filename );
    
    QHBoxLayout *hlay2 = new QHBoxLayout();
    vlay->addLayout( hlay2 );
    QLabel *type = new QLabel( i18n("Type:"), page);
    m_filetype = new KSqueezedTextLabel( KMimeType::findByURL(fileURL)->name(), page);
    type->setBuddy( m_filetype );
    hlay2->addWidget( type );
    hlay2->addStretch( 1 );
    hlay2->addWidget( m_filetype );

    QHBoxLayout *hlay3 = new QHBoxLayout();
    vlay->addLayout( hlay3 );
    QLabel *path = new QLabel( i18n("Location:"), page);
    m_filepath = new KSqueezedTextLabel( fileURL.path(), page);
    path->setBuddy( m_filepath );
    hlay3->addWidget( path );
    hlay3->addStretch( 1 );
    hlay3->addWidget( m_filepath );
    
    const KFileItem* fi = m_currItem->fileItem();    
    
    QHBoxLayout *hlay4 = new QHBoxLayout();
    vlay->addLayout( hlay4 );
    QLabel *date = new QLabel( i18n("Modification Date:"), page);
    QDateTime dateurl;
    dateurl.setTime_t(fi->time(KIO::UDS_MODIFICATION_TIME));
    m_filedate = new KSqueezedTextLabel( KGlobal::locale()->formatDateTime(dateurl, true, true),
                                         page);
    date->setBuddy( m_filedate );
    hlay4->addWidget( date );
    hlay4->addStretch( 1 );
    hlay4->addWidget( m_filedate );
    
    QHBoxLayout *hlay5 = new QHBoxLayout();
    vlay->addLayout( hlay5 );
    QLabel *size = new QLabel( i18n("Size:"), page);
    m_filesize = new KSqueezedTextLabel( i18n("%1 (%2)")
                                         .arg(KIO::convertSize(fi->size()))
                                         .arg(KGlobal::locale()->formatNumber(fi->size(), 0)),
                                         page);
    size->setBuddy( m_filesize );
    hlay5->addWidget( size );
    hlay5->addStretch( 1 );
    hlay5->addWidget( m_filesize );

    QHBoxLayout *hlay6 = new QHBoxLayout();
    vlay->addLayout( hlay6 );
    QLabel *owner = new QLabel( i18n("Owner:"), page);
    m_fileowner = new KSqueezedTextLabel( i18n("%1 - %2)")
                                         .arg(fi->user())
                                         .arg(fi->group()),
                                         page);
    owner->setBuddy( m_fileowner );
    hlay6->addWidget( owner );
    hlay6->addStretch( 1 );
    hlay6->addWidget( m_fileowner );

    QHBoxLayout *hlay7 = new QHBoxLayout();
    vlay->addLayout( hlay7 );
    QLabel *permissions = new QLabel( i18n("Permissions:"), page);
    m_filepermissions = new KSqueezedTextLabel( fi->permissionsString(),
                                                page);
    permissions->setBuddy( m_filepermissions );
    hlay7->addWidget( permissions );
    hlay7->addStretch( 1 );
    hlay7->addWidget( m_filepermissions );
        
    KSeparator *sep2 = new KSeparator (Horizontal, page);
    vlay->addWidget(sep2);
    
    vlay->addStretch(1);
}

void ImageProperties::slotGotGeneralThumbnail(const KURL&, const QPixmap& pix,
                                              const KFileMetaInfo*)
{
    m_generalThumb->setPixmap(pix);
}

#include "imageproperties.moc"

