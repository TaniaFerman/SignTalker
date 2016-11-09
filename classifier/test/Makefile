CFLAGS = `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv`

% : %.cpp 
	g++ -std=c++11 -Ddebug $< -o $@.out  $(CFLAGS) $(LIBS)

