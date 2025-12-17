cmake_minimum_required(VERSION 3.10)

project(bti_api VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include/http_server include/rabbitmq)

file(GLOB SOURCES "src/http_server/*.cpp" "src/rabbitmq/*.cpp")

add_executable(fastcgi src/main.cpp ${SOURCES})

find_library(FCGI_LIB fcgi)
find_library(FCGIPP_LIB fcgi++)
find_library(SAMQPC_LIB SimpleAmqpClient)
find_library(JSONCPP_LIB jsoncpp)

if(NOT FCGI_LIB)
  message(WARNING "No se encontró libfcgi automáticamente. Asegúrate de tener libfcgi instalado y linkea -lfcgi manualmente si hace falta.")
elseif(NOT FCGIPP_LIB)
  message(WARNING "No se encontró libfcgi-dev automáticamente. Asegúrate de tener libfcgi-dev instalado y linkea -lfcgi++ manualmente si hace falta.")
elseif(NOT SAMQPC_LIB)
  message(WARNING "No se encontró SimpleAmqpClient automáticamente. Asegúrate de tener SimpleAmqpClient instalado y linkea -lSimpleAmqpClient manualmente si hace falta.")
elseif(NOT JSONCPP_LIB)
  message(WARNING "No se encontró jsoncpp automáticamente. Asegúrate de tener jsoncpp instalado y linkea -ljsoncpp manualmente si hace falta.")
else()
  target_link_libraries(fastcgi PRIVATE ${FCGI_LIB} ${FCGIPP_LIB} ${SAMQPC_LIB} ${JSONCPP_LIB})
endif()
