

#ifndef SVGMarkerElement_h
#define SVGMarkerElement_h

#if ENABLE(SVG)
#include "RenderObject.h"
#include "SVGAngle.h"
#include "SVGExternalResourcesRequired.h"
#include "SVGFitToViewBox.h"
#include "SVGLangSpace.h"
#include "SVGResourceMarker.h"
#include "SVGStyledElement.h"

namespace WebCore {

    class Document;

    extern char SVGOrientTypeAttrIdentifier[];
    extern char SVGOrientAngleAttrIdentifier[];

    class SVGMarkerElement : public SVGStyledElement,
                             public SVGLangSpace,
                             public SVGExternalResourcesRequired,
                             public SVGFitToViewBox {
    public:
        enum SVGMarkerUnitsType {
            SVG_MARKERUNITS_UNKNOWN           = 0,
            SVG_MARKERUNITS_USERSPACEONUSE    = 1,
            SVG_MARKERUNITS_STROKEWIDTH       = 2
        };

        enum SVGMarkerOrientType {
            SVG_MARKER_ORIENT_UNKNOWN    = 0,
            SVG_MARKER_ORIENT_AUTO       = 1,
            SVG_MARKER_ORIENT_ANGLE      = 2
        };

        SVGMarkerElement(const QualifiedName&, Document*);
        virtual ~SVGMarkerElement();

        AffineTransform viewBoxToViewTransform(float viewWidth, float viewHeight) const;

        void setOrientToAuto();
        void setOrientToAngle(const SVGAngle&);

        virtual void parseMappedAttribute(MappedAttribute*);
        virtual void svgAttributeChanged(const QualifiedName&);
        virtual void synchronizeProperty(const QualifiedName&);
        virtual void childrenChanged(bool changedByParser = false, Node* beforeChange = 0, Node* afterChange = 0, int childCountDelta = 0);

        virtual RenderObject* createRenderer(RenderArena*, RenderStyle*);
        virtual SVGResource* canvasResource(const RenderObject*);

    private:
        DECLARE_ANIMATED_PROPERTY(SVGMarkerElement, SVGNames::refXAttr, SVGLength, RefX, refX)
        DECLARE_ANIMATED_PROPERTY(SVGMarkerElement, SVGNames::refYAttr, SVGLength, RefY, refY)
        DECLARE_ANIMATED_PROPERTY(SVGMarkerElement, SVGNames::markerWidthAttr, SVGLength, MarkerWidth, markerWidth)
        DECLARE_ANIMATED_PROPERTY(SVGMarkerElement, SVGNames::markerHeightAttr, SVGLength, MarkerHeight, markerHeight)
        DECLARE_ANIMATED_PROPERTY(SVGMarkerElement, SVGNames::markerUnitsAttr, int, MarkerUnits, markerUnits)
        DECLARE_ANIMATED_PROPERTY_MULTIPLE_WRAPPERS(SVGMarkerElement, SVGNames::orientAttr, SVGOrientTypeAttrIdentifier, int, OrientType, orientType)
        DECLARE_ANIMATED_PROPERTY_MULTIPLE_WRAPPERS(SVGMarkerElement, SVGNames::orientAttr, SVGOrientAngleAttrIdentifier, SVGAngle, OrientAngle, orientAngle)

        // SVGExternalResourcesRequired
        DECLARE_ANIMATED_PROPERTY(SVGMarkerElement, SVGNames::externalResourcesRequiredAttr, bool, ExternalResourcesRequired, externalResourcesRequired)

        // SVGFitToViewBox
        DECLARE_ANIMATED_PROPERTY(SVGMarkerElement, SVGNames::viewBoxAttr, FloatRect, ViewBox, viewBox)
        DECLARE_ANIMATED_PROPERTY(SVGMarkerElement, SVGNames::preserveAspectRatioAttr, SVGPreserveAspectRatio, PreserveAspectRatio, preserveAspectRatio)
 
        RefPtr<SVGResourceMarker> m_marker;
    };

} // namespace WebCore

#endif // ENABLE(SVG)
#endif
