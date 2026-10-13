#!/bin/bash

# --- Configuration and Setup ---
# Check for required tools
if ! command -v curl &> /dev/null; then
    echo "Error: 'curl' is required but not installed."
    exit 1
fi

if ! command -v jq &> /dev/null; then
    echo "Error: 'jq' is required but not installed. Please install it (e.g., sudo apt-get install jq)."
    exit 1
fi

# Function to display usage instructions
usage() {
    echo "Usage: $0 <github_owner> <repository_name>"
    echo "Example: $0 facebook react"
    echo
    echo "Fetches the download URLs for all assets included in the latest GitHub release."
}

# --- Input Validation ---
if [ "$#" -ne 2 ]; then
    usage
    exit 1
fi

OWNER=$1
REPO=$2

echo "=========================================================================="
echo "🚀 Fetching assets for repository: $OWNER/$REPO"
echo "=========================================================================="

# GitHub API endpoint for releases
API_URL="https://api.github.com/repos/${OWNER}/${REPO}/releases"

# Fetch the JSON data, using the token for authentication
API_RESPONSE=$(curl -s -H \
    -H "Accept: application/vnd.github&version=v3+json" \
    "$API_URL")

# 2. Extract the assets array from the *first* release (the latest one)
# We use jq to:
#   .[] : iterate over the array of releases (though we only care about the first)
#   .[0] : select the first release object
#   .assets[] : iterate over the assets array within that release
#   .browser_download_url : extract the actual download URL for each asset
ASSETS=$(echo "$API_RESPONSE" | jq -r '.[0].assets[].browser_download_url')

# 3. Check if any assets were found
if [ -z "$ASSETS" ]; then
    echo "✅ Success! Latest release found, but no downloadable assets were attached."
    echo "       (The assets array was empty for this release.)"
    exit 0
fi

# 4. Output the results
echo -e "\n✨ Successfully found artifacts from the latest release!"
echo "---------------------------------------------------------------------"

# Print each URL found, handling potential newline characters in the output
echo "$ASSETS" | while IFS= read -r URL; do
    echo "$URL"
done

echo "---------------------------------------------------------------------"
echo "Done."
