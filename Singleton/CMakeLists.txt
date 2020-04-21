set(TARGET p7_logger_lib)
set(SOURCE
   p7_logger.h
   p7_logger.cpp
   )

if (P7_LOGGER_SHARED_LIB)
   add_library(${TARGET} SHARED ${SOURCE})
else()
   add_library(${TARGET} STATIC ${SOURCE})
endif()

target_link_libraries(${TARGET} p7)
target_include_directories(${TARGET} PUBLIC ./ ../ ../Headers)
