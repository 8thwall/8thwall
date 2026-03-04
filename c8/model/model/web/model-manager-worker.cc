#include <emscripten.h>

#include <nlohmann/json.hpp>

#include "c8/hpoint.h"
#include "c8/model/model-manager.h"
#include "c8/model/web/constants.h"
#include "c8/quaternion.h"
#include "c8/stats/scope-timer.h"
#include "c8/symbol-visibility.h"

using namespace c8;

namespace {

struct StaticData {
  ModelManager modelManager;
  Vector<uint8_t> serializedModel;
  int serializedModelSize = 0;
};

StaticData &data() {
  static StaticData data;
  return data;
}

}  // namespace

extern "C" {

C8_PUBLIC
void c8EmAsm_loadModel(char *filename, uint8_t *modelData, int modelSize) {
  {
    ScopeTimer t("load-model");
    {
      ScopeTimer t1("decode-model");
      data().modelManager.loadModel(filename, modelData, modelSize);
    }
    {
      ScopeTimer t1("serialize-model");
      data().serializedModelSize = data().modelManager.serializeModel(data().serializedModel);
    }

    {
      ScopeTimer t1("window-export");
      EM_ASM_(
        {
          self._model8 = {};
          self._model8.modelData = $0;
          self._model8.modelDataSize = $1;
        },
        data().serializedModel.data(),
        data().serializedModelSize);
    }
  }

  if (C8_MODEL_LOG_PERFORMANCE) {
    C8Log("================ WORKER LOAD MODEL ================");
    ScopeTimer::logBriefSummary();
    ScopeTimer::reset();
  }
}

C8_PUBLIC
void c8EmAsm_mergeModel(char *mergeJson, uint8_t *modelData, int modelSize) {
  {
    ScopeTimer t("merge-model");
    {
      ScopeTimer t1("decode-model");
      auto json = nlohmann::json::parse(mergeJson);

      HPoint3 position = {
        json["position"]["x"].get<float>(),
        json["position"]["y"].get<float>(),
        json["position"]["z"].get<float>(),
      };

      Quaternion rotation = {
        json["rotation"]["w"].get<float>(),
        json["rotation"]["x"].get<float>(),
        json["rotation"]["y"].get<float>(),
        json["rotation"]["z"].get<float>(),
      };

      String filename = json["filename"].get<String>();
      data().modelManager.mergeModel(filename, modelData, modelSize, position, rotation);
    }
    {
      ScopeTimer t1("serialize-model");
      data().serializedModelSize = data().modelManager.serializeModel(data().serializedModel);
    }

    {
      ScopeTimer t1("window-export");
      EM_ASM_(
        {
          self._model8 = {};
          self._model8.modelData = $0;
          self._model8.modelDataSize = $1;
        },
        data().serializedModel.data(),
        data().serializedModelSize);
    }
  }

  if (C8_MODEL_LOG_PERFORMANCE) {
    C8Log("================ WORKER MERGE MODEL ================");
    ScopeTimer::logBriefSummary();
    ScopeTimer::reset();
  }
}

C8_PUBLIC
void c8EmAsm_loadModelMultiFile(
  char *filename1,
  uint8_t *modelData1,
  int modelSize1,
  char *filename2,
  uint8_t *modelData2,
  int modelSize2) {
  {
    ScopeTimer t("load-model-multi-file");
    {
      ScopeTimer t1("decode-model");
      data().modelManager.loadModel(
        filename1, modelData1, modelSize1, filename2, modelData2, modelSize2);
    }
    {
      ScopeTimer t1("serialize-model");
      data().serializedModelSize = data().modelManager.serializeModel(data().serializedModel);
    }

    {
      ScopeTimer t1("window-export");
      EM_ASM_(
        {
          self._model8 = {};
          self._model8.modelData = $0;
          self._model8.modelDataSize = $1;
        },
        data().serializedModel.data(),
        data().serializedModelSize);
    }
  }

  if (C8_MODEL_LOG_PERFORMANCE) {
    C8Log("================ WORKER LOAD MODEL ================");
    ScopeTimer::logBriefSummary();
    ScopeTimer::reset();
  }
}

C8_PUBLIC
void c8EmAsm_updateView(char *viewJson) {
  {
    ScopeTimer t("update-view");

    HPoint3 cameraPos;
    Quaternion cameraRot;
    HPoint3 modelPos;
    Quaternion modelRot;
    {
      ScopeTimer t1("json-parse");
      auto json = nlohmann::json::parse(viewJson);

      cameraPos = {
        json["cameraPos"]["x"].get<float>(),
        json["cameraPos"]["y"].get<float>(),
        json["cameraPos"]["z"].get<float>(),
      };

      cameraRot = {
        json["cameraRot"]["w"].get<float>(),
        json["cameraRot"]["x"].get<float>(),
        json["cameraRot"]["y"].get<float>(),
        json["cameraRot"]["z"].get<float>(),
      };

      modelPos = {
        json["modelPos"]["x"].get<float>(),
        json["modelPos"]["y"].get<float>(),
        json["modelPos"]["z"].get<float>(),
      };

      modelRot = {
        json["modelRot"]["w"].get<float>(),
        json["modelRot"]["x"].get<float>(),
        json["modelRot"]["y"].get<float>(),
        json["modelRot"]["z"].get<float>(),
      };
    }

    bool updated = false;
    {
      ScopeTimer t1("model-update");
      updated = data().modelManager.updateView(cameraPos, cameraRot, modelPos, modelRot);
    }

    if (updated) {
      ScopeTimer t1("serialize-model");
      data().serializedModelSize = data().modelManager.serializeModel(data().serializedModel);
    }

    {
      ScopeTimer t1("window-export");
      EM_ASM_(
        {
          self._model8 = {};
          self._model8.updated = $0;
          self._model8.modelData = $1;
          self._model8.modelDataSize = $2;
        },
        updated,
        data().serializedModel.data(),
        data().serializedModelSize);
    }

    if (!updated) {
      // Don't print timing info if we didn't update.
      return;
    }
  }

  if (C8_MODEL_LOG_PERFORMANCE) {
    static int updates = 0;
    if (++updates % 10 == 0) {
      C8Log("================ WORKER UPDATE MODEL ================");
      ScopeTimer::logBriefSummary();
      ScopeTimer::reset();
    }
  }
}

C8_PUBLIC
void c8EmAsm_configure(char *configJson) {
  auto json = nlohmann::json::parse(configJson);
  ModelManagerConfig config;
  if (auto value = json.find("coordinateSpace"); value != json.end()) {
    config.coordinateSpace = static_cast<ModelConfig::CoordinateSpace>(value->get<int>());
  }
  if (auto value = json.find("splatResortRadians"); value != json.end()) {
    config.splatResortRadians = value->get<float>();
  }
  if (auto value = json.find("splatResortMeters"); value != json.end()) {
    config.splatResortMeters = value->get<float>();
  }
  if (auto value = json.find("bakeSkyboxMeters"); value != json.end()) {
    config.bakeSkyboxMeters = value->get<float>();
  }
  if (auto value = json.find("sortTexture"); value != json.end()) {
    config.sortTexture = value->get<bool>();
  }
  if (auto value = json.find("multiTexture"); value != json.end()) {
    config.multiTexture = value->get<bool>();
  }
  // preferTexture defaults to true, only override it if it's explicitly set.
  if (auto value = json.find("preferTexture"); value != json.end()) {
    config.preferTexture = value->get<bool>();
  }
  // useOctree defaults to true, only override it if it's explicitly set.
  if (auto value = json.find("useOctree"); value != json.end()) {
    config.useOctree = value->get<bool>();
  }
  if (auto value = json.find("pointPruneDistance"); value != json.end()) {
    config.pointPruneDistance = value->get<float>();
  }
  if (auto value = json.find("pointMinDistance"); value != json.end()) {
    config.pointMinDistance = value->get<float>();
  }
  if (auto value = json.find("pointFrustumLimit"); value != json.end()) {
    config.pointFrustumLimit = value->get<float>();
  }
  if (auto value = json.find("pointSizeLimit"); value != json.end()) {
    config.pointSizeLimit = value->get<float>();
  }
  data().modelManager.configure(config);
}

}  // EXTERN "C"
