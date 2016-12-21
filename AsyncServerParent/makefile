CC = g++ -std=c++11
CC_FLAGS = -w
LIBS = -lodbc -lboost_system -lboost_serialization -lboost_thread -lprotobuf
INCLUDES =

EXEC = run
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

# Main target                                                                                                                                                                                               
$(EXEC): $(OBJECTS)
	$(CC) -c $(OBJECTS) -o $(EXEC) $(LIBS)
	ar rcs libasync_server.a $(OBJECTS)

# To obtain object files                                                                                                                                                                                    
%.o: %.cpp
	$(CC) -c $(INCLUDES) $(CC_FLAGS) $< -o $@

# To remove generated files                                                                                                                                                                                 
clean:
	rm -f $(EXEC) $(OBJECTS)
