################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
/home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Utilities/Logger.cpp \
/home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Utilities/Maths.cpp 

OBJS += \
./src/SGEngine2/Utilities/Logger.o \
./src/SGEngine2/Utilities/Maths.o 

CPP_DEPS += \
./src/SGEngine2/Utilities/Logger.d \
./src/SGEngine2/Utilities/Maths.d 


# Each subdirectory must supply rules for building sources it contributes
src/SGEngine2/Utilities/Logger.o: /home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Utilities/Logger.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -DUBUNTU -I/home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Utilities -I/home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/libpng -I/home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Font -I/home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Core/Meshes -I/home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Font/glu/include -I/home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Font/ftgl -I/home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Font/freeType/include -I/home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Font/freeType/include/freetype -I/home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2 -I/home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/Iyan3dEngineFiles -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/SGEngine2/Utilities/Maths.o: /home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Utilities/Maths.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -DUBUNTU -I/home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Utilities -I/home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/libpng -I/home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Font -I/home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Core/Meshes -I/home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Font/glu/include -I/home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Font/ftgl -I/home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Font/freeType/include -I/home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Font/freeType/include/freetype -I/home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2 -I/home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/Iyan3dEngineFiles -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


