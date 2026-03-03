// Copyright (c) 2024 Niantic, Inc.
// Original Author: Nicholas Butko (nbutko@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "crop-box.h",
  };
  deps = {
    "//c8:color",
    "//c8:hpoint",
    "//c8:vector",
    "//c8/geometry:box3",
    "//c8/model/shaders:model-shaders",
    "//c8/pixels/render:hit-test",
    "//c8/pixels/render:object8",
    "//c8/pixels/render:renderer",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x389fd13f);

#include "c8/color.h"
#include "c8/hpoint.h"
#include "c8/model/crop-box.h"
#include "c8/model/shaders/model-shaders.h"
#include "c8/pixels/render/hit-test.h"
#include "c8/vector.h"

namespace c8 {

namespace {
const Color CROPBOX_COLOR = Color::WHITE;
const float CROPBOX_LINE_WIDTH = 0.005f;            // % of min dimension
const float CROPBOX_POINT_SIZE_RATIO = 6.0f;        // compared to line width
const float CROPBOX_DRAG_HANDLE_SIZE_RATIO = 4.0f;  // compared to point size
}  // namespace

CropBox::CropBox(Box3 bounds, const Scene *scene, Renderer *renderer) {
  // Register the custom shader with the renderer.
  // NOTE: After the first time this shader is registered, this line becomes a no-op.
  renderer->registerShader(
    "cropbox-cutout",
    embeddedCropboxCutoutVertCStr,
    embeddedCropboxCutoutFragCStr,
    {Shaders::POSITION},
    {{"elliptical"}, {"bounds"}});

  bounds_ = bounds;
  scene_ = scene;

  // Add a subscene to render the crop box. This has the same resolution as the main scene and a
  // transparent background.
  auto rWidth = scene_->renderSpec().resolutionWidth;
  auto rHeight = scene_->renderSpec().resolutionHeight;
  auto &boxScene = this->add(ObGen::subScene("boxScene", {{rWidth, rHeight}}));
  boxScene.renderSpec().clearColor = Color::TRUE_BLACK.alpha(0);  // (0, 0, 0, 0)

  // Add a pixel camera and the drag handles / crop borders in the scene with the crop box.
  auto &boxCamera = boxScene.add(ObGen::pixelCamera(rWidth, rHeight));
  boxCamera.flipY();  // ObGen::pixelCamera is currently buggy and flips the Y axis, fix it here.

  dragHandles_ = &boxScene.add(ObGen::named(ObGen::pixelPoints(), "cropBoxCorners"));
  dragHandles_->material().setTransparent(true).setOpacity(0.0f);
  cropPoints_ = &boxScene.add(ObGen::pixelPoints());
  cropBorders_ = &boxScene.add(ObGen::pixelLines());
  cropCircle_ = &boxScene.add(ObGen::pixelLines());

  // In the main scene, add a quad placed over the whole scene in the front. This will render the
  // content from the box scene as a flat image overlayed on other 3d content.
  auto &overlayQuad = this->add(ObGen::frontQuad());
  overlayQuad.setMaterial(MatGen::transparent(MatGen::subsceneMaterial("boxScene")));

  auto &cutoutQuad = boxScene.add(ObGen::named(ObGen::backQuad(), "cutout"));
  auto cutoutMaterial = MatGen::empty();
  cutoutMaterial->setShader("cropbox-cutout")
    .setTransparent(true)
    .setRenderSide(RenderSide::FRONT)
    .set("elliptical", 0.0f)
    .set("bounds", std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f});
  cutoutQuad.setMaterial(std::move(cutoutMaterial));

  viewFrom(Side::TOP);
}

void CropBox::updateCropBox() {
  // Find the corners of the box for the current view in the model space.
  Vector<HPoint3> corners;
  switch (side_) {
    case Side::TOP:
      corners = {
        HPoint3(bounds_.min.x(), bounds_.max.y(), bounds_.min.z()),  // Front Left, bl
        HPoint3(bounds_.max.x(), bounds_.max.y(), bounds_.min.z()),  // Front Right, br
        HPoint3(bounds_.max.x(), bounds_.max.y(), bounds_.max.z()),  // Back Right, tr
        HPoint3(bounds_.min.x(), bounds_.max.y(), bounds_.max.z()),  // Back Left, tl
      };
      break;
    case Side::LEFT:
      corners = {
        HPoint3(bounds_.min.x(), bounds_.max.y(), bounds_.max.z()),  // Top Back, tl
        HPoint3(bounds_.min.x(), bounds_.max.y(), bounds_.min.z()),  // Top Front, tr
        HPoint3(bounds_.min.x(), bounds_.min.y(), bounds_.min.z()),  // Bottom Front, br
        HPoint3(bounds_.min.x(), bounds_.min.y(), bounds_.max.z()),  // Bottom Back, bl
      };
      break;
    case Side::RIGHT:
      corners = {
        HPoint3(bounds_.max.x(), bounds_.max.y(), bounds_.max.z()),  // Top Back, tr
        HPoint3(bounds_.max.x(), bounds_.max.y(), bounds_.min.z()),  // Top Front, tl
        HPoint3(bounds_.max.x(), bounds_.min.y(), bounds_.min.z()),  // Bottom Front, bl
        HPoint3(bounds_.max.x(), bounds_.min.y(), bounds_.max.z()),  // Bottom Back, br
      };
      break;
    case Side::FRONT:
      corners = {
        HPoint3(bounds_.min.x(), bounds_.max.y(), bounds_.min.z()),  // Top Left, tl
        HPoint3(bounds_.max.x(), bounds_.max.y(), bounds_.min.z()),  // Top Right, tr
        HPoint3(bounds_.max.x(), bounds_.min.y(), bounds_.min.z()),  // Bottom Right, br
        HPoint3(bounds_.min.x(), bounds_.min.y(), bounds_.min.z()),  // Bottom Left, bl
      };
      break;
    default:
      break;
  }

  // Find the x/y corners of the box face in the current image in clip space.
  const auto &sceneCam = scene_->find<Camera>();
  auto cornerClip = (sceneCam.projection() * sceneCam.world().inv() * this->world()) * corners;
  Vector<HPoint3> cornerClip2 = {
    HPoint3{cornerClip[0].x(), cornerClip[0].y(), 0.0f},
    HPoint3{cornerClip[1].x(), cornerClip[1].y(), 0.0f},
    HPoint3{cornerClip[2].x(), cornerClip[2].y(), 0.0f},
    HPoint3{cornerClip[3].x(), cornerClip[3].y(), 0.0f},
  };

  // Find the pixel coordinates of the corners in the image.
  auto &boxScene = this->find<Scene>("boxScene");
  auto cornerPix3 = boxScene.find<Camera>().projection().inv() * cornerClip2;
  Vector<HPoint2> cornerPix = {
    HPoint2{cornerPix3[0].x(), cornerPix3[0].y()},
    HPoint2{cornerPix3[1].x(), cornerPix3[1].y()},
    HPoint2{cornerPix3[2].x(), cornerPix3[2].y()},
    HPoint2{cornerPix3[3].x(), cornerPix3[3].y()},
  };

  // Connect the corners to form a crop box.
  auto bars = {
    std::pair<HPoint2, HPoint2>(cornerPix[0], cornerPix[1]),
    std::pair<HPoint2, HPoint2>(cornerPix[1], cornerPix[2]),
    std::pair<HPoint2, HPoint2>(cornerPix[2], cornerPix[3]),
    std::pair<HPoint2, HPoint2>(cornerPix[3], cornerPix[0]),
  };

  // Scale points and lines to be an appropriate size for the current image.
  auto pixWidth = boxScene.renderSpec().resolutionWidth;
  auto pixHeight = boxScene.renderSpec().resolutionHeight;
  auto minWidth = std::min(pixWidth, pixHeight);
  auto lineWidth = std::max(3.0f, CROPBOX_LINE_WIDTH * minWidth);
  auto pointSize = lineWidth * CROPBOX_POINT_SIZE_RATIO;
  auto dragHandleSize = pointSize * CROPBOX_DRAG_HANDLE_SIZE_RATIO;

  // Update the location, size, and color of the crop box handles and borders.
  ObGen::updatePixelPoints(
    dragHandles_, cornerPix, CROPBOX_COLOR, pixWidth, pixHeight, dragHandleSize);
  ObGen::updatePixelPoints(cropPoints_, cornerPix, CROPBOX_COLOR, pixWidth, pixHeight, pointSize);
  ObGen::updatePixelLines(cropBorders_, bars, CROPBOX_COLOR, pixWidth, pixHeight, lineWidth);

  // Set the cutout bounds in the shader. Only render as elliptical from the top.
  auto cx = 0.5f * (cornerClip2[0].x() + cornerClip2[1].x());
  auto cy = 0.5f * (cornerClip2[0].y() + cornerClip2[2].y());
  auto rx = 0.5f * std::abs(cornerClip2[1].x() - cornerClip2[0].x());
  auto ry = 0.5f * std::abs(cornerClip2[2].y() - cornerClip2[0].y());
  auto showEllipse = elliptical_ && side_ == Side::TOP;
  auto &cutout = boxScene.find<Renderable>("cutout");
  cutout.material().set("bounds", std::array<float, 4>{cx, cy, rx, ry}).set("elliptical", showEllipse ? 1.0f : 0.0f);

  // Update the crop circle if needed.
  if (showEllipse) {
    cropCircle_->setEnabled(true);
    Vector<std::pair<HPoint2, HPoint2>> circlePoints{360};
    HPoint2 lastPoint = {
      (((cx + rx * std::cos(0.0f)) + 1.0f) * 0.5f) * pixWidth,
      (1.0f - ((cy + ry * std::sin(0.0f)) + 1.0f) * 0.5f) * pixHeight};
    for (int i = 0; i < 360; i++) {
      float angle = (i + 1) * M_PI / 180.0f;
      HPoint2 nextPoint = {
        (((cx + rx * std::cos(angle)) + 1.0f) * 0.5f) * pixWidth,
        (1.0f - ((cy + ry * std::sin(angle)) + 1.0f) * 0.5f) * pixHeight};
      circlePoints[i] = {lastPoint, nextPoint};
      lastPoint = nextPoint;
    }
    ObGen::updatePixelLines(
      cropCircle_, circlePoints, CROPBOX_COLOR, pixWidth, pixHeight, lineWidth);
  } else {
    cropCircle_->setEnabled(false);
  }
}

int CropBox::hitTestClipSpace(HVector2 positionClip) {
  auto &camera = this->find<Scene>("boxScene").find<Camera>();
  auto hits = hitTestRenderable(*dragHandles_, camera, positionClip);
  if (hits.empty()) {
    return -1;
  }
  return hits[0].elementIndex;
}

void CropBox::viewFrom(Side side) {
  side_ = side;
  updateCropBox();
}

void CropBox::update(Box3 bounds) {
  bounds_ = bounds;
  updateCropBox();
}

void CropBox::tick(double frameTimeMillis) { updateCropBox(); }

void CropBox::update(float xClip, float yClip, int32_t cornerIndex) {
  const auto &camera = scene_->find<Camera>();

  // Find the distance from the camera to the closest crop box face.
  auto mv = camera.world().inv() * world();
  float distance =
    (side_ == Side::TOP || side_ == Side::RIGHT) ? (mv * bounds_.max).z() : (mv * bounds_.min).z();

  // Find the clip coordinate point in 3d from the camera's point of view, at distance 1.
  auto camPoint2 = (camera.projection().inv() * HPoint3{xClip, yClip, 0.0f}).flatten();

  // Find the clip coordinate point in 3d from the camera's point of view at the dist of the box.
  auto camPoint3 = HPoint3{camPoint2.x() * distance, camPoint2.y() * distance, distance};

  // Find the new 3d point in model coordinates.
  auto newCorner = mv.inv() * camPoint3;

  auto bounds = bounds_;
  switch (side_) {
    case Side::TOP:
      if (cornerIndex == 0) {  // Top Front Left
        bounds.min = {newCorner.x(), bounds_.min.y(), newCorner.z()};
        bounds.max = {bounds_.max.x(), bounds_.max.y(), bounds_.max.z()};
      } else if (cornerIndex == 1) {  // Top Front Right
        bounds.min = {bounds_.min.x(), bounds_.min.y(), newCorner.z()};
        bounds.max = {newCorner.x(), bounds_.max.y(), bounds_.max.z()};
      } else if (cornerIndex == 2) {  // Top Back Right
        bounds.min = {bounds_.min.x(), bounds_.min.y(), bounds_.min.z()};
        bounds.max = {newCorner.x(), bounds_.max.y(), newCorner.z()};
      } else if (cornerIndex == 3) {  // Top Back Left
        bounds.min = {newCorner.x(), bounds_.min.y(), bounds_.min.z()};
        bounds.max = {bounds_.max.x(), bounds_.max.y(), newCorner.z()};
      }
      break;
    case Side::LEFT:
      if (cornerIndex == 0) {  // Left Top Back
        bounds.min = {bounds_.min.x(), bounds_.min.y(), bounds_.min.z()};
        bounds.max = {bounds_.max.x(), newCorner.y(), newCorner.z()};
      } else if (cornerIndex == 1) {  // Left Top Front
        bounds.min = {bounds_.min.x(), bounds_.min.y(), newCorner.z()};
        bounds.max = {bounds_.max.x(), newCorner.y(), bounds_.max.z()};
      } else if (cornerIndex == 2) {  // Left Bottom Front
        bounds.min = {bounds_.min.x(), newCorner.y(), newCorner.z()};
        bounds.max = {bounds_.max.x(), bounds_.max.y(), bounds_.max.z()};
      } else if (cornerIndex == 3) {  // Left Bottom Back
        bounds.min = {bounds_.min.x(), newCorner.y(), bounds_.min.z()};
        bounds.max = {bounds_.max.x(), bounds_.max.y(), newCorner.z()};
      }
      break;
    case Side::RIGHT:
      if (cornerIndex == 0) {  // Right Top Back
        bounds.min = {bounds_.min.x(), bounds_.min.y(), bounds_.min.z()};
        bounds.max = {bounds_.max.x(), newCorner.y(), newCorner.z()};
      } else if (cornerIndex == 1) {  // Right Top Front
        bounds.min = {bounds_.min.x(), bounds_.min.y(), newCorner.z()};
        bounds.max = {bounds_.max.x(), newCorner.y(), bounds_.max.z()};
      } else if (cornerIndex == 2) {  // Right Bottom Front
        bounds.min = {bounds_.min.x(), newCorner.y(), newCorner.z()};
        bounds.max = {bounds_.max.x(), bounds_.max.y(), bounds_.max.z()};
      } else if (cornerIndex == 3) {  // Right Bottom Back
        bounds.min = {bounds_.min.x(), newCorner.y(), bounds_.min.z()};
        bounds.max = {bounds_.max.x(), bounds_.max.y(), newCorner.z()};
      }
      break;
    case Side::FRONT:
      if (cornerIndex == 0) {  // Front Top Left
        bounds.min = {newCorner.x(), bounds_.min.y(), bounds_.min.z()};
        bounds.max = {bounds_.max.x(), newCorner.y(), bounds_.max.z()};
      } else if (cornerIndex == 1) {  // Front Top Right
        bounds.min = {bounds_.min.x(), bounds_.min.y(), bounds_.min.z()};
        bounds.max = {newCorner.x(), newCorner.y(), bounds_.max.z()};
      } else if (cornerIndex == 2) {  // Front Bottom Right
        bounds.min = {bounds_.min.x(), newCorner.y(), bounds_.min.z()};
        bounds.max = {newCorner.x(), bounds_.max.y(), bounds_.max.z()};
      } else if (cornerIndex == 3) {  // Front Bottom Left
        bounds.min = {newCorner.x(), newCorner.y(), bounds_.min.z()};
        bounds.max = {bounds_.max.x(), bounds_.max.y(), bounds_.max.z()};
      }
      break;
  }
  if (bounds.dimensions().x() > 0 && bounds.dimensions().y() > 0 && bounds.dimensions().z() > 0) {
    bounds_ = bounds;
    updateCropBox();
  }
}

Box3 CropBox::bounds() const { return bounds_; }

}  // namespace c8
