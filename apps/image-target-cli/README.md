# Image Target CLI

This CLI tool allows the creation of image targets in the format supported by the
[Offline Engine](https://8th.io/xrjs).

## Usage

```
npx @8thwall/image-target-cli@latest
```

You will be prompted to enter an image path, select crop, and choose a folder/image target name. You can either use a default centered crop, or choose your crop dimensions according to this diagram:

![Diagram showing an original image dimension of 2000x2000, then a crop being applied by offsetting the top left corner by the top and left parameters, and reducing the width and height of the crop using width and height parameters](./docs/flat-diagram.jpg)

If you choose a cylindrical geometry, you will also be prompted for the cylinder circumference and target width. See the diagram below for a visualization. The choice of units (mm vs inches) will not affect tracking behavior, so the measurements can also be scale-free.

![3D Visualization showing two views of a cylinder with a label wrapped around the side. One view shows the entire top circle of the cylinder highlighted. The other view shows the top edge of the label highlighted, which is about one third of the full circumference](./docs/cylinder-diagram.jpg) 

For conical images, wider top and wider bottom shapes are both supported. The below image shows the input format, with the label curving around the shape of a large circle. For wider bottom images, the arc of the label should curve the opposite direction. You will be prompted for the outer radius and inner radius of the label's curve. You'll also input the angle of the cone covered by label. If you choose to customize the crop, you'll be cropping against the flattened geometry image, as shown below.

![Diagram showing a conical image target as it is processed. Initially the label is aligned to a section of a ring, with rounded top and bottom edges. The same label is shown flattened to a rectangle, with right angled sides. The flattened label displays a crop being applied by offsetting the top left corner by the top and left parameters, and reducing the width and height of the crop using width and height parameters.](./docs/cone-diagram.jpg) 

On generation, the following will be outputted:

- Metadata within a json file
- Original image
- Geometry image (for conical)
- Cropped image
- Thumbnail image (263x350)
- Luminance image (grayscale, 480x640)

Generated image targets can be loaded into the engine by adding this code to your project:

```
const onxrloaded = () => {
  XR8.XrController.configure({
    imageTargetData: [
      require('../image-targets/target1.json'),
      require('../image-targets/target2.json'),
    ],
  })
}
window.XR8 ? onxrloaded() : window.addEventListener('xrloaded', onxrloaded)
```

The `imagePath` field of the json data will tell the engine where to load the tracked image, which
will have its features extracted and compared to the camera feed.
