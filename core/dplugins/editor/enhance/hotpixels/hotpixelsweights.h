/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-03-27
 * Description : a class to calculate filter weights for hot pixels tool
 *
 * Copyright (C) 2005-2006 by Unai Garro <ugarro at users dot sourceforge dot net>
 * Copyright (C) 2005-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_EDITOR_HOT_PIXELS_WEIGHTS_H
#define DIGIKAM_EDITOR_HOT_PIXELS_WEIGHTS_H

// Qt includes

#include <QList>
#include <QRect>

namespace DigikamEditorHotPixelsToolPlugin
{

class HotPixelsWeights
{
public:

    HotPixelsWeights();
    HotPixelsWeights(const HotPixelsWeights& w);
    HotPixelsWeights& operator=(const HotPixelsWeights& w);

    ~HotPixelsWeights()
    {
        if (!mWeightMatrices)
        {
            return;
        }

        for (int i = 0 ; i < mPositions.count() ; ++i)
        {
            for (unsigned int j = 0 ; j < mHeight ; ++j)
            {
                delete [] mWeightMatrices[i][j];
            }
        }

        delete [] mWeightMatrices;
    }

    unsigned int height()                       const
    {
        return mHeight;
    };

    unsigned int polynomeOrder()                const
    {
        return mPolynomeOrder;
    };

    bool twoDim() const
    {
        return mTwoDim;
    };

    unsigned int width()                        const
    {
        return mWidth;
    };

    void setHeight(int h)
    {
        mHeight = h;
    };

    void setPolynomeOrder(int order)
    {
        mPolynomeOrder = order;
    };

    void setTwoDim(bool td)
    {
        mTwoDim = td;
    };

    void setWidth(int w)
    {
        mWidth = w;
    };

    void calculateHotPixelsWeights();

    bool operator==(const HotPixelsWeights& ws) const;

    double** operator[](int n) const
    {
        return mWeightMatrices[n];
    };

    const QList<QPoint> positions()             const
    {
        return mPositions;
    };

protected:

    int coefficientNumber()                     const
    {
        return mCoefficientNumber;
    };

    double** * weightMatrices()                 const
    {
        return mWeightMatrices;
    };

private:

    double polyTerm(const size_t i_coeff,
                    const int x,
                    const int y,
                    const int poly_order)       const;

    void   matrixInv(double* const a,
                     const size_t size);

private:

    unsigned int  mHeight;
    unsigned int  mWidth;
    unsigned int  mCoefficientNumber;
    bool          mTwoDim;
    unsigned int  mPolynomeOrder;
    double** *    mWeightMatrices;   ///< Stores a list of weight matrices
    QList<QPoint> mPositions;
};

} // namespace DigikamEditorHotPixelsToolPlugin

#endif // DIGIKAM_EDITOR_HOT_PIXELS_WEIGHTS_H
