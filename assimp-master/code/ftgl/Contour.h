/*
 * FTGL - OpenGL font library
 *
 * Copyright (c) 2001-2004 Henry Maddocks <ftgl@opengl.geek.nz>
 * Copyright (c) 2008 Éric Beets <ericbeets@free.fr>
 * Copyright (c) 2008 Sam Hocevar <sam@zoy.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _DIFONT_CONTOUR_H_
#define _DIFONT_CONTOUR_H_

#include <vector>
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

#include "FtglPoint.h"

/**
 * Contour class is a container of points that describe a vector font
 * outline. It is used as a container for the output of the bezier curve
 * evaluator in Vectoriser.
 *
 * @see OutlineGlyph
 * @see PolygonGlyph
 * @see FtglPoint
 */

 class Contour {
    public:
        /**
         * Constructor
         *
         * @param contour
         * @param pointTags
         * @param numberOfPoints
         */
        Contour(FT_Vector* contour, char* pointTags, unsigned int numberOfPoints, unsigned short bezierSteps);

        /**
         * Destructor
         */
        ~Contour()
        {
            pointList.clear();
            outsetPointList.clear();
            frontPointList.clear();
            backPointList.clear();
        }

        /**
         * Return a point at index.
         *
         * @param index of the point in the curve.
         * @return const point reference
         */
        const FtglPoint& GetPoint(size_t index) const { return pointList[index]; }

        /**
         * How many points define this contour
         *
         * @return the number of points in this contour
         */
        size_t PointCount() const { return pointList.size(); }

        /**
         * Make sure the glyph has the proper parity and create the front/back
         * outset contour.
         *
         * @param parity  The contour's parity within the glyph.
         */
        void SetParity(int parity);

        /**
         * Make sure the glyph has the proper parity and create the front/back
         * outset contour.
         *
         * @param parity  The contour's parity within the glyph.
         */
        bool GetDirection() const {
            return clockwise;
        }

        bool IsInside(const Contour* big) const {
            if(minx > big->minx && miny > big->miny 
                && maxx < big->maxx && maxy < big->maxy)
                return true;
            else 
                return false;
        }

     void Reverse();

        float minx, miny, maxx, maxy;
    private:
        /**
         * Add a point to this contour. This function tests for duplicate
         * points.
         *
         * @param point The point to be added to the contour.
         */
        void AddPoint(FtglPoint point);

        /**
         * Add a point to this contour. This function tests for duplicate
         * points.
         *
         * @param point The point to be added to the contour.
         */
        void AddOutsetPoint(FtglPoint point);


        /**
         * De Casteljau (bezier) algorithm contributed by Jed Soane
         * Evaluates a quadratic or conic (second degree) curve
         */
        void evaluateQuadraticCurve(FtglPoint, FtglPoint, FtglPoint, unsigned short);

        /**
         * De Casteljau (bezier) algorithm contributed by Jed Soane
         * Evaluates a cubic (third degree) curve
         */
        void evaluateCubicCurve(FtglPoint, FtglPoint, FtglPoint, FtglPoint, unsigned short);

        /**
         * Compute the outset point coordinates
         */
        FtglPoint ComputeOutsetPoint(FtglPoint a, FtglPoint b, FtglPoint c);

        /**
         *  The list of points in this contour
         */
        typedef std::vector<FtglPoint> PointVector;
        PointVector pointList;
        PointVector outsetPointList;
        PointVector frontPointList;
        PointVector backPointList;
        
        /**
         *  Is this contour clockwise or anti-clockwise?
         */
        bool clockwise;

};

#endif // __Contour__

