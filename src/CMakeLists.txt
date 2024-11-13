add_executable(moncfg main.cpp)

target_include_directories(moncfg PRIVATE 
  "${CMAKE_BINARY_DIR}/configured_files/include"
)

target_link_libraries(
  moncfg
  PRIVATE moncfg::moncfg_options
          moncfg::moncfg_warnings
  )

target_link_system_libraries(
  moncfg
  PRIVATE
          CLI11::CLI11
          fmt::fmt
          spdlog::spdlog)

target_include_directories(moncfg PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")
if (WIN32)
  add_custom_command(
    TARGET moncfg POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:moncfg> $<TARGET_FILE_DIR:moncfg>
    COMMAND_EXPAND_LISTS
  )
endif()