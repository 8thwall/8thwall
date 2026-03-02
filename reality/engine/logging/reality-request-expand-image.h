namespace c8 {

// Replace requestOut's getSensors().getCamera().getCurrentFrame() with a frame that contains the
// image pixels instead of just a point to the data
void expandImagePtrsToImages(const RealityRequest::Reader &request, RealityRequest::Builder requestOut);


// Replace requestOut's getSensors().getCamera().getCurrentFrame() with a frame that contains the
// image pixels as a jpg instead of just a point to the data
void expandImagePtrsToJpg(const RealityRequest::Reader &request, RealityRequest::Builder requestOut);

}
