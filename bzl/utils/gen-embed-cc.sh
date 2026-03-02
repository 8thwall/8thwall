#!/bin/bash
set -e

for path in $@; do

SYMBOL_PREFIX=${SYMBOL_PREFIX:-}
ROSECTION=${ROSECTION:-.rodata}
USE_ASSEMBLY=${USE_ASSEMBLY:-1}

base=$(basename $path)
symbolBase="embedded"$(echo $base | sed -E -e 's/[^A-Za-z0-9_]+/_/g' | tr '[:upper:]' '[:lower:]' | perl -pe 's/(^|_)(\w)/\U$2/g')
symbolData=${symbolBase}Data
symbolSize=${symbolBase}Size
symbolCStr=${symbolBase}CStr
symbolView=${symbolBase}View
fileSize=$(wc -c < $path)

echo "#include <stdio.h>"
echo "#ifdef __cplusplus"
echo "#include <string_view>"
echo "#endif"

if [[ $USE_ASSEMBLY == "1" ]]; then
  echo "__asm(R\"("
  echo ".section ${ROSECTION}"

  echo -e .global $SYMBOL_PREFIX$symbolData
  echo -e .p2align 4
  echo -e $SYMBOL_PREFIX$symbolData:
  echo -e "\t.incbin \"$path\""
  echo -e "\t.byte 0"
  echo -e ")\");"

  echo "#ifdef __cplusplus"
  echo "extern \"C\" {"
  echo "#endif"

  echo "extern const uint8_t $symbolData[];"
else
  echo "#ifdef __cplusplus"
  echo "extern \"C\" {"
  echo "#endif"

  echo "extern const uint8_t $symbolData[] = {"
  hexdump -v -e '/1 "0x%02x, "' "${path}" | fmt -w 100
  echo "0x00"
  echo "};"
fi

echo "extern const char *const $symbolCStr = reinterpret_cast<const char*>($symbolData);"
echo "extern const size_t $symbolSize = $fileSize;"

echo "#ifdef __cplusplus"
echo "}  // extern \"C\""
echo extern const std::string_view $symbolView = { $symbolCStr, $symbolSize }\;
echo "#endif"

done
