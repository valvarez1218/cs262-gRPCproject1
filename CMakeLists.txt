cmake_minimum_required(VERSION 3.2)
project(my_project)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)

# find_package(gRPC CONFIG REQUIRED)
find_package(gRPC CONFIG)


get_filename_component(cs_proto "./chatService/chatService.proto" ABSOLUTE)
get_filename_component(cs_proto_path "${rg_proto}" PATH)

set(cs_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/chatService.pb.cc")
set(rcs_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/chatService.pb.h")
set(cs_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/chatService.grpc.pb.cc")
set(cs_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/chatService.grpc.pb.h")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
include_directories("./chatService/")

# cs_proto
add_library(cs_grpc_proto
  ${cs_grpc_srcs}
  ${cs_grpc_hdrs}
  ${cs_proto_srcs}
  ${cs_proto_hdrs})
target_link_libraries(cs_grpc_proto)

add_executable(chatClient ./chatService/client/chatClient.cc)
target_link_libraries(chatClient grpc++)