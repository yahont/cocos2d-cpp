################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../label_nodes/CCLabelAtlas.cpp \
../label_nodes/CCLabelBMFont.cpp \
../label_nodes/CCLabelTTF.cpp 

OBJS += \
./label_nodes/CCLabelAtlas.o \
./label_nodes/CCLabelBMFont.o \
./label_nodes/CCLabelTTF.o 

CPP_DEPS += \
./label_nodes/CCLabelAtlas.d \
./label_nodes/CCLabelBMFont.d \
./label_nodes/CCLabelTTF.d 


# Each subdirectory must supply rules for building sources it contributes
label_nodes/%.o: ../label_nodes/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DLINUX -I"/home/laschweinski/git/cocos2d-x/cocos2dx" -I"/home/laschweinski/git/cocos2d-x/cocos2dx/platform/third_party/linux/libfreetype2" -I"/home/laschweinski/git/cocos2d-x/cocos2dx/include" -I"/home/laschweinski/git/cocos2d-x/cocos2dx/platform" -I"/home/laschweinski/git/cocos2d-x/cocos2dx/platform/third_party/linux/libpng" -I"/home/laschweinski/git/cocos2d-x/cocos2dx/platform/third_party/linux/libxml2" -I"/home/laschweinski/git/cocos2d-x/cocos2dx/platform/third_party/linux/libjpeg" -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


