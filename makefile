LIB_PATHS=-L/usr/local/lib
LIBS=-lopencv_highgui -lopencv_core -lopencv_imgcodecs
all: skeleton

skeleton: skeleton.cpp
	g++ skeleton.cpp ${LIB_PATHS} ${LIBS} -o skeleton

.PHONY:clean

clean:
	rm skeleton
