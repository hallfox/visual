LIB_PATHS=-L/usr/local/lib
LIBS=-lopencv_highgui -lopencv_core -lopencv_imgcodecs
all: visual

visual: visual.cpp
	g++ visual.cpp ${LIB_PATHS} ${LIBS} -o visual

.PHONY:clean

clean:
	rm visual
