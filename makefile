FLAGS_GLEW=`pkg-config --cflags glew`
LIB_GLEW=`pkg-config --libs glew`
FLAGS_GLFW=`pkg-config --cflags glfw3`
LIB_GLFW=`pkg-config --libs glfw3`

all:
	g++ src/glfw_game.cpp src/chip8.cpp -o bin/chip8 $(LIB_GLEW) $(LIB_GLFW) $(FLAGS_GLFW) $(FLAGS_GLEW) -framework OpenGl -framework Cocoa -framework IOKit -framework CoreVideo
