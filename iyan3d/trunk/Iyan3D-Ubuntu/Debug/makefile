################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/SGRenderer/subdir.mk
-include src/SGEngine2/libpng/subdir.mk
-include src/SGEngine2/Utilities/subdir.mk
-include src/SGEngine2/SceneManager/subdir.mk
-include src/SGEngine2/RenderManager/subdir.mk
-include src/SGEngine2/Loaders/subdir.mk
-include src/SGEngine2/Font/glu/libutil/subdir.mk
-include src/SGEngine2/Font/glu/libtess/subdir.mk
-include src/SGEngine2/Font/ftgl/subdir.mk
-include src/SGEngine2/Font/freeType/include/src/truetype/subdir.mk
-include src/SGEngine2/Font/freeType/include/src/smooth/subdir.mk
-include src/SGEngine2/Font/freeType/include/src/sfnt/subdir.mk
-include src/SGEngine2/Font/freeType/include/src/raster/subdir.mk
-include src/SGEngine2/Font/freeType/include/src/psnames/subdir.mk
-include src/SGEngine2/Font/freeType/include/src/pshinter/subdir.mk
-include src/SGEngine2/Font/freeType/include/src/cff/subdir.mk
-include src/SGEngine2/Font/freeType/include/src/base/subdir.mk
-include src/SGEngine2/Font/freeType/include/src/autofit/subdir.mk
-include src/SGEngine2/Font/subdir.mk
-include src/SGEngine2/Core/common/subdir.mk
-include src/SGEngine2/Core/Nodes/subdir.mk
-include src/SGEngine2/Core/Meshes/subdir.mk
-include src/SGEngine2/Core/Material/subdir.mk
-include src/Iyan3dEngineFiles/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: Iyan3D-Ubuntu

# Tool invocations
Iyan3D-Ubuntu: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++  -o "Iyan3D-Ubuntu" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C++_DEPS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) Iyan3D-Ubuntu
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
