#assimp - 0458ead

* Partial import of [assimp](https://github.com/assimp/assimp) at commit
  [0458ead](https://github.com/assimp/assimp/commit/0458ead) (2020-12-01)
* Includes common portion and plugins for COLLADA, FBX, and OBJ.
* Additional plugins can be added by copying the appropriate directories into AssetLib
* BSD License


## Import Instructions

1. Check out the source:

```
git clone https://github.com/assimp/assimp.git
cd assimp
git checkout 0458ead
```

2. Set up directory structure

```
mkdir -p path/to/temp_folder/Vendor/assimp-0458ead/include
mkdir -p path/to/temp_folder/Vendor/assimp-0458ead/code/AssetLib
mkdir -p path/to/temp_folder/Vendor/assimp-0458ead/code/PostProcessing
mkdir -p path/to/temp_folder/Vendor/assimp-0458ead/contrib/rapidjson
mkdir -p path/to/temp_folder/Vendor/assimp-0458ead/contrib/pugixml
mkdir -p path/to/temp_folder/Vendor/assimp-0458ead/contrib/unzip
mkdir -p path/to/temp_folder/Vendor/assimp-0458ead/contrib/utf8cpp
```

3. Copy files

```
cp -r include/assimp path/to/temp_folder/Vendor/assimp-0458ead/include/
cp -r contrib/rapidjson/include/rapidjson/* path/to/temp_folder/Vendor/assimp-0458ead/contrib/rapidjson
cp    contrib/pugixml/src/* path/to/temp_folder/Vendor/assimp-0458ead/contrib/pugixml/
cp    contrib/unzip/* path/to/temp_folder/Vendor/assimp-0458ead/contrib/unzip/
cp -r contrib/utf8cpp/source path/to/temp_folder/Vendor/assimp-0458ead/contrib/utf8cpp
cp -r code/Common path/to/temp_folder/Vendor/assimp-0458ead/code/
cp -r code/Material path/to/temp_folder/Vendor/assimp-0458ead/code/
cp    code/PostProcessing/CalcTangentsProcess.* path/to/temp_folder/Vendor/assimp-0458ead/code/PostProcessing/
cp    code/PostProcessing/ConvertToLHProcess.* path/to/temp_folder/Vendor/assimp-0458ead/code/PostProcessing/
cp    code/PostProcessing/JoinVerticesProcess.* path/to/temp_folder/Vendor/assimp-0458ead/code/PostProcessing/
cp    code/PostProcessing/MakeVerboseFormat.* path/to/temp_folder/Vendor/assimp-0458ead/code/PostProcessing/
cp    code/PostProcessing/PretransformVertices.* path/to/temp_folder/Vendor/assimp-0458ead/code/PostProcessing/
cp    code/PostProcessing/ProcessHelper.* path/to/temp_folder/Vendor/assimp-0458ead/code/PostProcessing/
cp    code/PostProcessing/SplitLargeMeshes.* path/to/temp_folder/Vendor/assimp-0458ead/code/PostProcessing/
cp -r code/AssetLib/Collada path/to/temp_folder/Vendor/assimp-0458ead/code/AssetLib/
cp -r code/AssetLib/FBX path/to/temp_folder/Vendor/assimp-0458ead/code/AssetLib/
cp -r code/AssetLib/glTF path/to/temp_folder/Vendor/assimp-0458ead/code/AssetLib/
cp -r code/AssetLib/glTF2 path/to/temp_folder/Vendor/assimp-0458ead/code/AssetLib/
cp -r code/AssetLib/Obj path/to/temp_folder/Vendor/assimp-0458ead/code/AssetLib/
```

4. Generate configuration and copy into the tree

```
mkdir build
cd build
cmake ..
cp revision.h path/to/temp_folder/Vendor/assimp-0458ead/code/Common
cp include/assimp/config.h path/to/temp_folder/Vendor/assimp-0458ead/include/assimp/
```

5. Create `features.h` file and patch it into config.h as in commit
   [633d296](https://github.com/ToolboxAI/scaniverse-app/commit/633d296).

6. Patch the include paths as in commit
   [49122a4](https://github.com/ToolboxAI/scaniverse-app/commit/49122a4).

7. Add to Xcode project
   * Don't include `code/Common/Assimp.cpp`
   * Don't include `contrib/pugixml/pugixml.cpp`
   * Add `$(PROJECT_DIR)/Vendor/assimp-0458ead/include` to **Build Settings > Search Paths > Header Search Paths**.
