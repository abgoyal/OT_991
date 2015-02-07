

#include "config.h"

#if ENABLE(SVG)
#include "SVGFitToViewBox.h"

#include "AffineTransform.h"
#include "Attr.h"
#include "Document.h"
#include "FloatRect.h"
#include "MappedAttribute.h"
#include "SVGNames.h"
#include "SVGParserUtilities.h"
#include "SVGPreserveAspectRatio.h"
#include "StringImpl.h"

namespace WebCore {

SVGFitToViewBox::SVGFitToViewBox()
{
}

SVGFitToViewBox::~SVGFitToViewBox()
{
}

bool SVGFitToViewBox::parseViewBox(Document* doc, const UChar*& c, const UChar* end, float& x, float& y, float& w, float& h, bool validate)
{
    String str(c, end - c);

    skipOptionalSpaces(c, end);

    bool valid = (parseNumber(c, end, x) && parseNumber(c, end, y) &&
          parseNumber(c, end, w) && parseNumber(c, end, h, false));
    if (!validate)
        return true;
    if (!valid) {
        doc->accessSVGExtensions()->reportWarning("Problem parsing viewBox=\"" + str + "\"");
        return false;
    }

    if (w < 0.0) { // check that width is positive
        doc->accessSVGExtensions()->reportError("A negative value for ViewBox width is not allowed");
        return false;
    } else if (h < 0.0) { // check that height is positive
        doc->accessSVGExtensions()->reportError("A negative value for ViewBox height is not allowed");
        return false;
    } else {
        skipOptionalSpaces(c, end);
        if (c < end) { // nothing should come after the last, fourth number
            doc->accessSVGExtensions()->reportWarning("Problem parsing viewBox=\"" + str + "\"");
            return false;
        }
    }

    return true;
}

AffineTransform SVGFitToViewBox::viewBoxToViewTransform(const FloatRect& viewBoxRect, const SVGPreserveAspectRatio& preserveAspectRatio, float viewWidth, float viewHeight)
{
    if (!viewBoxRect.width() || !viewBoxRect.height())
        return AffineTransform();

    return preserveAspectRatio.getCTM(viewBoxRect.x(), viewBoxRect.y(), viewBoxRect.width(), viewBoxRect.height(), 0, 0, viewWidth, viewHeight);
}

bool SVGFitToViewBox::parseMappedAttribute(Document* document, MappedAttribute* attr)
{
    if (attr->name() == SVGNames::viewBoxAttr) {
        float x = 0.0f, y = 0.0f, w = 0.0f, h = 0.0f;
        const UChar* c = attr->value().characters();
        const UChar* end = c + attr->value().length();
        if (parseViewBox(document, c, end, x, y, w, h))
            setViewBoxBaseValue(FloatRect(x, y, w, h));
        return true;
    } else if (attr->name() == SVGNames::preserveAspectRatioAttr) {
        SVGPreserveAspectRatio::parsePreserveAspectRatio(this, attr->value());
        return true;
    }

    return false;
}

bool SVGFitToViewBox::isKnownAttribute(const QualifiedName& attrName)
{
    return (attrName == SVGNames::viewBoxAttr ||
            attrName == SVGNames::preserveAspectRatioAttr);
}

}

#endif // ENABLE(SVG)
