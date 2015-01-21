################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
F2837xD_CodeStartBranch.obj: /opt/ti/F28377D\ Support/F2837xD_common/source/F2837xD_CodeStartBranch.asm $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"/opt/ti/ccsv6/tools/compiler/c2000_6.2.7/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="/opt/ti/ccsv6/tools/compiler/c2000_6.2.7/include" --include_path="/opt/ti/F28377D Support/F2837xD_headers/include" --include_path="/opt/ti/F28377D Support/F2837xD_common/include" --include_path="/opt/ti/F28377D Support/F2837xD_common/" -g --define=CPU1 --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="F2837xD_CodeStartBranch.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

F2837xD_DefaultISR.obj: /opt/ti/F28377D\ Support/F2837xD_common/source/F2837xD_DefaultISR.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"/opt/ti/ccsv6/tools/compiler/c2000_6.2.7/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="/opt/ti/ccsv6/tools/compiler/c2000_6.2.7/include" --include_path="/opt/ti/F28377D Support/F2837xD_headers/include" --include_path="/opt/ti/F28377D Support/F2837xD_common/include" --include_path="/opt/ti/F28377D Support/F2837xD_common/" -g --define=CPU1 --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="F2837xD_DefaultISR.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

F2837xD_GlobalVariableDefs.obj: /opt/ti/F28377D\ Support/F2837xD_headers/source/F2837xD_GlobalVariableDefs.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"/opt/ti/ccsv6/tools/compiler/c2000_6.2.7/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="/opt/ti/ccsv6/tools/compiler/c2000_6.2.7/include" --include_path="/opt/ti/F28377D Support/F2837xD_headers/include" --include_path="/opt/ti/F28377D Support/F2837xD_common/include" --include_path="/opt/ti/F28377D Support/F2837xD_common/" -g --define=CPU1 --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="F2837xD_GlobalVariableDefs.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

F2837xD_Gpio.obj: /opt/ti/F28377D\ Support/F2837xD_common/source/F2837xD_Gpio.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"/opt/ti/ccsv6/tools/compiler/c2000_6.2.7/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="/opt/ti/ccsv6/tools/compiler/c2000_6.2.7/include" --include_path="/opt/ti/F28377D Support/F2837xD_headers/include" --include_path="/opt/ti/F28377D Support/F2837xD_common/include" --include_path="/opt/ti/F28377D Support/F2837xD_common/" -g --define=CPU1 --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="F2837xD_Gpio.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

F2837xD_PieCtrl.obj: /opt/ti/F28377D\ Support/F2837xD_common/source/F2837xD_PieCtrl.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"/opt/ti/ccsv6/tools/compiler/c2000_6.2.7/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="/opt/ti/ccsv6/tools/compiler/c2000_6.2.7/include" --include_path="/opt/ti/F28377D Support/F2837xD_headers/include" --include_path="/opt/ti/F28377D Support/F2837xD_common/include" --include_path="/opt/ti/F28377D Support/F2837xD_common/" -g --define=CPU1 --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="F2837xD_PieCtrl.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

F2837xD_PieVect.obj: /opt/ti/F28377D\ Support/F2837xD_common/source/F2837xD_PieVect.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"/opt/ti/ccsv6/tools/compiler/c2000_6.2.7/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="/opt/ti/ccsv6/tools/compiler/c2000_6.2.7/include" --include_path="/opt/ti/F28377D Support/F2837xD_headers/include" --include_path="/opt/ti/F28377D Support/F2837xD_common/include" --include_path="/opt/ti/F28377D Support/F2837xD_common/" -g --define=CPU1 --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="F2837xD_PieVect.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

F2837xD_SysCtrl.obj: /opt/ti/F28377D\ Support/F2837xD_common/source/F2837xD_SysCtrl.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"/opt/ti/ccsv6/tools/compiler/c2000_6.2.7/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="/opt/ti/ccsv6/tools/compiler/c2000_6.2.7/include" --include_path="/opt/ti/F28377D Support/F2837xD_headers/include" --include_path="/opt/ti/F28377D Support/F2837xD_common/include" --include_path="/opt/ti/F28377D Support/F2837xD_common/" -g --define=CPU1 --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="F2837xD_SysCtrl.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

F2837xD_usDelay.obj: /opt/ti/F28377D\ Support/F2837xD_common/source/F2837xD_usDelay.asm $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"/opt/ti/ccsv6/tools/compiler/c2000_6.2.7/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="/opt/ti/ccsv6/tools/compiler/c2000_6.2.7/include" --include_path="/opt/ti/F28377D Support/F2837xD_headers/include" --include_path="/opt/ti/F28377D Support/F2837xD_common/include" --include_path="/opt/ti/F28377D Support/F2837xD_common/" -g --define=CPU1 --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="F2837xD_usDelay.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

interrupt.obj: /opt/ti/F28377D\ Support/F2837xD_common/driverlib/interrupt.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"/opt/ti/ccsv6/tools/compiler/c2000_6.2.7/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="/opt/ti/ccsv6/tools/compiler/c2000_6.2.7/include" --include_path="/opt/ti/F28377D Support/F2837xD_headers/include" --include_path="/opt/ti/F28377D Support/F2837xD_common/include" --include_path="/opt/ti/F28377D Support/F2837xD_common/" -g --define=CPU1 --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="interrupt.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

main.obj: ../main.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"/opt/ti/ccsv6/tools/compiler/c2000_6.2.7/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="/opt/ti/ccsv6/tools/compiler/c2000_6.2.7/include" --include_path="/opt/ti/F28377D Support/F2837xD_headers/include" --include_path="/opt/ti/F28377D Support/F2837xD_common/include" --include_path="/opt/ti/F28377D Support/F2837xD_common/" -g --define=CPU1 --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="main.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

sysctl.obj: /opt/ti/F28377D\ Support/F2837xD_common/driverlib/sysctl.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"/opt/ti/ccsv6/tools/compiler/c2000_6.2.7/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="/opt/ti/ccsv6/tools/compiler/c2000_6.2.7/include" --include_path="/opt/ti/F28377D Support/F2837xD_headers/include" --include_path="/opt/ti/F28377D Support/F2837xD_common/include" --include_path="/opt/ti/F28377D Support/F2837xD_common/" -g --define=CPU1 --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="sysctl.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

uart.obj: /opt/ti/F28377D\ Support/F2837xD_common/driverlib/uart.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"/opt/ti/ccsv6/tools/compiler/c2000_6.2.7/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="/opt/ti/ccsv6/tools/compiler/c2000_6.2.7/include" --include_path="/opt/ti/F28377D Support/F2837xD_headers/include" --include_path="/opt/ti/F28377D Support/F2837xD_common/include" --include_path="/opt/ti/F28377D Support/F2837xD_common/" -g --define=CPU1 --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="uart.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

uartstdio.obj: /opt/ti/F28377D\ Support/F2837xD_common/utils/uartstdio.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"/opt/ti/ccsv6/tools/compiler/c2000_6.2.7/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="/opt/ti/ccsv6/tools/compiler/c2000_6.2.7/include" --include_path="/opt/ti/F28377D Support/F2837xD_headers/include" --include_path="/opt/ti/F28377D Support/F2837xD_common/include" --include_path="/opt/ti/F28377D Support/F2837xD_common/" -g --define=CPU1 --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="uartstdio.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '


