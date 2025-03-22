# Compiler
CXX = g++
PROTOC = protoc

# Compiler Flags
CXXFLAGS = -Wall -g -std=c++11
LDFLAGS = -lSimpleAmqpClient -lrabbitmq -lprotobuf

# ProtoBuf Files
PROTO_SRC = message.proto
PROTO_GEN_SRC = message.pb.cc
PROTO_GEN_HDR = message.pb.h

# Source Files
SRC = producer.cpp consumer.cpp consumer1.cpp consumer2.cpp create_user.cpp $(PROTO_GEN_SRC)
OBJ = $(SRC:.cpp=.o)

# Executables
TARGETS = producer consumer consumer1 consumer2 create_user
# Default rule: build all
all: $(TARGETS) user_data.bin

# Generate ProtoBuf files	
$(PROTO_GEN_SRC) $(PROTO_GEN_HDR): $(PROTO_SRC)
	$(PROTOC) --cpp_out=. $<

# Compile Create User
create_user: create_user.o $(PROTO_GEN_SRC)
	$(CXX) $(CXXFLAGS) -o $@ create_user.o $(PROTO_GEN_SRC) $(LDFLAGS)

# Compile Publisher
producer: producer.o $(PROTO_GEN_SRC)
	$(CXX) $(CXXFLAGS) -o $@ producer.o $(PROTO_GEN_SRC) $(LDFLAGS)

# Compile Consumer
consumer: consumer.o $(PROTO_GEN_SRC)
	$(CXX) $(CXXFLAGS) -o $@ consumer.o $(PROTO_GEN_SRC) $(LDFLAGS)

# Compile Consumer1
consumer1: consumer1.o $(PROTO_GEN_SRC)
	$(CXX) $(CXXFLAGS) -o $@ consumer1.o $(PROTO_GEN_SRC) $(LDFLAGS)

# Compile Consumer2
consumer2: consumer2.o $(PROTO_GEN_SRC)
	$(CXX) $(CXXFLAGS) -o $@ consumer2.o $(PROTO_GEN_SRC) $(LDFLAGS)

# Compile C++ files into object files
%.o: %.cpp $(PROTO_GEN_HDR)
	$(CXX) $(CXXFLAGS) -c $< -o $@
# Generate User Data File
user_data.bin: create_user
	./create_user
# Clean up generated files
clean:
	rm -f $(OBJ) $(TARGETS) $(PROTO_GEN_SRC) $(PROTO_GEN_HDR)

# Run Publisher with a User data file
run_producer: producer user_data.bin
	./producer user_data.bin

# Run Consumer
run_consumer: consumer
	./consumer

# Run Consumer1 and Consumer2
run_consumer1: consumer1
	./consumer1

run_consumer2: consumer2
	./consumer2
