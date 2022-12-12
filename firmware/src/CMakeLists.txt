set(target "simon-game")

# Use STM32Cube library
include(stm32hal)
use_STM32hal(G0 .)

set(src
  main.c
  board/bsp.c

  stm32g0xx_it.c
  stm32g0xx_hal_msp.c
  
  board/startup_stm32g030xx.s
  board/system_stm32g0xx.c
  )

set(inc
  board
  )

set(linker_file ${CMAKE_CURRENT_SOURCE_DIR}/board/STM32G030F6Px_FLASH.ld)

add_executable(${target} ${src})
target_include_directories(${target} PRIVATE ${inc})

target_link_libraries(${target} PRIVATE
  -Wl,--start-group
  m
  c
  gcc
  nosys
  stm32hal_g0
  -Wl,--end-group
  )

target_link_options(${target} PRIVATE
  -T${linker_file}
  )


  set_target_properties(${target} PROPERTIES OUTPUT_NAME ${target}.elf)

# Create hex file
add_custom_command(TARGET ${target}
    POST_BUILD
    COMMAND ${TOOLCHAIN_PREFIX}-objcopy -O ihex ${target}.elf ${target}.hex
    COMMAND ${TOOLCHAIN_PREFIX}-objcopy -O binary ${target}.elf ${target}.bin
    )


# Load Firmware
add_custom_target(load
  COMMAND openocd -f interface/stlink.cfg -f target/stm32g0x.cfg -c 'program ${CMAKE_CURRENT_BINARY_DIR}/${target} verify reset exit'
  COMMENT "Loading firmware..."
  USES_TERMINAL
)

# Show largest Flash/RAM entities
add_custom_target(size
  COMMAND arm-none-eabi-nm --print-size --size-sort --radix=d ${CMAKE_CURRENT_BINARY_DIR}/${target} | tail -30
  COMMENT "Flash/RAM entity sizes:"
)