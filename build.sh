
#!/bin/sh

# Set all dependencies
C_COMPILER_PATH=/usr/bin/gcc
CXX_COMPILER_PATH=/usr/bin/g++
SAMPLE="$1"
BUILD_DIR="$SAMPLE/build"
LOG_DIR="log"
CURRENT_TIME=`date "+%Y-%m-%d_%H-%M-%S"`
LOG_FILE="$LOG_DIR/build_$CURRENT_TIME.log"

# Create build directory
if [ ! -d "$BUILD_DIR" ]; then
  mkdir "$BUILD_DIR"
fi

# [CLEAR] Delete ./build folder containings
if [[ ("$2" == "--clean") || ("$2" == "-c") ]]; then
    rm -rf "$BUILD_DIR"
    echo "==> Clear build folder ($BUILD_DIR)"
    exit
fi

# Create log directory
if [ ! -d "$BUILD_DIR/$LOG_DIR" ]; then
  mkdir "$BUILD_DIR/$LOG_DIR"
fi

cd $BUILD_DIR

  # Create log file
  touch $LOG_FILE

  # Generate makefile
  echo "==> Run cmake for '$SAMPLE'" | tee -a $LOG_FILE
  CC=$C_COMPILER_PATH CXX=$CXX_COMPILER_PATH cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON .. | tee -a $LOG_FILE

  # Run static analysis
  if [ ! -d "$BUILD_DIR" ]; then
    echo "==> Run cppcheck for '$SAMPLE'" | tee -a $LOG_FILE
    cppcheck --template=gcc --project=compile_commands.json | tee -a $LOG_FILE
  fi

  # Run make to build the project
  echo "==> Run make for '$SAMPLE'" | tee -a $LOG_FILE
  make | tee -a $LOG_FILE

cd ../..
