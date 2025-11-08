cmake_minimum_required(VERSION 3.10)
project(fastcgi_cpp_example LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(fastcgi_example main.cpp)

find_library(FCGI_LIB fcgi)
find_library(FCGI_LIBPP fcgi++)

if(NOT FCGI_LIB)
  message(WARNING "No se encontró libfcgi automáticamente. Asegúrate de tener libfcgi-dev instalado y linkea -lfcgi manualmente si hace falta.")
else()
  target_link_libraries(fastcgi_example PRIVATE ${FCGI_LIB} ${FCGI_LIBPP})
endif()

install(TARGETS fastcgi_example DESTINATION bin)