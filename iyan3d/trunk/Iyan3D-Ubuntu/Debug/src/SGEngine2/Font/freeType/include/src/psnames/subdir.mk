################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Font/freeType/include/src/psnames/psmodule.c \
/home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Font/freeType/include/src/psnames/psnames.c \
/home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Font/freeType/include/src/psnames/pspic.c 

OBJS += \
./src/SGEngine2/Font/freeType/include/src/psnames/psmodule.o \
./src/SGEngine2/Font/freeType/include/src/psnames/psnames.o \
./src/SGEngine2/Font/freeType/include/src/psnames/pspic.o 

C_DEPS += \
./src/SGEngine2/Font/freeType/include/src/psnames/psmodule.d \
./src/SGEngine2/Font/freeType/include/src/psnames/psnames.d \
./src/SGEngine2/Font/freeType/include/src/psnames/pspic.d 


# Each subdirectory must supply rules for building sources it contributes
src/SGEngine2/Font/freeType/include/src/psnames/psmodule.o: /home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Font/freeType/include/src/psnames/psmodule.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DUBUNTU -I/home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Utilities -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/SGEngine2/Font/freeType/include/src/psnames/psnames.o: /home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Font/freeType/include/src/psnames/psnames.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DUBUNTU -I/home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Utilities -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/SGEngine2/Font/freeType/include/src/psnames/pspic.o: /home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Font/freeType/include/src/psnames/pspic.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DUBUNTU -I/home/harishankar/Desktop/SGRenderer/iyan3d5.0/iyan3d/trunk/Iyan3D-Android/app/src/main/jni/SGEngine2/Utilities -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


