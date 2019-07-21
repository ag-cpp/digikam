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


namespace DigikamEditorHealingCloneToolPlugin
{

class Q_DECL_HIDDEN HealingCloneTool::Private
{

public:

    explicit Private()
      : radiusInput(0),
        zoomInput(0),
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
    DIntNumInput*           zoomInput;
    DDoubleNumInput*        blurPercent;
    ImageBrushGuideWidget*  previewWidget;
    EditorToolSettings*     gboxSettings;
    QPoint                  sourcePoint;
    QPoint                  destinationStartPoint;
    QPushButton*            srcButton;
};




const QString HealingCloneTool::Private::configGroupName(QLatin1String("Healing Clone Tool"));
const QString HealingCloneTool::Private::configRadiusAdjustmentEntry(QLatin1String("RadiusAdjustment"));
const QString HealingCloneTool::Private::configBlurAdjustmentEntry(QLatin1String("BlurAdjustment"));
// --------------------------------------------------------

HealingCloneTool::HealingCloneTool(QObject* const parent)
    : EditorTool(parent),
      d(new Private)
{
    setObjectName(QLatin1String("healing clone"));
    setToolHelp(QLatin1String("healingclonetool.anchor"));

    d->gboxSettings      = new EditorToolSettings(0);
    d->previewWidget     = new ImageBrushGuideWidget(0, true, ImageGuideWidget::PickColorMode, Qt::red);
    d->previewWidget->setFocusPolicy(Qt::StrongFocus);
    setToolView(d->previewWidget);
    setPreviewModeMask(PreviewToolBar::PreviewTargetImage);

    // --------------------------------------------------------

    QLabel* const label  = new QLabel(i18n("Brush Radius:"));
    d->radiusInput       = new DIntNumInput();
    d->radiusInput->setRange(0, 50, 1);
    d->radiusInput->setDefaultValue(10);
    d->radiusInput->setWhatsThis(i18n("A radius of 0 has no effect, "
                                      "1 and above determine the brush radius "
                                      "that determines the size of parts copied in the image."));

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

    QLabel* const label3 = new QLabel(i18n("Source:"));
    d->srcButton         = new QPushButton(i18n("Set Source Point"), d->gboxSettings->plainPage());

    // --------------------------------------------------------

    QLabel* const label4  = new QLabel(i18n("Zoom:"));
    d->zoomInput       = new DIntNumInput();
    d->zoomInput->setRange(10, 300, 10);
    d->zoomInput->setDefaultValue(100);
    d->zoomInput->setWhatsThis(i18n("Zoom In or Out"));
    //----------------------------------------------------------

    const int spacing = d->gboxSettings->spacingHint();

    QGridLayout* const grid = new QGridLayout( );
    grid->addWidget(label3,         1, 0, 1, 2);
    grid->addWidget(d->srcButton,   2, 0, 1, 2);
    grid->addWidget(new DLineWidget(Qt::Horizontal, d->gboxSettings->plainPage()), 3, 0, 1, 2);
    grid->addWidget(label,          4, 0, 1, 2);
    grid->addWidget(d->radiusInput, 5, 0, 1, 2);
    grid->addWidget(label2,         6, 0, 1, 2);
    grid->addWidget(d->blurPercent, 7, 0, 1, 2);
    grid->addWidget(label4,         8, 0, 1, 2);
    grid->addWidget(d->zoomInput, 9, 0, 1, 2);
    grid->setRowStretch(10, 10);
    grid->setContentsMargins(spacing, spacing, spacing, spacing);
    grid->setSpacing(spacing);
    d->gboxSettings->plainPage()->setLayout(grid);

    // --------------------------------------------------------

    setPreviewModeMask(PreviewToolBar::PreviewTargetImage);
    setToolSettings(d->gboxSettings);
    setToolView(d->previewWidget);

    // --------------------------------------------------------

    this->CloneInfoVector = new std::vector<CloneInfo>();
    this->lassoColors.push_back(DColor(Qt::red));
    this->lassoColors.push_back(DColor(Qt::white));
    this->lassoColors.push_back(DColor(Qt::black));
    this->lassoColors.push_back(DColor(Qt::yellow));
    this->lassoColors.push_back(DColor(Qt::blue));
    this->lassoColors.push_back(DColor(Qt::yellow));
    // --------------------------------------------------------

    connect(d->radiusInput, SIGNAL(valueChanged(int)),
            this, SLOT(slotRadiusChanged(int)));

    connect(d->zoomInput, SIGNAL(valueChanged(int)),
            this, SLOT(slotZoomPercentChanged(int)));

    connect(d->srcButton, SIGNAL(clicked(bool)),
            d->previewWidget, SLOT(slotSetSourcePoint()));

    connect(d->previewWidget, SIGNAL(signalClone(QPoint,QPoint)),
            this, SLOT(slotReplace(QPoint,QPoint)));

    connect(d->previewWidget, SIGNAL(signalResized()),
            this, SLOT(slotResized()));

    connect(d->previewWidget, SIGNAL(signalReclone()),
            this, SLOT(slotReclone()));

    connect(d->previewWidget, SIGNAL(signalLasso(QPoint)),
            this, SLOT(slotLasso(QPoint)));

    connect(d->previewWidget, SIGNAL(signalResetLassoPoint()),
            this, SLOT(slotResetLassoPoint()));

    connect(d->previewWidget,SIGNAL(signalContinuePolygon()),
            this, SLOT(slotContinuePolygon()));
    connect(d->previewWidget,SIGNAL(signalEndLassoSession()),
            this, SLOT(slotEndLassoSession()));


}

HealingCloneTool::~HealingCloneTool()
{
    delete d;
    delete this->CloneInfoVector;
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
    DImg dest = d->previewWidget->imageIface()->preview();
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
    ImageIface* const iface = d->previewWidget->imageIface();
    DImg* const current     = iface->previewReference();
    clone(current, srcPoint, dstPoint, d->radiusInput->value());
    d->previewWidget->updatePreview();
}

void HealingCloneTool::slotRadiusChanged(int r)
{
    d->previewWidget->setMaskPenSize(r);

}

void HealingCloneTool :: slotZoomPercentChanged(int z)
{
   d->previewWidget->zoomImage(z);
}

void HealingCloneTool::clone(DImg* const img, const QPoint& srcPoint, const QPoint& dstPoint, int radius)
{
    double blurPercent = d->blurPercent->value() / 100;
    double scaleRatio = d->previewWidget->getScaleRatio();



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

                qCDebug(DIGIKAM_GENERAL_LOG()) << insideLassoOperation << this->lassoPoints.empty();
                if(insideLassoOperation && !this->lassoPoints.empty())
                {
                    if(this->lassoFlags.at(srcPoint.x() +i).at(srcPoint.y() +j) ||
                       this->lassoFlags.at(dstPoint.x()+i).at(dstPoint.y()+j))
                    {
                        continue;
                    }

                    bool isInside = this->lassoPolygon.containsPoint(QPoint(dstPoint.x()+i,dstPoint.y()+j),
                                                                     Qt::OddEvenFill);

                     if(!isInside)
                        {
                            continue;
                        }


                }

                double rP   = blurPercent * rPercent / (radius * radius);
                DColor cSrc = img->getPixelColor(srcPoint.x()+i, srcPoint.y()+j);
                DColor cDst = img->getPixelColor(dstPoint.x()+i, dstPoint.y()+j);
                cSrc.multiply(1 - rP);
                cDst.multiply(rP);
                cSrc.blendAdd(cDst);

                img->setPixelColor(dstPoint.x()+i, dstPoint.y()+j, cSrc);
                this->CloneInfoVector->push_back({dstPoint.x()+i, dstPoint.y()+j, scaleRatio,cSrc});
            }
        }
    }
}


void HealingCloneTool :: slotReclone()
{

    double currentScaleRatio = d->previewWidget->getScaleRatio();

    ImageIface* const iface = d->previewWidget->imageIface();
    DImg* const img     = iface->previewReference();


    for(int q = 0 ; q < CloneInfoVector->size(); q++)
    {
        int x = (*CloneInfoVector)[q].dstX;
        int y = (*CloneInfoVector)[q].dstY;
        double thenScaleRatio = (*CloneInfoVector)[q].scaleRatio;
        DColor color = (*CloneInfoVector)[q].color;
       double ratioOfRatios = currentScaleRatio/thenScaleRatio;
       int radius = ceil(ratioOfRatios);
       for(int k = 0 ; k < radius ; k++)
       {
           for(int s = 0 ; s<radius ; s++)
           {
               img->setPixelColor(round(x * currentScaleRatio/thenScaleRatio)+k,
                                  round(y*currentScaleRatio/thenScaleRatio) + s ,
                                  color);

           }
       }
    }

    d->previewWidget->updatePreview();

}

void HealingCloneTool :: updateLasso(std::vector<QPoint>& points)
{
    uint radius = 5;
    static uint colorCounter = 0;
    ImageIface* const iface = d->previewWidget->imageIface();
    DImg* const img     = iface->previewReference();
    for (QPoint p: points)
    {
        for(uint i = 0 ; i < radius ; i++)
        {
            for(uint j = 0; j<radius ; j++)
            {

                img->setPixelColor(p.x()+i,p.y()+j,this->lassoColors[(colorCounter)%this->lassoColors.size()]);
                this->lassoFlags.at(p.x()+i).at(p.y()+j) = true;
                colorCounter++;
            }
        }
    }

    d->previewWidget->updatePreview();
}

void HealingCloneTool :: slotLasso(const QPoint& dst)
{

    if(this->resetLassoPoint)
    {
        this->previousLassoPoint = dst;
        this->resetLassoPoint = false;
        this->startLassoPoint = dst;
    }
    std::vector<QPoint> points = interpolate(this->previousLassoPoint, dst);
    this->lassoPoints.push_back(dst);
    this->previousLassoPoint = dst;
    this->updateLasso(points);


}

std::vector<QPoint> HealingCloneTool :: interpolate(const QPoint& start, const QPoint& end)
{
    std::vector<QPoint> points;
    points.push_back(start);
    QPointF distanceVec = QPoint(end.x()-start.x() , end.y() - start.y());
    double distance = sqrt(distanceVec.x() * distanceVec.x() + distanceVec.y() * distanceVec.y());
    //creating a unit vector
    distanceVec.setX(distanceVec.x()/distance);
    distanceVec.setY(distanceVec.y()/distance);
    int steps = (int) distance ;
    for(int i = 0 ; i<steps ; i++)
    {
        points.push_back(QPoint(start.x() + i*distanceVec.x() ,start.y() + i*distanceVec.y()));
    }

    points.push_back(end);

    return points;
}

void HealingCloneTool ::slotResetLassoPoint()
{
    this->resetLassoPoint = true;
    this->lassoPoints.clear();
    this->insideLassoOperation = true;
    this->lassoPolygon.clear();
    this->initializeLassoFlags();

}

void HealingCloneTool :: slotContinuePolygon()
{
    if(this->lassoPoints.empty())
        return;
    QPoint& start = this->startLassoPoint;
    QPoint& end = this->previousLassoPoint;
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

void HealingCloneTool :: slotEndLassoSession()
{
    this->slotResetLassoPoint();
    this->insideLassoOperation = false;
    this->d->previewWidget->resetPixels();
    qCDebug(DIGIKAM_GENERAL_LOG()) << "ENDING LASSO OPERATION, L PRESSED";

}

void HealingCloneTool :: initializeLassoFlags()
{
    ImageIface* const iface = d->previewWidget->imageIface();
    DImg* const img     = iface->previewReference();
    int w = img->width();
    int h = img->height();
    this->lassoFlags.resize(w);
    for(int i = 0 ; i < w; i++)
    {
        this->lassoFlags.at(i).resize(h);
    }

    for(int i = 0 ; i < w; i++)
    {
        for(int j = 0 ; j < h ; j++)
        {
            this->lassoFlags.at(i).at(j) = false;
        }
    }
}

} // namespace DigikamEditorHealingCloneToolPlugin
