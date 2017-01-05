################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
FreeRTOS/portable/MemMang/heap_4.obj: ../FreeRTOS/portable/MemMang/heap_4.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/home/willian/ti/ccsv6/tools/compiler/ti-cgt-arm_15.12.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/home/willian/Tivaware" --include_path="/home/willian/Documents/CodeComposerProg/EmbarcadosTWFR/Tasks" --include_path="/home/willian/Documents/CodeComposerProg/EmbarcadosTWFR/Drivers" --include_path="/home/willian/Tivaware/examples/boards/ek-tm4c1294xl" --include_path="/home/willian/ti/ccsv6/tools/compiler/ti-cgt-arm_15.12.1.LTS/include" --include_path="/home/willian/Documents/CodeComposerProg/EmbarcadosTWFR/FreeRTOS" --include_path="/home/willian/Documents/CodeComposerProg/EmbarcadosTWFR/FreeRTOS/include" --include_path="/home/willian/Documents/CodeComposerProg/EmbarcadosTWFR/FreeRTOS/portable/MemMang" --include_path="/home/willian/Documents/CodeComposerProg/EmbarcadosTWFR/FreeRTOS/portable/CCS/ARM_CM4F" -g --gcc --define=ccs="ccs" --define=PART_TM4C1294NCPDT --define=TARGET_IS_TM4C129_RA0 --diag_wrap=off --display_error_number --diag_warning=225 --abi=eabi --preproc_with_compile --preproc_dependency="FreeRTOS/portable/MemMang/heap_4.d" --obj_directory="FreeRTOS/portable/MemMang" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '


