CC = g++
FLAGS = -std=c++11 -O3 -fpermissive -fPIC
LDLFAGS =-lpng -lz -lfftw3 

SRCS = driver.cpp refpng.cpp mycodec.cpp rle.cpp delta.cpp bucket.cpp color_transform.cpp compress.cpp dct.cpp utils.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = mycodec

all: $(TARGET)

%.o: %.cpp
	$(CC) $(FLAGS) -c $< -o $@ $(LDFLAGS)

$(TARGET): $(OBJS)
	$(CC) $(FLAGS) -o $(TARGET) $(OBJS) $(LDLFAGS)

clean:
	rm -f $(OBJS) $(TARGET)
