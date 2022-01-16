################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Font/freeType/include/src/smooth/ftgrays.c \
/home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Font/freeType/include/src/smooth/ftsmooth.c \
/home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Font/freeType/include/src/smooth/ftspic.c \
/home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Font/freeType/include/src/smooth/smooth.c 

OBJS += \
./src/SGEngine2/Font/freeType/include/src/smooth/ftgrays.o \
./src/SGEngine2/Font/freeType/include/src/smooth/ftsmooth.o \
./src/SGEngine2/Font/freeType/include/src/smooth/ftspic.o \
./src/SGEngine2/Font/freeType/include/src/smooth/smooth.o 

C_DEPS += \
./src/SGEngine2/Font/freeType/include/src/smooth/ftgrays.d \
./src/SGEngine2/Font/freeType/include/src/smooth/ftsmooth.d \
./src/SGEngine2/Font/freeType/include/src/smooth/ftspic.d \
./src/SGEngine2/Font/freeType/include/src/smooth/smooth.d 


# Each subdirectory must supply rules for building sources it contributes
src/SGEngine2/Font/freeType/include/src/smooth/ftgrays.o: /home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Font/freeType/include/src/smooth/ftgrays.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DUBUNTU -DRTCORE_ENABLE_RAY_MASK -I/home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Utilities -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/SGEngine2/Font/freeType/include/src/smooth/ftsmooth.o: /home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Font/freeType/include/src/smooth/ftsmooth.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DUBUNTU -DRTCORE_ENABLE_RAY_MASK -I/home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Utilities -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/SGEngine2/Font/freeType/include/src/smooth/ftspic.o: /home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Font/freeType/include/src/smooth/ftspic.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DUBUNTU -DRTCORE_ENABLE_RAY_MASK -I/home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Utilities -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/SGEngine2/Font/freeType/include/src/smooth/smooth.o: /home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Font/freeType/include/src/smooth/smooth.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DUBUNTU -DRTCORE_ENABLE_RAY_MASK -I/home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Utilities -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


