#OBJS specifies which files to compile as part of the project
OBJS = *.cpp

#CC specifies which compiler we're using
CC = g++
#INCLUDE_PATHS specifies the additional include paths we'll need
INCLUDE_PATHS = -I/opt/homebrew/Cellar/sdl2/2.0.16/include/sdl2

#LIBRARY_PATHS specifies the additional library paths we'll need
LIBRARY_PATHS = -L/opt/homebrew/Cellar/sdl2/2.0.16/lib

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL2main -lSDL2 -lSDL2_image

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = game

#This is the target that compiles our executable
compile : $(OBJS)
	$(CC) -std=c++11 $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
run :
	./$(OBJ_NAME)
clean :
	rm -f $(OBJ_NAME)


     