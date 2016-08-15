CXXFLAGS += -std=c++11
CXXFLAGS += -I./
CXXFLAGS += -I./third_party/rapidjson/include/
CXXFLAGS += -std=c++11 -Wall -g -c -o

LIB_FILES :=-lglog -lgflags -levent -lamqp-cpp -lpthread \
	-L/usr/local/lib -lgrpc++ -lgrpc \
	-lgrpc++_reflection \
	-lprotobuf -lpthread -ldl \
	-lmysqlclient

EPUB_INFO_LIBS=./third_party/epubtools/libepubtools.a -lz -lmxml -lgumbo -lssl -lpthread

PROTOC = protoc
GRPC_CPP_PLUGIN=grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`
PROTOS_PATH = ./protos

CPP_SOURCES := \
	./base/ascii_ctype.cc	\
	./base/callback_internal.cc \
	./base/callback_helpers.cc \
	./base/file.cc	\
	./base/file_enumerator.cc \
	./base/file_path.cc	\
	./base/file_util.cc	\
	./base/location.cc	\
	./base/numbers.cc	\
	./base/once.cc	\
	./base/pickle.cc	\
	./base/ref_counted.cc	\
	./base/scoped_file.cc	\
	./base/scoped_temp_dir.cc	\
	./base/status.cc \
	./base/string_encode.cc	\
	./base/string_piece.cc	\
	./base/string_printf.cc	\
	./base/string_util.cc	\
	./base/time.cc	\
	\
	\
	./net/pc_queue.cc \
	./net/count_down_latch.cc \
	./net/connection.cc \
	./net/network_options.cc \
	./net/ip_util.cc \
	\


CPP_OBJECTS := $(CPP_SOURCES:.cc=.o)


TESTS := \
	./base/file_path_unittest \
	\


all: $(CPP_OBJECTS) $(TESTS)
.cc.o:
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

./base/file_path_unittest: ./base/file_path_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) -L/usr/local/lib -lgtest -lgtest_main -lpthread
./base/file_path_unittest.o: ./base/file_path_unittest.cc
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

## /////////////////////////////

############ rpc

clean:
	rm -fr base/*.o
	rm -fr *.o
	rm -fr ./server/*.o
	rm -fr ./server/amqp/*.o
	rm -fr ./protos/*.o 
	rm -fr ./protos/*.pb.cc 
	rm -fr ./protos/*.pb.h
	rm -fr ./rpc/epub_info_service/*.o
	@rm -fr $(TESTS)
	@rm -fr $(CPP_OBJECTS)
