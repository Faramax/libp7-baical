set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std1y")

add_library(p7_logger_lib SHARED
   p7_logger.h
   p7_logger.cpp)
target_link_libraries(p7_logger_lib P7)
target_include_directories(p7_logger_lib PUBLIC ./ ../ ../Headers)
