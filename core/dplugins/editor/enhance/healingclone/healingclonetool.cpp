/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-06-15
 * Description : a tool to replace part of the image using another
 *
 * Copyright (C) 2004-2019 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2017      by Shaza Ismail Kaoud <shaza dot ismail dot k at gmail dot com>
 * Copyright (C) 2019      by Ahmed Fathi <ahmed dot fathi dot abdelmageed at gmail dot com>
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

#include "healingclonetool.h"

// Qt includes

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include<QGroupBox>
#include <QIcon>
#include <QPoint>

// KDE includes

#include <ksharedconfig.h>
#include <klocalizedstring.h>

// Local includes

#include "dexpanderbox.h"
#include "dnuminput.h"
#include "editortoolsettings.h"
#include "imageiface.h"
#include "imageguidewidget.h"
#include "imagebrushguidewidget.h"
#include "redeyecorrectioncontainer.h"
#include "redeyecorrectionfilter.h"

namespace DigikamEditorHealingCloneToolPlugin
{

class Q_DECL_HIDDEN HealingCloneTool::Private
{

public:

    explicit Private()
      : radiusInput(0),
        blurPercent(0),
        previewWidget(0),
        gboxSettings(0),
        srcButton(0)
    {
    }

    static const QString configGroupName;
    static const QString configRadiusAdjustmentEntry;
    static const QString configBlurAdjustmentEntry;

    DIntNumInput*           radiusInput;
    DDoubleNumInput*        blurPercent;
    ImageBrushGuideWidget*  previewWidget;
    EditorToolSettings*     gboxSettings;
    QPoint                  sourcePoint;
    QPoint                  destinationStartPoint;
    QPushButton*            srcButton;
    QPushButton*            lassoButton;
    QPushButton*            moveButton;
    QPushButton*            undoCloneButton;
    QPushButton*            redoCloneButton;
};

const QString HealingCloneTool::Private::configGroupName(QLatin1String("Healing Clone Tool"));
const QString HealingCloneTool::Private::configRadiusAdjustmentEntry(QLatin1String("RadiusAdjustment"));
const QString HealingCloneTool::Private::configBlurAdjustmentEntry(QLatin1String("BlurAdjustment"));

// --------------------------------------------------------

const QSize btnSize = QSize(50, 50);
const QSize iconSize = QSize(30,30);

// --------------------------------------------------------

HealingCloneTool::HealingCloneTool(QObject* const parent)
    : EditorTool(parent),
      d(new Private)
{
    setObjectName(QLatin1String("healing clone"));
    setToolHelp(QLatin1String("healingclonetool.anchor"));

    d->gboxSettings      = new EditorToolSettings(0);
    d->previewWidget     = new ImageBrushGuideWidget;
    refreshImage(); 
    d->previewWidget->setFocusPolicy(Qt::StrongFocus);
    setToolView(d->previewWidget);
    setPreviewModeMask(PreviewToolBar::PreviewTargetImage);

    // --------------------------------------------------------

    QLabel* const label  = new QLabel(i18n("Brush Radius:"));
    d->radiusInput       = new DIntNumInput();
    d->radiusInput->setRange(0, 100, 1);
    d->radiusInput->setDefaultValue(10);
    d->radiusInput->setWhatsThis(i18n("A radius of 0 has no effect, "
                                      "1 and above determine the brush radius "
                                      "that determines the size of parts copied in the image. \nShortcut :: [ and ]"));
    d->radiusInput->setToolTip(i18n("A radius of 0 has no effect, "
                                      "1 and above determine the brush radius "
                                      "that determines the size of parts copied in the image. \nShortcut :: [ and ]"));

    d->previewWidget->setBrushRadius(d->radiusInput->value());

    // --------------------------------------------------------

    QLabel* const label2 = new QLabel(i18n("Radial Blur Percent:"));
    d->blurPercent       = new DDoubleNumInput();
    d->blurPercent->setRange(0, 100, 0.1);
    d->blurPercent->setDefaultValue(0);
    d->blurPercent->setWhatsThis(i18n("A percent of 0 has no effect, values "
                                      "above 0 represent a factor for mixing "
                                      "the destination color with source color "
                                      "this is done radially i.e. the inner part of "
                                      "the brush radius is totally from source and mixing "
                                      "with destination is done gradually till the outer part "
                                      "of the circle."));

    // --------------------------------------------------------


    QPixmap pixmap_SRC(QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                        QLatin1String("digikam/data/healing_clone_SRC.png")));
    const QIcon ButtonIcon_SRC(pixmap_SRC);
    d->srcButton  = new QPushButton();
    d->srcButton->setFixedSize(btnSize);
    d->srcButton->setIcon(ButtonIcon_SRC);
    d->srcButton->setIconSize(iconSize);
    d->srcButton->setWhatsThis(i18n("Select Source Point. \nShortcut :: S"));
    d->srcButton->setToolTip(i18n("Select Source Point. \nShortcut :: S"));

    // --------------------------------------------------------

    QPixmap pixmap_LASSO(QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                        QLatin1String("digikam/data/healing_clone_LASSO.png")));
    const QIcon ButtonIcon_LASSO(pixmap_LASSO);
    d->lassoButton  = new QPushButton();
    d->lassoButton->setFixedSize(btnSize);
    d->lassoButton->setIcon(ButtonIcon_LASSO);
    d->lassoButton->setIconSize(iconSize);
    d->lassoButton->setWhatsThis(i18n("LASSO/POLYGON SELECT. \nShortcut :: L\n"
                                      "To Continue polygon, either press L or double click\n"
                                      "To Cancel, press ESC"));
    d->lassoButton->setToolTip(i18n("LASSO/POLYGON SELECT. \nShortcut :: L\n"
                                    "To Continue polygon, either press L or double click\n"
                                    "To Cancel, press ESC"));

    // --------------------------------------------------------

    QPixmap pixmap_MOVE(QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                        QLatin1String("digikam/data/healing_clone_MOVE.png")));
    const QIcon ButtonIcon_MOVE(pixmap_MOVE);
    d->moveButton  = new QPushButton();
    d->moveButton->setFixedSize(btnSize);
    d->moveButton->setIcon(ButtonIcon_MOVE);
    d->moveButton->setIconSize(iconSize);
    d->moveButton->setWhatsThis(i18n("Move Image. \nShortcut :: M"));
    d->moveButton->setToolTip(i18n("Move Image. \nShortcut :: M"));

    // --------------------------------------------------------

    QPixmap pixmap_UNDO(QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                        QLatin1String("digikam/data/healing_clone_UNDO.png")));
    const QIcon ButtonIcon_UNDO(pixmap_UNDO);
    d->undoCloneButton  = new QPushButton();
    d->undoCloneButton->setFixedSize(btnSize);
    d->undoCloneButton->setIcon(ButtonIcon_UNDO);
    d->undoCloneButton->setIconSize(iconSize);
    d->undoCloneButton->setWhatsThis(i18n("UNDO CLONE. \nShortcut :: CTRL+Z"));
    d->undoCloneButton->setToolTip(i18n("UNDO CLONE. \nShortcut :: CTRL+Z"));

    // --------------------------------------------------------

    QPixmap pixmap_REDO(QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                        QLatin1String("digikam/data/healing_clone_REDO.png")));
    const QIcon ButtonIcon_REDO(pixmap_REDO);
    d->redoCloneButton  = new QPushButton();
    d->redoCloneButton->setFixedSize(btnSize);
    d->redoCloneButton->setIcon(ButtonIcon_REDO);
    d->redoCloneButton->setIconSize(iconSize);
    d->redoCloneButton->setWhatsThis(i18n("REDO CLONE. \nShortcut :: CTRL+Y"));
    d->redoCloneButton->setToolTip(i18n("REDO CLONE. \nShortcut :: CTRL+Y"));

    const int spacing              = d->gboxSettings->spacingHint();

    QGridLayout* const grid        = new QGridLayout();
    // Tool Buttons
    QGroupBox* const iconsGroupBox = new QGroupBox();
    QHBoxLayout* const iconsHBox   = new QHBoxLayout();
    iconsHBox->setSpacing(0);
    iconsHBox->addWidget(d->srcButton);
    iconsHBox->addWidget(d->lassoButton);
    iconsHBox->addWidget(d->moveButton);
    iconsHBox->addWidget(d->undoCloneButton);
    iconsHBox->addWidget(d->redoCloneButton);
    iconsGroupBox->setLayout(iconsHBox);

    grid->addWidget(iconsGroupBox);
    grid->addWidget(new DLineWidget(Qt::Horizontal, d->gboxSettings->plainPage()), 3, 0, 1, 2);
    grid->addWidget(label,          4, 0, 1, 2);
    grid->addWidget(d->radiusInput, 5, 0, 1, 2);
    grid->addWidget(label2,         6, 0, 1, 2);
    grid->addWidget(d->blurPercent, 7, 0, 1, 2);
    grid->setRowStretch(10, 10);
    grid->setContentsMargins(spacing, spacing, spacing, spacing);
    grid->setSpacing(spacing);

    d->gboxSettings->plainPage()->setLayout(grid);

    // --------------------------------------------------------

    setPreviewModeMask(PreviewToolBar::PreviewTargetImage);
    setToolSettings(d->gboxSettings);
    setToolView(d->previewWidget);

    // --------------------------------------------------------

    this->lassoColors.push_back(DColor(Qt::red));
    this->lassoColors.push_back(DColor(Qt::white));
    this->lassoColors.push_back(DColor(Qt::black));
    this->lassoColors.push_back(DColor(Qt::yellow));
    this->lassoColors.push_back(DColor(Qt::blue));
    this->lassoColors.push_back(DColor(Qt::yellow));

    // --------------------------------------------------------

    connect(d->radiusInput, SIGNAL(valueChanged(int)),
            this, SLOT(slotRadiusChanged(int)));

    connect(d->srcButton, SIGNAL(clicked(bool)),
            d->previewWidget, SLOT(slotSetSourcePoint()));

    connect(d->moveButton, SIGNAL(clicked(bool)),
            d->previewWidget, SLOT(slotMoveImage()));

    connect(d->lassoButton, SIGNAL(clicked(bool)),
            d->previewWidget, SLOT(slotLassoSelect()));

    connect(d->undoCloneButton, SIGNAL(clicked(bool)),
            this, SLOT(slotUndoClone()));

    connect(d->redoCloneButton, SIGNAL(clicked(bool)),
            this, SLOT(slotRedoClone()));

    connect(d->previewWidget, SIGNAL(signalClone(QPoint,QPoint)),
            this, SLOT(slotReplace(QPoint,QPoint)));

    connect(d->previewWidget, SIGNAL(signalResized()),
            this, SLOT(slotResized()));

    connect(d->previewWidget, SIGNAL(signalLasso(QPoint)),
            this, SLOT(slotLasso(QPoint)));

    connect(d->previewWidget, SIGNAL(signalResetLassoPoint()),
            this, SLOT(slotResetLassoPoints()));

    connect(d->previewWidget,SIGNAL(signalContinuePolygon()),
            this, SLOT(slotContinuePolygon()));

    connect(d->previewWidget,SIGNAL(signalIncreaseBrushRadius()),
            this, SLOT(slotIncreaseBrushRadius()));

    connect(d->previewWidget,SIGNAL(signalDecreaseBrushRadius()),
            this, SLOT(slotDecreaseBrushRadius()));

    // undo - redo
    connect(d->previewWidget,SIGNAL(signalPushToUndoStack()),
            this, SLOT(slotPushToUndoStack()));

    connect(d->previewWidget,SIGNAL(signalUndoClone()),
            this, SLOT(slotUndoClone()));

    connect(d->previewWidget,SIGNAL(signalRedoClone()),
            this, SLOT(slotRedoClone()));
}

HealingCloneTool::~HealingCloneTool()
{
    delete d;
}

void HealingCloneTool::readSettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(d->configGroupName);
    d->radiusInput->setValue(group.readEntry(d->configRadiusAdjustmentEntry, d->radiusInput->defaultValue()));
    d->blurPercent->setValue(group.readEntry(d->configBlurAdjustmentEntry, d->blurPercent->defaultValue()));
}

void HealingCloneTool::writeSettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(d->configGroupName);
    group.writeEntry(d->configRadiusAdjustmentEntry, d->radiusInput->value());
    group.writeEntry(d->configBlurAdjustmentEntry, d->blurPercent->value());
    config->sync();
}

void HealingCloneTool::finalRendering()
{
    ImageIface iface;
    DImg dest = d->previewWidget->getOriginalImage();
    FilterAction action(QLatin1String("digikam:healingCloneTool"), 1);
    iface.setOriginal(i18n("healingClone"), action, dest);
}

void HealingCloneTool::slotResetSettings()
{
    d->radiusInput->blockSignals(true);
    d->radiusInput->slotReset();
    d->radiusInput->blockSignals(false);
}

void HealingCloneTool::slotResized()
{
    toolView()->update();
}

void HealingCloneTool::slotReplace(const QPoint& srcPoint, const QPoint& dstPoint)
{
    DImg  current = d->previewWidget->getOriginalImage();
    clone(&current, srcPoint, dstPoint, d->radiusInput->value());
    qCDebug(DIGIKAM_DIMG_LOG())<< "src,dst inside tool ::" <<  srcPoint << dstPoint;
}

void HealingCloneTool::slotRadiusChanged(int r)
{
    d->previewWidget->setBrushRadius(r);
}

void HealingCloneTool::clone(DImg* const img, const QPoint& srcPoint, const QPoint& dstPoint, int radius)
{
    ImageRegionItem* const item = (ImageRegionItem *) d->previewWidget->item();
    double scale                = item->zoomSettings()->zoomFactor();
    radius                      = radius / scale;
    double blurPercent          = d->blurPercent->value() / 100;

    for (int i = -1 * radius ; i < radius ; ++i)
    {
        for (int j = -1 * radius ; j < radius ; ++j)
        {
            int rPercent = (i * i) + (j * j);

            if (rPercent < (radius * radius)) // Check for inside the circle
            {
                if (srcPoint.x()+i < 0 || srcPoint.x()+i >= (int)img->width()  ||
                    srcPoint.y()+j < 0 || srcPoint.y()+j >= (int)img->height() ||
                    dstPoint.x()+i < 0 || dstPoint.x()+i >= (int)img->width()  ||
                    dstPoint.y()+j < 0 || dstPoint.y()+j >= (int)img->height())
                {
                    continue;
                }

                DColor cSrc = img->getPixelColor(srcPoint.x() + i, srcPoint.y() + j);

                if (insideLassoOperation && !this->lassoPoints.empty())
                {
                    if (this->lassoFlags.at(dstPoint.x()+i).at(dstPoint.y()+j))
                    {
                        continue;
                    }

                    bool isInside = this->lassoPolygon.containsPoint(QPoint(dstPoint.x()+i,dstPoint.y()+j),
                                                                     Qt::OddEvenFill);

                     if (!isInside)
                     {
                         continue;
                     }

                     if (this->lassoFlags.at(srcPoint.x() + i).at(srcPoint.y() + j))
                     {
                         cSrc = this->lassoColorsMap[std::make_pair(srcPoint.x() + i,srcPoint.y() + j)];
                     }
                }

                double rP   = blurPercent * rPercent / (radius * radius);
                DColor cDst = img->getPixelColor(dstPoint.x() + i, dstPoint.y() + j);

                cSrc.multiply(1 - rP);
                cDst.multiply(rP);
                cSrc.blendAdd(cDst);
                img->setPixelColor(dstPoint.x()+i, dstPoint.y()+j, cSrc);
                this->d->previewWidget->setCloneVectorChanged(true);
            }
        }
    }

    d->previewWidget->updateImage(*img);
}

void HealingCloneTool::updateLasso(std::vector<QPoint>& points)
{
    uint radius              = 5;
    static uint colorCounter = 0;
    DImg img                 = d->previewWidget->getOriginalImage();

    for (QPoint p: points)
    {
        for (uint i = 0 ; i < radius ; i++)
        {
            for (uint j = 0; j < radius ; j++)
            {
                uint x_shifted = p.x()+i;
                uint y_shifted = p.y()+j;
                DColor c       = img.getPixelColor(x_shifted,y_shifted);

                this->lassoColorsMap.insert(std::make_pair(std::make_pair(x_shifted,y_shifted), c)) ;
                img.setPixelColor(x_shifted, y_shifted, this->lassoColors[(colorCounter) % this->lassoColors.size()]);
                this->lassoFlags.at(x_shifted).at(y_shifted) = true;
                colorCounter++;
            }
        }
    }

    d->previewWidget->updateImage(img);
}

void HealingCloneTool::slotLasso(const QPoint& dst)
{
    if (this->resetLassoPoint)
    {
        this->previousLassoPoint = dst;
        this->resetLassoPoint    = false;
        this->startLassoPoint    = dst;
    }

    std::vector<QPoint> points = interpolate(this->previousLassoPoint, dst);
    this->lassoPoints.push_back(dst);
    this->previousLassoPoint   = dst;
    this->updateLasso(points);
    this->d->previewWidget->setIsLassoPointsVectorEmpty(this->lassoPoints.empty());
}

std::vector<QPoint> HealingCloneTool::interpolate(const QPoint& start, const QPoint& end)
{
    std::vector<QPoint> points;
    points.push_back(start);
    QPointF distanceVec = QPoint(end.x()-start.x() , end.y() - start.y());
    double distance     = sqrt(distanceVec.x() * distanceVec.x() + distanceVec.y() * distanceVec.y());

    //creating a unit vector
    distanceVec.setX(distanceVec.x()/distance);
    distanceVec.setY(distanceVec.y()/distance);
    int steps = (int) distance ;

    for (int i = 0 ; i<steps ; i++)
    {
        points.push_back(QPoint(start.x() + i*distanceVec.x() ,start.y() + i*distanceVec.y()));
    }

    points.push_back(end);

    return points;
}

void HealingCloneTool::removeLassoPixels()
{
    DImg img = d->previewWidget->getOriginalImage();
    std::map<std::pair<int,int>, DColor>::iterator it;

    for (it = lassoColorsMap.begin() ; it != lassoColorsMap.end() ; it++)
    {
        std::pair<int,int> xy = it->first;
        DColor color          = it->second;
        img.setPixelColor(xy.first, xy.second,color);
    }

    d->previewWidget->updateImage(img);
}

void HealingCloneTool::redrawLassoPixels()
{
    int colorCounter = 0;
    DImg img         = d->previewWidget->getOriginalImage();
    std::map<std::pair<int,int>, DColor>::iterator it;

    for (it = lassoColorsMap.begin() ; it != lassoColorsMap.end() ; it++)
    {
        colorCounter++;
        DColor color          = this->lassoColors[(colorCounter) % this->lassoColors.size()];
        std::pair<int,int> xy = it->first;
        img.setPixelColor(xy.first, xy.second,color);
    }

    d->previewWidget->updateImage(img);
}

void HealingCloneTool::slotResetLassoPoints()
{
    removeLassoPixels();
    this->resetLassoPoint      = true;
    this->lassoPoints.clear();
    this->insideLassoOperation = true;
    this->lassoPolygon.clear();
    this->lassoColorsMap.clear();
    this->initializeLassoFlags();
    this->d->previewWidget->setIsLassoPointsVectorEmpty(this->lassoPoints.empty());
}

void HealingCloneTool::slotContinuePolygon()
{
    if (this->lassoPoints.empty())
        return;

    QPoint& start              = this->startLassoPoint;
    QPoint& end                = this->previousLassoPoint;
    std::vector<QPoint> points = interpolate(end,start);
    updateLasso(points);

    this->lassoPoints.push_back(start);

    QVector<QPoint> polygon;

    for(QPoint point: this->lassoPoints)
    {
        polygon.append(point);
    }

    this->lassoPolygon = QPolygon(polygon);
}

void HealingCloneTool::slotIncreaseBrushRadius()
{
    int size = d->radiusInput->value();
    d->radiusInput->setValue(size+1);
}

void HealingCloneTool::slotDecreaseBrushRadius()
{
    int size = d->radiusInput->value();

    d->radiusInput->setValue(size-1);
}

void HealingCloneTool::initializeLassoFlags()
{
//  ImageIface* const iface = d->previewWidget->imageIface();
//  DImg* const img         = iface->previewReference();

    DImg img = d->previewWidget->getOriginalImage();
    int w    = img.width();
    int h    = img.height();
    this->lassoFlags.resize(w);

    for (int i = 0 ; i < w ; i++)
    {
        this->lassoFlags.at(i).resize(h);
    }

    for (int i = 0 ; i < w ; i++)
    {
        for (int j = 0 ; j < h ; j++)
        {
            this->lassoFlags.at(i).at(j) = false;
        }
    }
}

void HealingCloneTool::slotPushToUndoStack()
{
    this->redoStack = std::stack<DImg>();
    removeLassoPixels();
    this->undoStack.push(d->previewWidget->getOriginalImage());
    redrawLassoPixels();
}

void HealingCloneTool::slotUndoClone()
{
    if(this->undoStack.empty())
        return;

    removeLassoPixels();
    this->redoStack.push(d->previewWidget->getOriginalImage());
    DImg temp = this->undoStack.top();
    this->undoStack.pop();
    d->previewWidget->updateImage(temp);
    redrawLassoPixels();
}

void HealingCloneTool::slotRedoClone()
{
    //slotResetLassoPoints();

    if (this->redoStack.empty())
        return;

    removeLassoPixels();
    this->undoStack.push(d->previewWidget->getOriginalImage());

    DImg temp = this->redoStack.top();
    this->redoStack.pop();
    d->previewWidget->updateImage(temp);
    redrawLassoPixels();
}

void HealingCloneTool::refreshImage()
{
    ImageRegionWidget* const wgt = dynamic_cast<ImageRegionWidget*>(d->previewWidget);

    if (wgt)
    {
        qCDebug(DIGIKAM_DIMG_LOG()) << "PROOOOOOF\n\n\n\n\n";
        QRectF test                 = wgt->sceneRect();
        ImageRegionItem* const item = (ImageRegionItem*)wgt->item();
        int w                       = item->boundingRect().width();
        int h                       = item->boundingRect().height();

        test.setWidth(10);
        test.setHeight(10);
        wgt->fitInView( test, Qt::KeepAspectRatio );

        test.setWidth(w);
        test.setHeight(h);
        wgt->fitInView( test, Qt::KeepAspectRatio );
    }
}

} // namespace DigikamEditorHealingCloneToolPlugin
