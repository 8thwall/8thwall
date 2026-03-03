#!/bin/bash

MANIFEST=$1
CLASSES_JAR=$2
APK=$3
PROGUARD=$4
OUTPUT_DIR=$5
OUTPUT_NAME=$6
MANIFEST_MERGER_JAR=$7
AAR_SRC_NAME=$OUTPUT_DIR/aar_dir

# Strip any .so files that weren't imported by libraries.
STRIP_NATIVE_LIBS=${STRIP_NATIVE:-0}

# Only keep these architectures.
ARCHS_TO_KEEP=${ARCHS}

function unpack_aar_resources_and_libs {
  manifest_merger_jar=$1
  main_manifest=$2
  aar=$3
  out_dir=$4

  rm -r tmp_unpacked &> /dev/null
  mkdir tmp_unpacked

  # Unzip contents we'd like to merge to our main AAR.
  unzip $aar assets/** AndroidManifest.xml jni/** res/** -d tmp_unpacked/ &> /dev/null

  # If a list of archs is provided, only include those. Otherwise include all.
  JNI_TMP=tmp_unpacked/jni
  if [[ $ARCHS_TO_KEEP != "None" ]]; then
    ls $JNI_TMP | while read archDir ; do
      if [[ $ARCHS_TO_KEEP != *"$archDir"* ]]; then
        rm -r "$JNI_TMP/$archDir"
      fi
    done
  fi

  if [ -d "tmp_unpacked/jni/" ]; then
    cp -nR tmp_unpacked/jni/ $out_dir/jni
    if [ $? -ne 0 ]; then
      echo "Error: Attempting to merge duplicate jni library."
      exit 1
    fi
  fi

  if [ -d "tmp_unpacked/res/" ]; then
    cp -nR tmp_unpacked/res/ $out_dir/res
    if [ $? -ne 0 ]; then
      echo "Error: Attempting to merge duplicate res file."
      exit 1
    fi
  fi

  if [ -d "tmp_unpacked/assets/" ]; then
    cp -nR tmp_unpacked/assets/ $out_dir/assets
    if [ $? -ne 0 ]; then
      echo "Error: Attempting to merge duplicate assets file."
      exit 1
    fi
  fi

  # Merge the lib manifest into the main manifest.
  java -jar $manifest_merger_jar \
    --main $main_manifest \
    --libs tmp_unpacked/AndroidManifest.xml \
    --out $main_manifest &> /dev/null

  # Clean up tmp directories created.
  rm -r tmp_unpacked/ &> /dev/null
}

# Construct the src files under the same directory.
rm -r $AAR_SRC_NAME &> /dev/null
mkdir $AAR_SRC_NAME
touch $AAR_SRC_NAME/R.txt
touch $AAR_SRC_NAME/public.txt
mkdir $AAR_SRC_NAME/res
mkdir $AAR_SRC_NAME/jni
cp $MANIFEST $AAR_SRC_NAME/AndroidManifest.xml
cp $CLASSES_JAR $AAR_SRC_NAME/classes.jar
cp $PROGUARD $AAR_SRC_NAME/proguard.txt

# Keep this in sync w/ the number of params listed at the top of the file.
for MERGE_AAR in "${@:8}" ; do
  unpack_aar_resources_and_libs $MANIFEST_MERGER_JAR $AAR_SRC_NAME/AndroidManifest.xml $MERGE_AAR $AAR_SRC_NAME
done

# Copy over assets and .so files that should be packaged into the AAR.
unzip $APK assets/** lib/** -d $AAR_SRC_NAME/ &> /dev/null
cp -R $AAR_SRC_NAME/lib/ $AAR_SRC_NAME/jni

if [ $STRIP_NATIVE_LIBS != "0" ]; then
  # Strip any .so files that weren't imported by libraries.
  find $AAR_SRC_NAME/jni -name '*-apk.so' | while read file; do
    rm $file
  done
else
  # Remove the '-apk' suffix from any native libraries.
  find $AAR_SRC_NAME/jni -name '*-apk.so' | while read file; do
    mv "$file" "${file/-apk}"
  done
fi

# Delete any empty directories left over from the previous deletions.
find $AAR_SRC_NAME/jni -type d -empty | while read dir; do
  rm -r $dir
done

# Delete jni directory if empty.
if [ -z "$(ls -A $AAR_SRC_NAME/jni)" ]; then
  rm -r $AAR_SRC_NAME/jni
fi

# Create the zip.
pushd $AAR_SRC_NAME &> /dev/null
zip -r $OUTPUT_NAME R.txt public.txt assets/ res/ AndroidManifest.xml classes.jar jni/ proguard.txt &> /dev/null
popd &> /dev/null

mv $AAR_SRC_NAME/$OUTPUT_NAME $OUTPUT_DIR/

# Remove source directory that was created.
rm -rf $AAR_SRC_NAME
