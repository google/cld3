# This cmake scripts only builds a static cld3 lib and the unittests.

project(cld3)

# Old versions of cmake dont search/find protobuf lite
cmake_minimum_required(VERSION 3.9)

find_package(Protobuf REQUIRED)
message(STATUS "Protobuf_FOUND= ${Protobuf_FOUND}")
message(STATUS "Protobuf_VERSION= ${Protobuf_VERSION}")
message(WARNING "Protobuf 2.5 and CLD3 seems happy together. This script does NOT check if your verison of protobuf is compatible.")
message(STATUS "Protobuf_LIBRARIES= ${Protobuf_LIBRARIES}")
message(STATUS "Protobuf_LITE_LIBRARIES= ${Protobuf_LITE_LIBRARIES}") # Usually /usr/lib64/libprotobuf-lite.so

# By default, protobuf_generate_cpp generates pb.* files directy in the cmake build dir. 
# But CLD3 sources have been coded using hard coded pathes to cld_3/protos/*.pb.h. 
# So *.pb.h must be output to cld_3/protos.
# For that, let's use a custom my_protobuf_generate_cpp:
include(${CMAKE_CURRENT_SOURCE_DIR}/misc/myprotobuf.cmake)
my_protobuf_generate_cpp(cld_3/protos PROTO_SRCS PROTO_HDRS src/feature_extractor.proto src/sentence.proto src/task_spec.proto)
message(STATUS "PROTO_HDRS= ${PROTO_HDRS}")

add_definitions(-fPIC) # Position Independant Code
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
add_definitions(-std=c++11) # Needed for std::to_string(), ...

include_directories(${CMAKE_CURRENT_BINARY_DIR}) # needed to include generated pb headers

add_library(${PROJECT_NAME} 
	${PROTO_SRCS} ${PROTO_HDRS}
	src/base.cc
	src/embedding_feature_extractor.cc
	src/embedding_network.cc
	src/feature_extractor.cc
	src/feature_extractor.h
	src/feature_types.cc
	src/fml_parser.cc
	src/language_identifier_features.cc
	src/lang_id_nn_params.cc 
	src/nnet_language_identifier.cc
	src/registry.cc
	src/relevant_script_feature.cc
	src/sentence_features.cc
	src/task_context.cc
	src/task_context_params.cc
	src/unicodetext.cc
	src/utils.cc
	src/workspace.cc
	
	src/script_span/generated_entities.cc
	src/script_span/getonescriptspan.cc
	src/script_span/getonescriptspan.h
	src/script_span/getonescriptspan_test.cc
	src/script_span/utf8statetable.cc
	src/script_span/offsetmap.cc
	src/script_span/text_processing.cc
	src/script_span/text_processing.h
	src/script_span/fixunicodevalue.cc
	)

# unit tests exec:
add_executable(language_identifier_main src/language_identifier_main.cc)
target_link_libraries(language_identifier_main cld3 ${Protobuf_LITE_LIBRARIES})

add_executable(getonescriptspan_test src/script_span/getonescriptspan_test.cc)
target_link_libraries(getonescriptspan_test cld3 ${Protobuf_LITE_LIBRARIES})

add_executable(language_identifier_features_test src/language_identifier_features_test.cc)
target_link_libraries(language_identifier_features_test cld3 ${Protobuf_LITE_LIBRARIES})
