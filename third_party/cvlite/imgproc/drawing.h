// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "third_party/cvlite/core/core.hpp"
#include "third_party/cvlite/core/core_c.h"

namespace c8cv {

/****************************************************************************************\
*                                     Drawing                                            *
\****************************************************************************************/

/****************************************************************************************\
*       Drawing functions work with images/matrices of arbitrary type.                   *
*       For color images the channel order is BGR[A]                                     *
*       Antialiasing is supported only for 8-bit image now.                              *
*       All the functions include parameter color that means rgb value (that may be      *
*       constructed with CV_RGB macro) for color images and brightness                   *
*       for grayscale images.                                                            *
*       If a drawn figure is partially or completely outside of the image, it is clipped.*
\****************************************************************************************/

#define CV_RGB(r, g, b) cvScalar((b), (g), (r), 0)
#define CV_FILLED -1

#define CV_AA 16

/** @brief Draws 4-connected, 8-connected or antialiased line segment connecting two points
@see cv::line
*/
CVAPI(void)
cvLine(
  CvArr *img,
  CvPoint pt1,
  CvPoint pt2,
  CvScalar color,
  int thickness CV_DEFAULT(1),
  int line_type CV_DEFAULT(8),
  int shift CV_DEFAULT(0));

/** @brief Draws a rectangle given two opposite corners of the rectangle (pt1 & pt2)

   if thickness<0 (e.g. thickness == CV_FILLED), the filled box is drawn
@see cv::rectangle
*/
CVAPI(void)
cvRectangle(
  CvArr *img,
  CvPoint pt1,
  CvPoint pt2,
  CvScalar color,
  int thickness CV_DEFAULT(1),
  int line_type CV_DEFAULT(8),
  int shift CV_DEFAULT(0));

/** @brief Draws a rectangle specified by a CvRect structure
@see cv::rectangle
*/
CVAPI(void)
cvRectangleR(
  CvArr *img,
  CvRect r,
  CvScalar color,
  int thickness CV_DEFAULT(1),
  int line_type CV_DEFAULT(8),
  int shift CV_DEFAULT(0));

/** @brief Draws a circle with specified center and radius.

   Thickness works in the same way as with cvRectangle
@see cv::circle
*/
CVAPI(void)
cvCircle(
  CvArr *img,
  CvPoint center,
  int radius,
  CvScalar color,
  int thickness CV_DEFAULT(1),
  int line_type CV_DEFAULT(8),
  int shift CV_DEFAULT(0));

/** @brief Draws ellipse outline, filled ellipse, elliptic arc or filled elliptic sector

   depending on _thickness_, _start_angle_ and _end_angle_ parameters. The resultant figure
   is rotated by _angle_. All the angles are in degrees
@see cv::ellipse
*/
CVAPI(void)
cvEllipse(
  CvArr *img,
  CvPoint center,
  CvSize axes,
  double angle,
  double start_angle,
  double end_angle,
  CvScalar color,
  int thickness CV_DEFAULT(1),
  int line_type CV_DEFAULT(8),
  int shift CV_DEFAULT(0));

CV_INLINE void cvEllipseBox(
  CvArr *img,
  CvBox2D box,
  CvScalar color,
  int thickness CV_DEFAULT(1),
  int line_type CV_DEFAULT(8),
  int shift CV_DEFAULT(0)) {
  CvSize axes;
  axes.width = cvRound(box.size.width * 0.5);
  axes.height = cvRound(box.size.height * 0.5);

  cvEllipse(
    img, cvPointFrom32f(box.center), axes, box.angle, 0, 360, color, thickness, line_type, shift);
}

/** @brief Fills convex or monotonous polygon.
@see cv::fillConvexPoly
*/
CVAPI(void)
cvFillConvexPoly(
  CvArr *img,
  const CvPoint *pts,
  int npts,
  CvScalar color,
  int line_type CV_DEFAULT(8),
  int shift CV_DEFAULT(0));

/** @brief Fills an area bounded by one or more arbitrary polygons
@see cv::fillPoly
*/
CVAPI(void)
cvFillPoly(
  CvArr *img,
  CvPoint **pts,
  const int *npts,
  int contours,
  CvScalar color,
  int line_type CV_DEFAULT(8),
  int shift CV_DEFAULT(0));

/** @brief Draws one or more polygonal curves
@see cv::polylines
*/
CVAPI(void)
cvPolyLine(
  CvArr *img,
  CvPoint **pts,
  const int *npts,
  int contours,
  int is_closed,
  CvScalar color,
  int thickness CV_DEFAULT(1),
  int line_type CV_DEFAULT(8),
  int shift CV_DEFAULT(0));

#define cvDrawRect cvRectangle
#define cvDrawLine cvLine
#define cvDrawCircle cvCircle
#define cvDrawEllipse cvEllipse
#define cvDrawPolyLine cvPolyLine

/** @brief Clips the line segment connecting *pt1 and *pt2
   by the rectangular window

   (0<=x<img_size.width, 0<=y<img_size.height).
@see cv::clipLine
*/
CVAPI(int) cvClipLine(CvSize img_size, CvPoint *pt1, CvPoint *pt2);

/** @brief Initializes line iterator.

Initially, line_iterator->ptr will point to pt1 (or pt2, see left_to_right description) location in
the image. Returns the number of pixels on the line between the ending points.
@see cv::LineIterator
*/
CVAPI(int)
cvInitLineIterator(
  const CvArr *image,
  CvPoint pt1,
  CvPoint pt2,
  CvLineIterator *line_iterator,
  int connectivity CV_DEFAULT(8),
  int left_to_right CV_DEFAULT(0));

#define CV_NEXT_LINE_POINT(line_iterator)                                               \
  {                                                                                     \
    int _line_iterator_mask = (line_iterator).err < 0 ? -1 : 0;                         \
    (line_iterator).err +=                                                              \
      (line_iterator).minus_delta + ((line_iterator).plus_delta & _line_iterator_mask); \
    (line_iterator).ptr +=                                                              \
      (line_iterator).minus_step + ((line_iterator).plus_step & _line_iterator_mask);   \
  }

#define CV_FONT_HERSHEY_SIMPLEX 0
#define CV_FONT_HERSHEY_PLAIN 1
#define CV_FONT_HERSHEY_DUPLEX 2
#define CV_FONT_HERSHEY_COMPLEX 3
#define CV_FONT_HERSHEY_TRIPLEX 4
#define CV_FONT_HERSHEY_COMPLEX_SMALL 5
#define CV_FONT_HERSHEY_SCRIPT_SIMPLEX 6
#define CV_FONT_HERSHEY_SCRIPT_COMPLEX 7

#define CV_FONT_ITALIC 16

#define CV_FONT_VECTOR0 CV_FONT_HERSHEY_SIMPLEX

/** Font structure */
typedef struct CvFont {
  const char *nameFont;  // Qt:nameFont
  CvScalar color;    // Qt:ColorFont -> cvScalar(blue_component, green_component, red_component[,
                     // alpha_component])
  int font_face;     // Qt: bool italic         /** =CV_FONT_* */
  const int *ascii;  //!< font data and metrics
  const int *greek;
  const int *cyrillic;
  float hscale, vscale;
  float shear;    //!< slope coefficient: 0 - normal, >0 - italic
  int thickness;  //!< Qt: weight               /** letters thickness */
  float dx;       //!< horizontal interval between letters
  int line_type;  //!< Qt: PointSize
} CvFont;

/** @brief Initializes font structure (OpenCV 1.x API).

The function initializes the font structure that can be passed to text rendering functions.

@param font Pointer to the font structure initialized by the function
@param font_face Font name identifier. See cv::HersheyFonts and corresponding old CV_* identifiers.
@param hscale Horizontal scale. If equal to 1.0f , the characters have the original width
depending on the font type. If equal to 0.5f , the characters are of half the original width.
@param vscale Vertical scale. If equal to 1.0f , the characters have the original height depending
on the font type. If equal to 0.5f , the characters are of half the original height.
@param shear Approximate tangent of the character slope relative to the vertical line. A zero
value means a non-italic font, 1.0f means about a 45 degree slope, etc.
@param thickness Thickness of the text strokes
@param line_type Type of the strokes, see line description

@sa cvPutText
 */
CVAPI(void)
cvInitFont(
  CvFont *font,
  int font_face,
  double hscale,
  double vscale,
  double shear CV_DEFAULT(0),
  int thickness CV_DEFAULT(1),
  int line_type CV_DEFAULT(8));

CV_INLINE CvFont cvFont(double scale, int thickness CV_DEFAULT(1)) {
  CvFont font;
  cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, scale, scale, 0, thickness, CV_AA);
  return font;
}

/** @brief Renders text stroke with specified font and color at specified location.
   CvFont should be initialized with cvInitFont
@see cvInitFont, cvGetTextSize, cvFont, cv::putText
*/
CVAPI(void)
cvPutText(CvArr *img, const char *text, CvPoint org, const CvFont *font, CvScalar color);

/** @brief Calculates bounding box of text stroke (useful for alignment)
@see cv::getTextSize
*/
CVAPI(void)
cvGetTextSize(const char *text_string, const CvFont *font, CvSize *text_size, int *baseline);

/** @brief Unpacks color value

if arrtype is CV_8UC?, _color_ is treated as packed color value, otherwise the first channels
(depending on arrtype) of destination scalar are set to the same value = _color_
*/
CVAPI(CvScalar) cvColorToScalar(double packed_color, int arrtype);

/** @brief Returns the polygon points which make up the given ellipse.

The ellipse is define by the box of size 'axes' rotated 'angle' around the 'center'. A partial
sweep of the ellipse arc can be done by spcifying arc_start and arc_end to be something other than
0 and 360, respectively. The input array 'pts' must be large enough to hold the result. The total
number of points stored into 'pts' is returned by this function.
@see cv::ellipse2Poly
*/
CVAPI(int)
cvEllipse2Poly(
  CvPoint center, CvSize axes, int angle, int arc_start, int arc_end, CvPoint *pts, int delta);

/** @brief Draws contour outlines or filled interiors on the image
@see cv::drawContours
*/
CVAPI(void)
cvDrawContours(
  CvArr *img,
  CvSeq *contour,
  CvScalar external_color,
  CvScalar hole_color,
  int max_level,
  int thickness CV_DEFAULT(1),
  int line_type CV_DEFAULT(8),
  CvPoint offset CV_DEFAULT(cvPoint(0, 0)));

//! @addtogroup imgproc_draw
//! @{

/** @brief Draws a line segment connecting two points.

The function line draws the line segment between pt1 and pt2 points in the image. The line is
clipped by the image boundaries. For non-antialiased lines with integer coordinates, the 8-connected
or 4-connected Bresenham algorithm is used. Thick lines are drawn with rounding endings. Antialiased
lines are drawn using Gaussian filtering.

@param img Image.
@param pt1 First point of the line segment.
@param pt2 Second point of the line segment.
@param color Line color.
@param thickness Line thickness.
@param lineType Type of the line, see cv::LineTypes.
@param shift Number of fractional bits in the point coordinates.
 */
CV_EXPORTS_W void line(
  InputOutputArray img,
  Point pt1,
  Point pt2,
  const Scalar &color,
  int thickness = 1,
  int lineType = LINE_8,
  int shift = 0);

/** @brief Draws a arrow segment pointing from the first point to the second one.

The function arrowedLine draws an arrow between pt1 and pt2 points in the image. See also cv::line.

@param img Image.
@param pt1 The point the arrow starts from.
@param pt2 The point the arrow points to.
@param color Line color.
@param thickness Line thickness.
@param line_type Type of the line, see cv::LineTypes
@param shift Number of fractional bits in the point coordinates.
@param tipLength The length of the arrow tip in relation to the arrow length
 */
CV_EXPORTS_W void arrowedLine(
  InputOutputArray img,
  Point pt1,
  Point pt2,
  const Scalar &color,
  int thickness = 1,
  int line_type = 8,
  int shift = 0,
  double tipLength = 0.1);

/** @brief Draws a simple, thick, or filled up-right rectangle.

The function rectangle draws a rectangle outline or a filled rectangle whose two opposite corners
are pt1 and pt2.

@param img Image.
@param pt1 Vertex of the rectangle.
@param pt2 Vertex of the rectangle opposite to pt1 .
@param color Rectangle color or brightness (grayscale image).
@param thickness Thickness of lines that make up the rectangle. Negative values, like CV_FILLED ,
mean that the function has to draw a filled rectangle.
@param lineType Type of the line. See the line description.
@param shift Number of fractional bits in the point coordinates.
 */
CV_EXPORTS_W void rectangle(
  InputOutputArray img,
  Point pt1,
  Point pt2,
  const Scalar &color,
  int thickness = 1,
  int lineType = LINE_8,
  int shift = 0);

/** @overload

use `rec` parameter as alternative specification of the drawn rectangle: `r.tl() and
r.br()-Point(1,1)` are opposite corners
*/
CV_EXPORTS void rectangle(
  CV_IN_OUT Mat &img,
  Rect rec,
  const Scalar &color,
  int thickness = 1,
  int lineType = LINE_8,
  int shift = 0);

/** @brief Draws a circle.

The function circle draws a simple or filled circle with a given center and radius.
@param img Image where the circle is drawn.
@param center Center of the circle.
@param radius Radius of the circle.
@param color Circle color.
@param thickness Thickness of the circle outline, if positive. Negative thickness means that a
filled circle is to be drawn.
@param lineType Type of the circle boundary. See the line description.
@param shift Number of fractional bits in the coordinates of the center and in the radius value.
 */
CV_EXPORTS_W void circle(
  InputOutputArray img,
  Point center,
  int radius,
  const Scalar &color,
  int thickness = 1,
  int lineType = LINE_8,
  int shift = 0);

/** @brief Draws a simple or thick elliptic arc or fills an ellipse sector.

The function cv::ellipse with less parameters draws an ellipse outline, a filled ellipse, an
elliptic arc, or a filled ellipse sector. A piecewise-linear curve is used to approximate the
elliptic arc boundary. If you need more control of the ellipse rendering, you can retrieve the curve
using ellipse2Poly and then render it with polylines or fill it with fillPoly . If you use the first
variant of the function and want to draw the whole ellipse, not an arc, pass startAngle=0 and
endAngle=360 . The figure below explains the meaning of the parameters.

![Parameters of Elliptic Arc](pics/ellipse.png)

@param img Image.
@param center Center of the ellipse.
@param axes Half of the size of the ellipse main axes.
@param angle Ellipse rotation angle in degrees.
@param startAngle Starting angle of the elliptic arc in degrees.
@param endAngle Ending angle of the elliptic arc in degrees.
@param color Ellipse color.
@param thickness Thickness of the ellipse arc outline, if positive. Otherwise, this indicates that
a filled ellipse sector is to be drawn.
@param lineType Type of the ellipse boundary. See the line description.
@param shift Number of fractional bits in the coordinates of the center and values of axes.
 */
CV_EXPORTS_W void ellipse(
  InputOutputArray img,
  Point center,
  Size axes,
  double angle,
  double startAngle,
  double endAngle,
  const Scalar &color,
  int thickness = 1,
  int lineType = LINE_8,
  int shift = 0);

/** @overload
@param img Image.
@param box Alternative ellipse representation via RotatedRect. This means that the function draws
an ellipse inscribed in the rotated rectangle.
@param color Ellipse color.
@param thickness Thickness of the ellipse arc outline, if positive. Otherwise, this indicates that
a filled ellipse sector is to be drawn.
@param lineType Type of the ellipse boundary. See the line description.
*/
CV_EXPORTS_W void ellipse(
  InputOutputArray img,
  const RotatedRect &box,
  const Scalar &color,
  int thickness = 1,
  int lineType = LINE_8);

/* ----------------------------------------------------------------------------------------- */
/* ADDING A SET OF PREDEFINED MARKERS WHICH COULD BE USED TO HIGHLIGHT POSITIONS IN AN IMAGE */
/* ----------------------------------------------------------------------------------------- */

//! Possible set of marker types used for the cv::drawMarker function
enum MarkerTypes {
  MARKER_CROSS = 0,         //!< A crosshair marker shape
  MARKER_TILTED_CROSS = 1,  //!< A 45 degree tilted crosshair marker shape
  MARKER_STAR = 2,          //!< A star marker shape, combination of cross and tilted cross
  MARKER_DIAMOND = 3,       //!< A diamond marker shape
  MARKER_SQUARE = 4,        //!< A square marker shape
  MARKER_TRIANGLE_UP = 5,   //!< An upwards pointing triangle marker shape
  MARKER_TRIANGLE_DOWN = 6  //!< A downwards pointing triangle marker shape
};

/** @brief Draws a marker on a predefined position in an image.

The function drawMarker draws a marker on a given position in the image. For the moment several
marker types are supported, see cv::MarkerTypes for more information.

@param img Image.
@param position The point where the crosshair is positioned.
@param color Line color.
@param markerType The specific type of marker you want to use, see cv::MarkerTypes
@param thickness Line thickness.
@param line_type Type of the line, see cv::LineTypes
@param markerSize The length of the marker axis [default = 20 pixels]
 */
CV_EXPORTS_W void drawMarker(
  CV_IN_OUT Mat &img,
  Point position,
  const Scalar &color,
  int markerType = MARKER_CROSS,
  int markerSize = 20,
  int thickness = 1,
  int line_type = 8);

/* ----------------------------------------------------------------------------------------- */
/* END OF MARKER SECTION */
/* ----------------------------------------------------------------------------------------- */

/** @overload */
CV_EXPORTS void fillConvexPoly(
  Mat &img, const Point *pts, int npts, const Scalar &color, int lineType = LINE_8, int shift = 0);

/** @brief Fills a convex polygon.

The function fillConvexPoly draws a filled convex polygon. This function is much faster than the
function cv::fillPoly . It can fill not only convex polygons but any monotonic polygon without
self-intersections, that is, a polygon whose contour intersects every horizontal line (scan line)
twice at the most (though, its top-most and/or the bottom edge could be horizontal).

@param img Image.
@param points Polygon vertices.
@param color Polygon color.
@param lineType Type of the polygon boundaries. See the line description.
@param shift Number of fractional bits in the vertex coordinates.
 */
CV_EXPORTS_W void fillConvexPoly(
  InputOutputArray img,
  InputArray points,
  const Scalar &color,
  int lineType = LINE_8,
  int shift = 0);

/** @overload */
CV_EXPORTS void fillPoly(
  Mat &img,
  const Point **pts,
  const int *npts,
  int ncontours,
  const Scalar &color,
  int lineType = LINE_8,
  int shift = 0,
  Point offset = Point());

/** @brief Fills the area bounded by one or more polygons.

The function fillPoly fills an area bounded by several polygonal contours. The function can fill
complex areas, for example, areas with holes, contours with self-intersections (some of their
parts), and so forth.

@param img Image.
@param pts Array of polygons where each polygon is represented as an array of points.
@param color Polygon color.
@param lineType Type of the polygon boundaries. See the line description.
@param shift Number of fractional bits in the vertex coordinates.
@param offset Optional offset of all points of the contours.
 */
CV_EXPORTS_W void fillPoly(
  InputOutputArray img,
  InputArrayOfArrays pts,
  const Scalar &color,
  int lineType = LINE_8,
  int shift = 0,
  Point offset = Point());

/** @overload */
CV_EXPORTS void polylines(
  Mat &img,
  const Point *const *pts,
  const int *npts,
  int ncontours,
  bool isClosed,
  const Scalar &color,
  int thickness = 1,
  int lineType = LINE_8,
  int shift = 0);

/** @brief Draws several polygonal curves.

@param img Image.
@param pts Array of polygonal curves.
@param isClosed Flag indicating whether the drawn polylines are closed or not. If they are closed,
the function draws a line from the last vertex of each curve to its first vertex.
@param color Polyline color.
@param thickness Thickness of the polyline edges.
@param lineType Type of the line segments. See the line description.
@param shift Number of fractional bits in the vertex coordinates.

The function polylines draws one or more polygonal curves.
 */
CV_EXPORTS_W void polylines(
  InputOutputArray img,
  InputArrayOfArrays pts,
  bool isClosed,
  const Scalar &color,
  int thickness = 1,
  int lineType = LINE_8,
  int shift = 0);

/** @example contours2.cpp
  An example using the drawContour functionality
*/

/** @example segment_objects.cpp
An example using drawContours to clean up a background segmentation result
 */

/** @brief Draws contours outlines or filled contours.

The function draws contour outlines in the image if \f$\texttt{thickness} \ge 0\f$ or fills the area
bounded by the contours if \f$\texttt{thickness}<0\f$ . The example below shows how to retrieve
connected components from the binary image and label them: :
@code
    #include "opencv2/imgproc.hpp"
    #include "opencv2/highgui.hpp"

    using namespace cv;
    using namespace std;

    int main( int argc, char** argv )
    {
        Mat src;
        // the first command-line parameter must be a filename of the binary
        // (black-n-white) image
        if( argc != 2 || !(src=imread(argv[1], 0)).data)
            return -1;

        Mat dst = Mat::zeros(src.rows, src.cols, CV_8UC3);

        src = src > 1;
        namedWindow( "Source", 1 );
        imshow( "Source", src );

        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;

        findContours( src, contours, hierarchy,
            RETR_CCOMP, CHAIN_APPROX_SIMPLE );

        // iterate through all the top-level contours,
        // draw each connected component with its own random color
        int idx = 0;
        for( ; idx >= 0; idx = hierarchy[idx][0] )
        {
            Scalar color( rand()&255, rand()&255, rand()&255 );
            drawContours( dst, contours, idx, color, FILLED, 8, hierarchy );
        }

        namedWindow( "Components", 1 );
        imshow( "Components", dst );
        waitKey(0);
    }
@endcode

@param image Destination image.
@param contours All the input contours. Each contour is stored as a point vector.
@param contourIdx Parameter indicating a contour to draw. If it is negative, all the contours are
drawn.
@param color Color of the contours.
@param thickness Thickness of lines the contours are drawn with. If it is negative (for example,
thickness=CV_FILLED ), the contour interiors are drawn.
@param lineType Line connectivity. See cv::LineTypes.
@param hierarchy Optional information about hierarchy. It is only needed if you want to draw only
some of the contours (see maxLevel ).
@param maxLevel Maximal level for drawn contours. If it is 0, only the specified contour is drawn.
If it is 1, the function draws the contour(s) and all the nested contours. If it is 2, the function
draws the contours, all the nested contours, all the nested-to-nested contours, and so on. This
parameter is only taken into account when there is hierarchy available.
@param offset Optional contour shift parameter. Shift all the drawn contours by the specified
\f$\texttt{offset}=(dx,dy)\f$ .
 */
CV_EXPORTS_W void drawContours(
  InputOutputArray image,
  InputArrayOfArrays contours,
  int contourIdx,
  const Scalar &color,
  int thickness = 1,
  int lineType = LINE_8,
  InputArray hierarchy = noArray(),
  int maxLevel = INT_MAX,
  Point offset = Point());

/** @brief Clips the line against the image rectangle.

The function cv::clipLine calculates a part of the line segment that is entirely within the
specified rectangle. it returns false if the line segment is completely outside the rectangle.
Otherwise, it returns true .
@param imgSize Image size. The image rectangle is Rect(0, 0, imgSize.width, imgSize.height) .
@param pt1 First line point.
@param pt2 Second line point.
 */
CV_EXPORTS bool clipLine(Size imgSize, CV_IN_OUT Point &pt1, CV_IN_OUT Point &pt2);

/** @overload
@param imgSize Image size. The image rectangle is Rect(0, 0, imgSize.width, imgSize.height) .
@param pt1 First line point.
@param pt2 Second line point.
*/
CV_EXPORTS bool clipLine(Size2l imgSize, CV_IN_OUT Point2l &pt1, CV_IN_OUT Point2l &pt2);

/** @overload
@param imgRect Image rectangle.
@param pt1 First line point.
@param pt2 Second line point.
*/
CV_EXPORTS_W bool clipLine(Rect imgRect, CV_OUT CV_IN_OUT Point &pt1, CV_OUT CV_IN_OUT Point &pt2);

/** @brief Approximates an elliptic arc with a polyline.

The function ellipse2Poly computes the vertices of a polyline that approximates the specified
elliptic arc. It is used by cv::ellipse.

@param center Center of the arc.
@param axes Half of the size of the ellipse main axes. See the ellipse for details.
@param angle Rotation angle of the ellipse in degrees. See the ellipse for details.
@param arcStart Starting angle of the elliptic arc in degrees.
@param arcEnd Ending angle of the elliptic arc in degrees.
@param delta Angle between the subsequent polyline vertices. It defines the approximation
accuracy.
@param pts Output vector of polyline vertices.
 */
CV_EXPORTS_W void ellipse2Poly(
  Point center,
  Size axes,
  int angle,
  int arcStart,
  int arcEnd,
  int delta,
  CV_OUT std::vector<Point> &pts);

/** @overload
@param center Center of the arc.
@param axes Half of the size of the ellipse main axes. See the ellipse for details.
@param angle Rotation angle of the ellipse in degrees. See the ellipse for details.
@param arcStart Starting angle of the elliptic arc in degrees.
@param arcEnd Ending angle of the elliptic arc in degrees.
@param delta Angle between the subsequent polyline vertices. It defines the approximation
accuracy.
@param pts Output vector of polyline vertices.
*/
CV_EXPORTS void ellipse2Poly(
  Point2d center,
  Size2d axes,
  int angle,
  int arcStart,
  int arcEnd,
  int delta,
  CV_OUT std::vector<Point2d> &pts);

/** @brief Draws a text string.

The function putText renders the specified text string in the image. Symbols that cannot be rendered
using the specified font are replaced by question marks. See getTextSize for a text rendering code
example.

@param img Image.
@param text Text string to be drawn.
@param org Bottom-left corner of the text string in the image.
@param fontFace Font type, see cv::HersheyFonts.
@param fontScale Font scale factor that is multiplied by the font-specific base size.
@param color Text color.
@param thickness Thickness of the lines used to draw a text.
@param lineType Line type. See the line for details.
@param bottomLeftOrigin When true, the image data origin is at the bottom-left corner. Otherwise,
it is at the top-left corner.
 */
CV_EXPORTS_W void putText(
  InputOutputArray img,
  const String &text,
  Point org,
  int fontFace,
  double fontScale,
  Scalar color,
  int thickness = 1,
  int lineType = LINE_8,
  bool bottomLeftOrigin = false);

/** @brief Calculates the width and height of a text string.

The function getTextSize calculates and returns the size of a box that contains the specified text.
That is, the following code renders some text, the tight box surrounding it, and the baseline: :
@code
    String text = "Funny text inside the box";
    int fontFace = FONT_HERSHEY_SCRIPT_SIMPLEX;
    double fontScale = 2;
    int thickness = 3;

    Mat img(600, 800, CV_8UC3, Scalar::all(0));

    int baseline=0;
    Size textSize = getTextSize(text, fontFace,
                                fontScale, thickness, &baseline);
    baseline += thickness;

    // center the text
    Point textOrg((img.cols - textSize.width)/2,
                  (img.rows + textSize.height)/2);

    // draw the box
    rectangle(img, textOrg + Point(0, baseline),
              textOrg + Point(textSize.width, -textSize.height),
              Scalar(0,0,255));
    // ... and the baseline first
    line(img, textOrg + Point(0, thickness),
         textOrg + Point(textSize.width, thickness),
         Scalar(0, 0, 255));

    // then put the text itself
    putText(img, text, textOrg, fontFace, fontScale,
            Scalar::all(255), thickness, 8);
@endcode

@param text Input text string.
@param fontFace Font to use, see cv::HersheyFonts.
@param fontScale Font scale factor that is multiplied by the font-specific base size.
@param thickness Thickness of lines used to render the text. See putText for details.
@param[out] baseLine y-coordinate of the baseline relative to the bottom-most text
point.
@return The size of a box that contains the specified text.

@see cv::putText
 */
CV_EXPORTS_W Size getTextSize(
  const String &text, int fontFace, double fontScale, int thickness, CV_OUT int *baseLine);

/** @brief Line iterator

The class is used to iterate over all the pixels on the raster line
segment connecting two specified points.

The class LineIterator is used to get each pixel of a raster line. It
can be treated as versatile implementation of the Bresenham algorithm
where you can stop at each pixel and do some extra processing, for
example, grab pixel values along the line or draw a line with an effect
(for example, with XOR operation).

The number of pixels along the line is stored in LineIterator::count.
The method LineIterator::pos returns the current position in the image:

@code{.cpp}
// grabs pixels along the line (pt1, pt2)
// from 8-bit 3-channel image to the buffer
LineIterator it(img, pt1, pt2, 8);
LineIterator it2 = it;
vector<Vec3b> buf(it.count);

for(int i = 0; i < it.count; i++, ++it)
    buf[i] = *(const Vec3b)*it;

// alternative way of iterating through the line
for(int i = 0; i < it2.count; i++, ++it2)
{
    Vec3b val = img.at<Vec3b>(it2.pos());
    CV_Assert(buf[i] == val);
}
@endcode
*/
class CV_EXPORTS LineIterator {
public:
  /** @brief intializes the iterator

  creates iterators for the line connecting pt1 and pt2
  the line will be clipped on the image boundaries
  the line is 8-connected or 4-connected
  If leftToRight=true, then the iteration is always done
  from the left-most point to the right most,
  not to depend on the ordering of pt1 and pt2 parameters
  */
  LineIterator(
    const Mat &img, Point pt1, Point pt2, int connectivity = 8, bool leftToRight = false);
  /** @brief returns pointer to the current pixel
   */
  uchar *operator*();
  /** @brief prefix increment operator (++it). shifts iterator to the next pixel
   */
  LineIterator &operator++();
  /** @brief postfix increment operator (it++). shifts iterator to the next pixel
   */
  LineIterator operator++(int);
  /** @brief returns coordinates of the current pixel
   */
  Point pos() const;

  uchar *ptr;
  const uchar *ptr0;
  int step, elemSize;
  int err, count;
  int minusDelta, plusDelta;
  int minusStep, plusStep;
};

//! @cond IGNORED

// === LineIterator implementation ===

inline uchar *LineIterator::operator*() { return ptr; }

inline LineIterator &LineIterator::operator++() {
  int mask = err < 0 ? -1 : 0;
  err += minusDelta + (plusDelta & mask);
  ptr += minusStep + (plusStep & mask);
  return *this;
}

inline LineIterator LineIterator::operator++(int) {
  LineIterator it = *this;
  ++(*this);
  return it;
}

inline Point LineIterator::pos() const {
  Point p;
  p.y = (int)((ptr - ptr0) / step);
  p.x = (int)(((ptr - ptr0) - p.y * step) / elemSize);
  return p;
}

//! @endcond

//! @} imgproc_draw

//! @} imgproc

}  // namespace c8cv
