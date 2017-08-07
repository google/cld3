# Jeroen Ooms 2017 / jeroen@berkeley.edu
# You may override default options below using environment variables

# Your protobuf options
PROTOC ?= protoc
PROTOBUF_INCLUDE ?= -I/usr/local/include
PROTOBUF_LIBS ?= -L/usr/local/lib -lprotobuf

# Compiler options
CXX ?= g++
CXXFLAGS ?= -std=c++11 -pedantic
CPPFLAGS ?= $(PROTOBUF_INCLUDE)

# Linker options
LDLIBS = -L. -lcld3 $(PROTOBUF_LIBS)
CC = $(CXX)

# CLD3 library object files
LIBCLD3 = base.o embedding_feature_extractor.o embedding_network.o feature_extractor.o \
	feature_types.o fml_parser.o language_identifier_features.o lang_id_nn_params.o \
	nnet_language_identifier.o registry.o relevant_script_feature.o sentence_features.o \
	utils.o workspace.o task_context.o task_context_params.o unicodetext.o \
	script_span/fixunicodevalue.o script_span/generated_entities.o \
	script_span/generated_ulscript.o script_span/getonescriptspan.o script_span/offsetmap.o \
	script_span/text_processing.o script_span/utf8statetable.o \
	cld_3/protos/feature_extractor.pb.o cld_3/protos/sentence.pb.o cld_3/protos/task_spec.pb.o

# Note: language_identifier_features_test doesn't compile for me
TESTS = script_detector_test relevant_script_feature_test script_span/getonescriptspan_test \
	language_identifier_main

# Default 'make' command builds and runs tests
all: check

libcld3.a: $(LIBCLD3)
	$(AR) rcs libcld3.a $(LIBCLD3)	

$(LIBCLD3): cld_3/protos/feature_extractor.pb.h cld_3/protos/sentence.pb.h cld_3/protos/task_spec.pb.h

$(TESTS): libcld3.a

protodir:
	mkdir -p cld_3/protos

cld_3/protos/feature_extractor.pb.cc cld_3/protos/feature_extractor.pb.h: protodir
	$(PROTOC) feature_extractor.proto --cpp_out=./cld_3/protos

cld_3/protos/sentence.pb.cc cld_3/protos/sentence.pb.h: protodir
	$(PROTOC) sentence.proto --cpp_out=./cld_3/protos

cld_3/protos/task_spec.pb.cc cld_3/protos/task_spec.pb.h: protodir
	$(PROTOC) task_spec.proto --cpp_out=./cld_3/protos

check: $(TESTS)
	@echo "RUNNING relevant_script_feature_test" && ./relevant_script_feature_test
	@echo "RUNNING script_detector_test" && ./script_detector_test
	@echo "RUNNING getonescriptspan_test" && ./script_span/getonescriptspan_test
	#@echo "RUNNING language_identifier_features_test" && ./language_identifier_features_test
	@echo "RUNNING getonescriptspan_test" && ./language_identifier_main
	@echo "Great success!"

clean:
	rm -Rf ./cld_3/protos
	rm -f $(TESTS) $(LIBCLD3) libcld3.a
	rm -f language_identifier_main.o language_identifier_features_test.o relevant_script_feature_test.o \
	  script_detector_test.o script_span/getonescriptspan_test.o
