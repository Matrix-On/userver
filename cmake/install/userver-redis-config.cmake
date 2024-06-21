include_guard(GLOBAL)

if(userver_redis_FOUND)
  return()
endif()

find_package(userver REQUIRED COMPONENTS
    core
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/..")
include("${USERVER_CMAKE_DIR}/FindHiredis.cmake")

set(userver_redis_FOUND TRUE)
