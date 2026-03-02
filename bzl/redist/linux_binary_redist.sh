#!/bin/bash

# Check if HERMETIC_LDD environment variable is set
if [ -z "$HERMETIC_LDD" ]; then
  echo "HERMETIC_LDD environment variable is not set"
  exit 1
fi

# Check if BINARY_EXECUTABLE environment variable is set
if [ -z "$BINARY_EXECUTABLE" ]; then
  echo "BINARY_EXECUTABLE environment variable is not set"
  exit 1
fi

# Check if REDIST_BINARY_EXECUTABLE environment variable is set
if [ -z "$REDIST_BINARY_EXECUTABLE" ]; then
  echo "REDIST_BINARY_EXECUTABLE environment variable is not set"
  exit 1
fi

# List of libraries
# LIBRARIES=("LIBSTDCPP" "LIBGCC_S" "LIBM" "LIBDL" "LIBPTHREAD" "LIBC") 
LIBRARIES=("LIBSTDCPP" "LIBGCC_S")

# Iterate over each library
for LIBRARY in ${LIBRARIES[@]}; do
    # Check if OUTPUT_ environment variable is set
    OUTPUT_VAR="OUTPUT_$LIBRARY"
    if [ -z "${!OUTPUT_VAR}" ]; then
        echo "$OUTPUT_VAR environment variable is not set"
        exit 1
    fi

    # Check if HERMETIC_ environment variable is set
    HERMETIC_VAR="HERMETIC_$LIBRARY"
    if [ -z "${!HERMETIC_VAR}" ]; then
        echo "$HERMETIC_VAR environment variable is not set"
        exit 1
    fi
    
    for temp_hermetic_var in ${!HERMETIC_VAR}; do
        if [ "$(basename "${temp_hermetic_var}")" = "$(basename "${!OUTPUT_VAR}")" ]; then
            echo "Copying and dereferencing ${temp_hermetic_var} -> ${!OUTPUT_VAR}"
            cp --dereference --verbose "$temp_hermetic_var" "${!OUTPUT_VAR}"
            break;
        fi
    done
done

unset LD_LIBRARY_PATH

cp -fv $BINARY_EXECUTABLE $REDIST_BINARY_EXECUTABLE
