################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
/home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Core/Textures/OGLTexture.cpp 

OBJS += \
./src/SGEngine2/Core/Textures/OGLTexture.o 

CPP_DEPS += \
./src/SGEngine2/Core/Textures/OGLTexture.d 


# Each subdirectory must supply rules for building sources it contributes
src/SGEngine2/Core/Textures/OGLTexture.o: /home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Core/Textures/OGLTexture.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


