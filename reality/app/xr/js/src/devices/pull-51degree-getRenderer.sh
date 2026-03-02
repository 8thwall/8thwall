THIS_DIR=$(dirname "$0")
OUTPUT_FILE="${THIS_DIR}/getRenderer.js"
echo "/* eslint-disable */\n" > $OUTPUT_FILE

curl -s -m 10 --retry 0 "https://raw.githubusercontent.com/51Degrees/Renderer/master/renderer.js" >> $OUTPUT_FILE

EXTRA_CODE="
const maxRetries = 1;
export {
  getRenderer,
}
"

# You need quotes around $EXTRA_CODE to keep the \n
echo "$EXTRA_CODE" >> $OUTPUT_FILE
