

set(linker_flags " \
-mcpu=cortex-m0plus \
-g \
-Wall \
--specs=nano.specs \
--specs=nosys.specs \
-mthumb \
-Wl,--gc-sections \
-Wl,-Map=output.map \
-Wl,--cref \
-Wl,--print-memory-usage \
")

set(compiler_flags " \
-DUSE_HAL_DRIVER \
-DSTM32G030xx \
-mcpu=cortex-m0plus \
-g \
-O0 \
-ffunction-sections \
-mthumb \
-Wall \
-std=gnu99 \
")


set(CMAKE_C_FLAGS ${compiler_flags})
set(CMAKE_EXE_LINKER_FLAGS ${linker_flags})