LIB_PATHS=-L/usr/local/Cellar/opencv/2.4.12/lib
LIBS=-lopencv_highgui -lopencv_core
all: skeleton

skeleton: skeleton.cpp
	g++ skeleton.cpp ${LIB_PATHS} ${LIBS} -o skeleton

.PHONY:clean

clean:
	rm skeleton
