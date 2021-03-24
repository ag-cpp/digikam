/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-02-13
 * Description : Layouting photos on a page
 *
 * Copyright (C) 2007-2009 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2006-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_ATKINS_PAGE_LAYOUT_NODE_H
#define DIGIKAM_ATKINS_PAGE_LAYOUT_NODE_H

// Qt includes

#include <QRectF>
#include <QMap>

/**
 * Implements the algorithm described in
 * "Adaptive Photo Collection Page Layout",
 * C. Brian Atkins
 * Imaging Technology Department
 * HP Labs
 * Palo Alto, CA 94304
 * cbatkins@hpl.hp.com
 *
 * PDF available at this url:
 * hpl.hp.com/research/isl/layout/
 */

/**
 * The classes AtkinsPageLayoutNode and AtkinsPageLayoutTree provide the actual implementation.
 * Do not use these classes directly.
 * Use the AtkinsPageLayout adaptor class.
 */
namespace DigikamGenericPrintCreatorPlugin
{

class AtkinsPageLayoutNode
{
public:

    enum Type
    {
        TerminalNode,
        HorizontalDivision, ///< one image on top of the other
        VerticalDivision    ///< images side by side
    };

public:

    AtkinsPageLayoutNode(double aspectRatio,
                         double relativeArea,
                         int index);
    AtkinsPageLayoutNode(AtkinsPageLayoutNode* const subtree,
                         AtkinsPageLayoutNode* const terminalChild,
                         bool horizontal,
                         int index);
    AtkinsPageLayoutNode(const AtkinsPageLayoutNode&);
    ~AtkinsPageLayoutNode();

    AtkinsPageLayoutNode& operator=(const AtkinsPageLayoutNode&);

    double aspectRatio()  const;
    double relativeArea() const;
    double division()     const;
    Type   type()         const;
    int    index()        const;

    AtkinsPageLayoutNode* leftChild()  const;
    AtkinsPageLayoutNode* rightChild() const;

    /**
     * Replace one child with a new one.
     */
    void takeAndSetChild(AtkinsPageLayoutNode* const oldChild,
                         AtkinsPageLayoutNode* const newChild);

    /**
     * Retrieve the node which has the given index in the hierarchy of this node.
     */
    AtkinsPageLayoutNode* nodeForIndex(int index);

    /**
     * Retrieve the parent node of the given child in the hierarchy of this node.
     */
    AtkinsPageLayoutNode* parentOf(AtkinsPageLayoutNode* const child);

    /**
     * Compute the "aspect ratio" (m_a) and "relative size" (m_e) parameters.
     * Section 2.2.1, (1)-(4).
     */
    void computeRelativeSizes();

    /**
     * Compute the "division" (m_division) parameter.
     * Section 2.2.2
     */
    void computeDivisions();

private:

    double                m_a;        ///< aspect ratio
    double                m_e;        ///< relative size
    double                m_division;

    Type                  m_type;

    int                   m_index;

    AtkinsPageLayoutNode* m_leftChild;
    AtkinsPageLayoutNode* m_rightChild;
};

} // Namespace Digikam

#endif // DIGIKAM_ATKINS_PAGE_LAYOUT_NODE_H
