include_guard(GLOBAL)

set(lib_path "${CMAKE_CURRENT_LIST_DIR}" CACHE INTERNAL "")

function(use_STM32hal FAMILY hal_conf_path)

  message("Generating STM32 HAL library...")

  # Lower case familiy name
  string(TOLOWER ${FAMILY} FAMILY_L)  

  # HAL Drivers for familiy
  set(g0_hal_drivers
    adc cortex dma exti flash 
    gpio i2c dac 
    pwr rcc spi rtc rng
    tim iwdg

    adc_ex dma_ex dac_ex flash_ex i2c_ex 
    pwr_ex rcc_ex rtc_ex tim_ex 

  )

  # LL Drivers for familiy
  set(g0_ll_drivers
    adc dac dma exti gpio i2c pwr rcc rng rtc spi 
    tim usart utils 
  )

  # Get Paths
  set(hal_path "${lib_path}/stm32${FAMILY_L}xx_hal_driver")
  set(cmsis_path "${lib_path}/cmsis_core")
  set(cmsis_device_path "${lib_path}/cmsis_device_${FAMILY_L}")

  # --- Collect sources ---
  set(stm32cube_src "${hal_path}/Src/stm32${FAMILY_L}xx_hal.c")

  if(NOT DEFINED ${FAMILY_L}_hal_drivers OR NOT DEFINED ${FAMILY_L}_ll_drivers)
    message(FATAL_ERROR "Drivers for ${FAMILY} not defined")
  endif()

  # Collect HAL Drivers
  foreach(driver ${${FAMILY_L}_hal_drivers})
    list(APPEND stm32cube_src "${hal_path}/Src/stm32${FAMILY_L}xx_hal_${driver}.c")
  endforeach()

  # Collect LL Drivers
  foreach(driver ${${FAMILY_L}_ll_drivers})
    list(APPEND stm32cube_src "${hal_path}/Src/stm32${FAMILY_L}xx_ll_${driver}.c")
  endforeach()


  # --- Collect include paths ---
  set(stm32cube_inc
    # HAL
    ${hal_path}/Inc
    ${hal_path}/Inc/Legacy

    # CMSIS
    ${cmsis_device_path}/Include
    ${cmsis_path}/Core/Include
    ${cmsis_path}/DSP/Include

    # HAL Configuration file path
    ${hal_conf_path}
    )

  # --- Create Library ---
  set(hal_lib "stm32hal_${FAMILY_L}")
  add_library(${hal_lib} STATIC ${stm32cube_src})
  # target_link_libraries(${hal_lib} PRIVATE ${cmsis_path}/DSP/Lib/GCC/libarm_cortexM4lf_math.a)
  target_include_directories(${hal_lib} PUBLIC ${stm32cube_inc})
  # add_platform_library(${hal_lib})

  endfunction()
