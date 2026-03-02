@0xeda784665198fad9;

using import "id.capnp".EventId;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("c8");

using Java = import "/capnp/java.capnp";
$Java.package("com.the8thwall.reality.engine.api.base");
$Java.outerClassname("GeoTypes");  # Must match this file's name!

struct Quaternion32f {
  w @0 :Float32;
  x @1 :Float32;
  y @2 :Float32;
  z @3 :Float32;
}

struct Position32f {
  x @0 :Float32;
  y @1 :Float32;
  z @2 :Float32;
}

struct HMatrix32f {
  # A column-major 4x4 matrix and its inverse
  m0 @0 :Float32;
  m1 @1 :Float32;
  m2 @2 :Float32;
  m3 @3 :Float32;
  m4 @4 :Float32;
  m5 @5 :Float32;
  m6 @6 :Float32;
  m7 @7 :Float32;
  m8 @8 :Float32;
  m9 @9 :Float32;
  m10 @10 :Float32;
  m11 @11 :Float32;
  m12 @12 :Float32;
  m13 @13 :Float32;
  m14 @14 :Float32;
  m15 @15 :Float32;
  invM0 @16 :Float32;
  invM1 @17 :Float32;
  invM2 @18 :Float32;
  invM3 @19 :Float32;
  invM4 @20 :Float32;
  invM5 @21 :Float32;
  invM6 @22 :Float32;
  invM7 @23 :Float32;
  invM8 @24 :Float32;
  invM9 @25 :Float32;
  invM10 @26 :Float32;
  invM11 @27 :Float32;
  invM12 @28 :Float32;
  invM13 @29 :Float32;
  invM14 @30 :Float32;
  invM15 @31 :Float32;
}

struct Box32f {
  x @0 :Float32;
  y @1 :Float32;
  w @2 :Float32;
  h @3 :Float32;
}

struct ImagePoint32f {
  x @0 :Float32;
  y @1 :Float32;
}

# Corners of a logical box in an image, may be rotated or have other transforms applied.
struct BoxCorners {
  upperLeft @0: ImagePoint32f;
  upperRight @1: ImagePoint32f;
  lowerLeft @2: ImagePoint32f;
  lowerRight @3: ImagePoint32f;
}

struct ImageBoxRoi {
  # The corners of a bounding box in 0-1 in a rendered roi space. The box may be rotated or have
  # other transforms applied.
  corners @0: BoxCorners;

  # A column-major 4x4 matrix for converting box corners (0-1) into image coordinates (0-1).
  renderTexToImageTexMatrix44f @1: List(Float32);
}

struct Transform32f {
  position @0 :Position32f;
  rotation @1 :Quaternion32f;
}

struct SurfaceSet {
  # A surface is a collection of faces, specified by a range into the faces array. The faces are
  # composed of vertices, specified by the vertices array.
  surfaces @0 :List(Surface);

  # A face is three vertices in front-clockwise winding order. The vertices are specified as indexes
  # into the vertices array. The faces are arranged in contiguous blocks by surface.
  faces @1 :List(SurfaceFace);

  # A collection of all of the vertices contained in all of the surfaces.  The vertices are arranged
  # in contiguous blocks by surface.
  vertices @2 :List(SurfaceVertex);

  # A collection of all of the vertices contained in all of the boundaries.  The vertices are arranged
  # in contiguous blocks by boundary.
  boundaryVertices @3 :List(SurfaceVertex);

  # A collection of all of the texture coordinates contained in all of the surfaces.  The texture coord
  # maps to a data on a texture for each vertex.
  textureCoords @4 :List(SurfaceTextureCoord);
}

struct Surface {
  enum SurfaceType {
    unspecified @0;
    horizontalPlane @1;
    verticalPlane @2;
  }
  # Surfaces have a unique id so that they can be reidentified across calls.
  id @0: EventId;

  # A surface contains a set of faces in a contiguous block of the `faces` array. The begin index is
  # inclusive, the end index is exclusive.
  facesBeginIndex @1: Int32;
  facesEndIndex @2: Int32;

  # A surface contains a set of vertices in a contiguous block of the `vertices` array. The begin index is
  # inclusive, the end index is exclusive.
  verticesBeginIndex @3: Int32;
  verticesEndIndex @4: Int32;

  # A surface type allows users to control virtual object interactions
  # e.g. a vase will only be placed on a HORIZONTAL_PLANE surface type surface
  surfaceType @5: SurfaceType;

  # A boundary contains a set of vertices in a contiguous block of the `boundaryVertices` array. The begin index is
  # inclusive, the end index is exclusive.
  boundariesBeginIndex @6: Int32;
  boundariesEndIndex @7: Int32;

  # A boundary contains a set of vertices in a contiguous block of the `textureCoords` array. The begin index is
  # inclusive, the end index is exclusive.
  textureCoordsBeginIndex @8: Int32;
  textureCoordsEndIndex @9: Int32;

  # A normal vector located at some point along the plane
  # On vertical surfaces, this allow you to rotate a horizontal plane to match the orientation of this surface
  normal @10: Transform32f;
}

struct SurfaceVertex {
  x @0: Float32;
  y @1: Float32;
  z @2: Float32;
}

struct SurfaceTextureCoord {
  # UV texture coordinates for a vertex. This coordinate indicates that the data at (u,v) in the
  # associated texture is present for this vertex.
  u @0: Float32;
  v @1: Float32;
}

struct SurfaceFace {
  # Integer indices into the vertices array, specified in front-clockwise winding order, i.e.
  # normal = cross(v1 - v0, v2 - v0);
  v0 @0: Int32;
  v1 @1: Int32;
  v2 @2: Int32;
}

struct ActiveSurface {
  # Surfaces have a unique id so that they can be reidentified across calls.
  id @0: EventId;
  activePoint @1: Position32f;
}
