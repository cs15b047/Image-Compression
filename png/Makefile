CC = g++
FLAGS = -std=c++11 -O3 -fpermissive -fPIC -fopenmp
LDLFAGS =-lpng -lz -lfftw3 -lfftw3_omp -ljpeg

SRCS_COMMON = refpng.cpp rle.cpp delta.cpp bucket.cpp color_transform.cpp compress.cpp dct.cpp utils.cpp quantization.cpp
SRCS_JPEG = mycodec.cpp driver.cpp
SRCS_PNG = lossless.cpp driver_lossless.cpp

COMMON_OBJS = $(SRCS_COMMON:.cpp=.o)
JPEG_OBJS = $(SRCS_JPEG:.cpp=.o)
PNG_OBJS = $(SRCS_PNG:.cpp=.o)

JPEG = mycodec
LOSSLESS = lossless

all: $(JPEG) $(LOSSLESS)
jpeg: $(JPEG)
png: $(LOSSLESS)

%.o: %.cpp
	$(CC) $(FLAGS) -c $< -o $@ $(LDFLAGS)

$(JPEG): $(COMMON_OBJS) $(JPEG_OBJS)
	$(CC) $(FLAGS) -o $(JPEG) $(COMMON_OBJS) $(JPEG_OBJS) $(LDLFAGS)

$(LOSSLESS): $(COMMON_OBJS) $(PNG_OBJS)
	$(CC) $(FLAGS) -o $(LOSSLESS) $(COMMON_OBJS) $(PNG_OBJS) $(LDLFAGS)

clean:
	rm -f $(COMMON_OBJS) $(JPEG_OBJS) $(PNG_OBJS) $(JPEG) $(LOSSLESS)
