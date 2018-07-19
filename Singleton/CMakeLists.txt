set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED 1)

add_library(p7_logger_lib SHARED
   p7_logger.h
   p7_logger.cpp)
target_link_libraries(p7_logger_lib P7)
target_include_directories(p7_logger_lib PUBLIC ./ ../ ../Headers)
