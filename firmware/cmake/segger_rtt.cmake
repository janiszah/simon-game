include_guard(GLOBAL)

set(segger_rtt_path "${CMAKE_SOURCE_DIR}/3rd_party/segger_rtt")

# Segger RTT
set(segger_rtt_src
  ${segger_rtt_path}/RTT/SEGGER_RTT_ASM_ARMv7M.S
  ${segger_rtt_path}/RTT/SEGGER_RTT.c
  ${segger_rtt_path}/RTT/SEGGER_RTT_printf.c
  ${segger_rtt_path}/Syscalls/SEGGER_RTT_Syscalls_GCC.c

  CACHE INTERNAL ""
)

set(segger_rtt_inc
  ${segger_rtt_path}/RTT
  ${segger_rtt_path}/Config

  CACHE INTERNAL ""
)


function(use_SeggerRTT)

  message("Generating Segger RTT Library...")

  set(rtt_lib "segger_rtt")
  add_library(${rtt_lib} STATIC ${segger_rtt_src})
  target_include_directories(${rtt_lib} PUBLIC ${segger_rtt_inc})
  # add_platform_library(${rtt_lib})

endfunction()