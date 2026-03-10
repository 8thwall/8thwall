// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "openh264-encoder.h",
  };
  visibility = {
    "//visibility:private",
  };
  deps = {
    ":codec-api",
    "//c8/media:muxer",
    "//c8:string",
    "//c8:string-view",
    "//c8:vector",
    "@openh264//:openh264",
    "@json//:json",
  };
}
cc_end(0x9c646b44);

#include "c8/media/codec/openh264-encoder.h"

#include "c8/string.h"
#include "c8/vector.h"
#include "c8/media/muxer.h"

namespace c8 {

namespace {
String parseJsonString(const nlohmann::json &config, const char *field, const char *defaultValue) {
  if (config.count(field) > 0 && config[field].is_string()) {
    return config[field].get<std::string>();
  }
  return defaultValue;
}

int parseJsonInt(const nlohmann::json &config, const char *field, int defaultValue) {
  if (config.count(field) > 0 && config[field].is_number_integer()) {
    return config[field].get<int>();
  }
  return defaultValue;
}

uint64_t parseJsonUint64(const nlohmann::json &config, const char *field, uint64_t defaultValue) {
  if (config.count(field) > 0 && config[field].is_number_integer()) {
    return config[field].get<uint64_t>();
  }
  return defaultValue;
}

uintptr_t parseJsonPtr(const nlohmann::json &config, const char *field, uintptr_t defaultValue) {
  if (config.count(field) > 0 && config[field].is_number_integer()) {
    return config[field].get<uintptr_t>();
  }
  return defaultValue;
}

float parseJsonFloat(const nlohmann::json &config, const char *field, float defaultValue) {
  if (config.count(field) > 0 && config[field].is_number_float()) {
    return config[field].get<float>();
  }
  return defaultValue;
}

bool parseJsonBool(const nlohmann::json &config, const char *field, bool defaultValue) {
  if (config.count(field) > 0 && config[field].is_boolean()) {
    return config[field].get<bool>();
  }
  return defaultValue;
}
}  // namespace

OpenH264Encoder::OpenH264Encoder(const nlohmann::json &config)
    : config_(config),
      muxer_(nullptr),
      encoder_(nullptr),
      lastTimestamp_(0),
      lastFrameType_(videoFrameTypeInvalid),
      width_(0),
      height_(0),
      defaultFrameDuration_(0) {}

OpenH264Encoder::~OpenH264Encoder() {}

MediaStatus OpenH264Encoder::start(Muxer *muxer) {
  if (config_.count("name") == 0 || !config_["name"].is_string()) {
    return {"H.264 encoder must have a string 'name' field"};
  }
  name_ = config_["name"].get<std::string>();

  auto result = WelsCreateSVCEncoder(&encoder_);
  if (result != 0 || encoder_ == nullptr) {
    return {"Could not allocate H.264 Encoder"};
  }

  if (config_.count("fps") > 0) {
    if (config_.count("frameDuration") > 0) {
      return {"H.264 config cannot specify both 'fps' and 'frameDuration'"};
    }
    if (config_.count("timescale") == 0 && !config_.is_number_integer()) {
      return {"H.264 config must specifiy integer 'timescale' field when using 'fps'"};
    }
    if (!config_["fps"].is_number()) {
      return {"H.264 'fps' field must be a number"};
    }
    defaultFrameDuration_ =
      static_cast<uint32_t>(config_["timescale"].get<int>() / config_["fps"].get<double>());
  }
  if (config_.count("frameDuration") > 0) {
    if (config_.count("fps") > 0) {
      return {"H.264 config cannot specify both 'frameDuration' and 'fps'"};
    }
    if (!config_["frameDuration"].is_number()) {
      return {"H.264 'frameDuration' field must be a number"};
    }
    defaultFrameDuration_ = static_cast<uint32_t>(config_["frameDuration"].get<int>());
  }

  width_ = parseJsonInt(config_, "width", 0);
  height_ = parseJsonInt(config_, "height", 0);

  if (width_ == 0 || height_ == 0) {
    return {"H.264 Encoder width and height must be non-zero"};
  }

  SEncParamExt param;
  encoder_->GetDefaultParams(&param);

  param.iUsageType =
    static_cast<EUsageType>(parseJsonInt(config_, "usageType", CAMERA_VIDEO_REAL_TIME));
  param.iLtrMarkPeriod = parseJsonInt(config_, "ltrMarkPeriod", 30);
  param.iPicWidth = width_;
  param.iPicHeight = height_;
  param.iMinQp = parseJsonInt(config_, "minQp", 12);
  param.iMaxQp = parseJsonInt(config_, "maxQp", 42);
  param.iRCMode = static_cast<RC_MODES>(parseJsonInt(config_, "rcMode", RC_TIMESTAMP_MODE));
  param.iTargetBitrate = parseJsonInt(config_, "bitrate", 5000000);

  param.fMaxFrameRate = parseJsonFloat(config_, "framerate", 30.0f);

  param.bEnableDenoise = parseJsonBool(config_, "enableDenoise", false);
  param.bUseLoadBalancing = parseJsonBool(config_, "useLoadBalancing", false);
  param.bEnableSceneChangeDetect = parseJsonBool(config_, "enableSceneChangeDetect", false);
  param.bEnableBackgroundDetection = parseJsonBool(config_, "enableBackgroundDetection", false);
  param.bEnableAdaptiveQuant = parseJsonBool(config_, "enableAdaptiveQuant", false);
  param.bEnableFrameSkip = parseJsonBool(config_, "enableFrameSkip", false);

  param.iMultipleThreadIdc = 1;

  profile_ = static_cast<EProfileIdc>(parseJsonInt(config_, "profile", PRO_BASELINE));
  level_ = static_cast<ELevelIdc>(parseJsonInt(config_, "level", LEVEL_4_2));
  int qp = parseJsonInt(config_, "qp", 23);

  param.iSpatialLayerNum = parseJsonInt(config_, "spatialLayerNum", 1);
  for (int i = 0; i < param.iSpatialLayerNum; i++) {
    param.sSpatialLayers[i].iVideoWidth = width_ >> (param.iSpatialLayerNum - 1 - i);
    param.sSpatialLayers[i].iVideoHeight = height_ >> (param.iSpatialLayerNum - 1 - i);
    param.sSpatialLayers[i].fFrameRate = param.fMaxFrameRate;
    param.sSpatialLayers[i].iSpatialBitrate = param.iTargetBitrate;
    param.sSpatialLayers[i].uiProfileIdc = profile_;
    param.sSpatialLayers[i].uiLevelIdc = level_;
    param.sSpatialLayers[i].iDLayerQp = qp;

    SSliceArgument sliceArg;

    // Use a single slice per frame by default.
    sliceArg.uiSliceMode =
      static_cast<SliceModeEnum>(parseJsonInt(config_, "sliceMode", SM_SINGLE_SLICE));
    sliceArg.uiSliceSizeConstraint = parseJsonInt(config_, "sliceSizeConstraint", 0);
    sliceArg.uiSliceNum = parseJsonInt(config_, "sliceNum", 0);  // Use with SM_FIXEDSLCNUM_SLICE.

    param.sSpatialLayers[i].sSliceArgument = sliceArg;
  }

  param.uiMaxNalSize = parseJsonInt(config_, "maxNalSize", 0);
  param.iTargetBitrate *= param.iSpatialLayerNum;

  // Initialize h.264 encoder.
  if (0 != encoder_->InitializeExt(&param)) {
    return {"H.264 parameter initialization failed"};
  }

  // Set the log level. Default=2 (WARNINGS + ERRORS). Quiet is 0.
  logLevel_ = parseJsonInt(config_, "verbosity", WELS_LOG_DEFAULT);
  encoder_->SetOption(ENCODER_OPTION_TRACE_LEVEL, &logLevel_);

  // Set the video format.
  const String &format = parseJsonString(config_, "format", "yuv420p");
  if (format != "yuv420p") {
    return {"H.264 format must be 'yuv420p', i.e. Planar YUV420"};
  } else {
    videoFormat_ = videoFormatI420;
  }
  encoder_->SetOption(ENCODER_OPTION_DATAFORMAT, &videoFormat_);

  // Add the track to the Muxer.
  muxer_ = muxer;
  return muxer_->addTrack(config_);
}

MediaStatus OpenH264Encoder::flushPreviousFrame(uint32_t duration) {
  nlohmann::json muxerWriteConfig;
  muxerWriteConfig["width"] = width_;
  muxerWriteConfig["height"] = height_;
  muxerWriteConfig["name"] = name_;
  muxerWriteConfig["codec"] = "h264";
  muxerWriteConfig["profile"] = profile_;
  muxerWriteConfig["timestamp"] = lastTimestamp_;
  muxerWriteConfig["frameDuration"] = duration;
  muxerWriteConfig["syncFrame"] = (lastFrameType_ == videoFrameTypeIDR);
  muxerWriteConfig["type"] = "sample";
  auto writeResult = muxer_->write(muxerWriteConfig, outputBuffer_.data(), outputBuffer_.size());
  outputBuffer_.clear();
  if (writeResult.code() != 0) {
    return writeResult;
  }
  return {};
}

MediaStatus OpenH264Encoder::encode(
  const nlohmann::json &sampleConfig, const uint8_t *data, size_t byteSize) {
  uint64_t timestamp = parseJsonUint64(sampleConfig, "timestamp", 0);
  if (!outputBuffer_.empty()) {
    if (sampleConfig.count("timestamp") == 0) {
      return {"H.264 using variable framerate but integer timestamp is missing"};
    }

    if (timestamp < lastTimestamp_) {
      // TODO(mc): Allow B-frames with rendering offsets.
      return {"H.264 timestamps must be monotonically increasing"};
    }

    // Figure out the duration of the previous frame from the differences in
    // timestamps.
    int duration = timestamp - lastTimestamp_;

    // Flush the previous frame if we were delaying write until we could
    // determine the variable frame duration.
    auto rv = flushPreviousFrame(duration);
    if (rv.code() != 0) {
      return rv;
    }
  }

  uint32_t frameDuration = parseJsonInt(sampleConfig, "frameDuration", 0);

  SFrameBSInfo info;
  memset(&info, 0, sizeof(SFrameBSInfo));
  SSourcePicture pic;
  memset(&pic, 0, sizeof(SSourcePicture));

  if (sampleConfig.count("timestamp") > 0) {
    // Explicitly encode the timestamp if it was provided.
    pic.uiTimeStamp = timestamp;
  }

  int sampleWidth = parseJsonInt(sampleConfig, "width", 0);
  int sampleHeight = parseJsonInt(sampleConfig, "height", 0);

  if (sampleWidth != 0 && sampleWidth != width_) {
    return {"H.264 frame width cannot change between frames"};
  }
  if (sampleHeight != 0 && sampleHeight != height_) {
    return {"H.264 frame height cannot change between frames"};
  }

  pic.iPicWidth = width_;
  pic.iPicHeight = height_;
  pic.iColorFormat = videoFormat_;
  pic.iStride[0] = parseJsonInt(sampleConfig, "rowBytes0", 0);
  pic.iStride[1] = parseJsonInt(sampleConfig, "rowBytes1", 0);
  pic.iStride[2] = parseJsonInt(sampleConfig, "rowBytes2", 0);

  uintptr_t data0 = parseJsonPtr(sampleConfig, "data0", 0);
  uintptr_t data1 = parseJsonPtr(sampleConfig, "data1", 0);
  uintptr_t data2 = parseJsonPtr(sampleConfig, "data2", 0);

  if (data0 == 0 || data1 == 0 || data2 == 0) {
    return {"H.264 frame missing some of 'data0', 'data1', 'data2' params"};
  }

  pic.pData[0] = reinterpret_cast<unsigned char *>(data0);
  pic.pData[1] = reinterpret_cast<unsigned char *>(data1);
  pic.pData[2] = reinterpret_cast<unsigned char *>(data2);

  auto result = encoder_->EncodeFrame(&pic, &info);
  lastFrameType_ = info.eFrameType;

  if (result != cmResultSuccess) {
    return {"H.264 encoding failed"};
  }

  if (info.eFrameType == videoFrameTypeSkip) {
    return {};
  }

  nlohmann::json muxerWriteConfig;
  muxerWriteConfig["width"] = width_;
  muxerWriteConfig["height"] = height_;
  muxerWriteConfig["name"] = name_;
  muxerWriteConfig["codec"] = "h264";
  muxerWriteConfig["profile"] = profile_;
  muxerWriteConfig["syncFrame"] = (lastFrameType_ == videoFrameTypeIDR);

  // output bitstream
  for (int k = 0; k < info.iLayerNum; ++k) {
    const SLayerBSInfo &layerInfo = info.sLayerInfo[k];
    const uint8_t *nalu = layerInfo.pBsBuf;
    for (int j = 0; j < layerInfo.iNalCount; ++j) {
      if (0 != strncmp(reinterpret_cast<const char *>(nalu), "\0\0\0\x01", 4)) {
        // Assert the nalu starts with a start code.
        return {"H.264 encountered bad NAL unit start code"};
      }

      // Advance the nalu past the start code.
      nalu += 4;
      const size_t naluSize = layerInfo.pNalLengthInByte[j] - 4;
      const uint8_t naluType = nalu[0] & 0b00011111;

      switch (naluType) {
        // SPS unit.
        case 7: {
          // SPS
          muxerWriteConfig["type"] = "sps";
          auto writeResult = muxer_->write(muxerWriteConfig, nalu, naluSize);
          if (writeResult.code() != 0) {
            return writeResult;
          }
          break;
        }
        // PPS unit.
        case 8: {
          // PPS
          muxerWriteConfig["type"] = "pps";
          auto writeResult = muxer_->write(muxerWriteConfig, nalu, naluSize);
          if (writeResult.code() != 0) {
            return writeResult;
          }
          break;
        }
        default: {
          outputBuffer_.push_back((naluSize >> 24) & 0xff);
          outputBuffer_.push_back((naluSize >> 16) & 0xff);
          outputBuffer_.push_back((naluSize >> 8) & 0xff);
          outputBuffer_.push_back((naluSize >> 0) & 0xff);
          outputBuffer_.insert(outputBuffer_.end(), nalu, nalu + naluSize);
          break;
        }
      }
      nalu += naluSize;
    }
    if (!outputBuffer_.empty()) {
      if (sampleConfig.count("timestamp") > 0) {
        // If a timestamp was provided to the encoder, save it and wait until
        // the next frame to flush it to the muxer, when we know its duration.
        lastTimestamp_ = info.uiTimeStamp;
        lastDuration_ = (frameDuration > 0) ? frameDuration : defaultFrameDuration_;
      } else {
        // This is a variable rate frame.
        muxerWriteConfig["type"] = "sample";
        if (frameDuration > 0) {
          // Include a frameDuration if explicitly set.
          muxerWriteConfig["frameDuration"] = frameDuration;
        }
        auto writeResult =
          muxer_->write(muxerWriteConfig, outputBuffer_.data(), outputBuffer_.size());
        outputBuffer_.clear();
        if (writeResult.code() != 0) {
          return writeResult;
        }
      }
    }
  }

  return {};
}

MediaStatus OpenH264Encoder::finish() {
  if (!outputBuffer_.empty()) {
    // Flush the previous frame if we were delaying write until we could
    // determine the variable frame duration.
    auto rv = flushPreviousFrame(lastDuration_);
    if (rv.code() != 0) {
      return rv;
    }
  }

  if (encoder_) {
    encoder_->Uninitialize();
    WelsDestroySVCEncoder(encoder_);
    encoder_ = nullptr;
  }

  muxer_ = nullptr;

  return {};
}

}  // namespace c8
