cmake_minimum_required(VERSION 3.5)
project(GTKApp)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)



include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

add_executable(gtk_app main.c functii.c)

target_link_libraries(gtk_app ${GTK3_LIBRARIES})