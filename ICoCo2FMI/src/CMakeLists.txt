cmake_minimum_required (VERSION 3.2)


project (FMU)

if (WIN32)
   set(FMI_PLATFORM win)
elseif (APPLE)
   set(FMI_PLATFORM darwin)
else ()
   set(FMI_PLATFORM linux)
endif ()

if ("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
    set (FMI_PLATFORM ${FMI_PLATFORM}64)
else ()
    set (FMI_PLATFORM ${FMI_PLATFORM}32)
endif ()

MESSAGE("FMI_PLATFORM: " ${FMI_PLATFORM})

set(FMI_VERSION  20)
set(FMI_TYPE cs )

set(TARGET_NAME ${MODEL_NAME}_${FMI_VERSION}_${FMI_TYPE})

file (GLOB srcs  ICoCoFMI.cpp *.h ) 
if (FORCE_OLD_ABI)
  message("Forcing not use GLIBCXX_USE_CXX11_ABI")
  add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
endif()
add_library(${TARGET_NAME} SHARED ${srcs}  modelDescription_${FMI_TYPE}.xml)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/fmu${FMI_VERSION}/${FMI_TYPE})

target_compile_definitions(${TARGET_NAME} PRIVATE DISABLE_PREFIX)

if (${FMI_TYPE} MATCHES "cs")
  target_compile_definitions(${TARGET_NAME} PRIVATE FMI_COSIMULATION)
endif()


#set(ICOCODIR /users/fauchet/C3/c3dev/c3dev_cpp/install/src/ICoCo)
target_include_directories(${TARGET_NAME} PRIVATE "FMI/shared/include")
target_include_directories(${TARGET_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_include_directories(${TARGET_NAME} PRIVATE "${ICOCODIR}")
target_link_libraries (${TARGET_NAME} PRIVATE  "${ICOCOLIB}" )

set(FMU_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/fmu${FMI_VERSION}/${FMI_TYPE}/${MODEL_NAME})

set_target_properties(${TARGET_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY         "${FMU_BUILD_DIR}/binaries/${FMI_PLATFORM}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${FMU_BUILD_DIR}/binaries/${FMI_PLATFORM}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${FMU_BUILD_DIR}/binaries/${FMI_PLATFORM}"
    LIBRARY_OUTPUT_DIRECTORY         "${FMU_BUILD_DIR}/binaries/${FMI_PLATFORM}"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG   "${FMU_BUILD_DIR}/binaries/${FMI_PLATFORM}"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${FMU_BUILD_DIR}/binaries/${FMI_PLATFORM}"
#    ARCHIVE_OUTPUT_DIRECTORY         "${FMU_BUILD_DIR}/binaries/${FMI_PLATFORM}"
#    ARCHIVE_OUTPUT_DIRECTORY_DEBUG   "${FMU_BUILD_DIR}/binaries/${FMI_PLATFORM}"
#    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${FMU_BUILD_DIR}/binaries/${FMI_PLATFORM}"
)

set_target_properties(${TARGET_NAME} PROPERTIES PREFIX "")
set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME ${MODEL_NAME})

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy
  modelDescription_${FMI_TYPE}.xml
  "${FMU_BUILD_DIR}/modelDescription.xml"
)

 # COMMAND ${CMAKE_COMMAND} -E tar "cfv" "fmu${FMI_VERSION}/${FMI_TYPE}/${MODEL_NAME}.fmu" --format=zip
if(1)
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E tar "cfv" "${CMAKE_CURRENT_BINARY_DIR}/${MODEL_NAME}.fmu" --format=zip
  "modelDescription.xml"
  "binaries"
#  "sources"
# "resources"
  WORKING_DIRECTORY ${FMU_BUILD_DIR}
)

endif()
