cmake_minimum_required(VERSION 3.20)

# import zephyr library
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

# define cmake project
project(duck)

file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS
    "src/*.c"
    "src/*.cpp"
)

# list(FILTER APP_SOURCES EXCLUDE REGEX "src/key.cpp")

target_sources(app PRIVATE
    ${APP_SOURCES})

# 将上层模块目录添加到Zephyr的额外模块搜索路径中**
list(APPEND ZEPHYR_EXTRA_MODULES
    ${CMAKE_CURRENT_SOURCE_DIR}/..  
)


# 添加设备树绑定文件的搜索路径
list(APPEND DTS_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../boards) 
