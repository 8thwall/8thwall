#!/bin/bash

DISTRIBUTION_CERT_ID=$(security find-identity -v -p codesigning | grep "Apple Distribution" | head -1 | awk '{print $2}')
DISTRIBUTION_CERT_NAME=$(security find-identity -v -p codesigning | grep "Apple Distribution" | head -1 | awk -F\" '{print $(NF-1)}')

# Local testing:
# DISTRIBUTION_CERT_ID=$(security find-identity -v -p codesigning | grep "Apple Development" | head -1 | awk '{print $2}')
# DISTRIBUTION_CERT_NAME=$(security find-identity -v -p codesigning | grep "Apple Development" | head -1 | awk -F\" '{print $(NF-1)}')

PRESIGNATURE_BUNDLE=$1
SIGNED_BUNDLE=$2

# Contracts
if [ -z "${PRESIGNATURE_BUNDLE}" ] || [ -z "${SIGNED_BUNDLE}" ]
then
    echo "This script requires 2 arguments please."
    exit 1
fi

# Copy to final destination
mkdir -p $(dirname $SIGNED_BUNDLE)

echo "Copying $PRESIGNATURE_BUNDLE to $SIGNED_BUNDLE for signing..."
cp -fv $PRESIGNATURE_BUNDLE $SIGNED_BUNDLE

# Getting Distribution ID
if [ -n "$DISTRIBUTION_CERT_ID" ]
then
    echo "I will now try to codesign and notarize the bundle ${SIGNED_BUNDLE} with DISTRIBUTION_CERT_ID: ${DISTRIBUTION_CERT_ID} and DISTRIBUTION_CERT_NAME: ${DISTRIBUTION_CERT_NAME}"
    echo "Codesigning with distribution certificate: ${DISTRIBUTION_CERT_ID} - ${DISTRIBUTION_CERT_NAME}..."
    /usr/bin/codesign -vvv --force --verbose --options=runtime --timestamp --deep -s "${DISTRIBUTION_CERT_ID}" "${SIGNED_BUNDLE}"
    echo "... done codesign. Return code was: $?"

else
    echo "No Distribution certificate found on host: the bundle ${SIGNED_BUNDLE} will not be signed for distribution"
fi
