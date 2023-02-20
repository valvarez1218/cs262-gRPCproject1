cmake_minimum_required(VERSION 3.5.1)

project(ChatService)

include(./cmake/common.cmake)

# Proto file
get_filename_component(cs_proto "./chatService/chatService.proto" ABSOLUTE)
get_filename_component(cs_proto_path "${cs_proto}" PATH)

# Generated sources
set(cs_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/../chatService/chatService.pb.cc")
set(rcs_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/../chatService/chatService.pb.h")
set(cs_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/../chatService/chatService.grpc.pb.cc")
set(cs_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/../chatService/chatService.grpc.pb.h")
add_custom_command(
      OUTPUT "${cs_proto_srcs}" "${cs_proto_hdrs}" "${cs_grpc_srcs}" "${cs_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${cs_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${cs_proto}"
      DEPENDS "${cs_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# hw_grpc_proto
add_library(cs_grpc_proto
  ${cs_grpc_srcs}
  ${cs_grpc_hdrs}
  ${cs_proto_srcs}
  ${cs_proto_hdrs})
target_link_libraries(cs_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})


add_executable(chatClient ./chatService/client/chatClient.cc)
target_link_libraries(chatClient
    cs_grpc_proto
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF})

add_executable(chatServer ./chatService/server/chatServer.cc)
target_link_libraries(chatServer
    cs_grpc_proto
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF})