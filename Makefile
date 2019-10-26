CFLAGS= -std=gnu99 -pthread -Wall -Wextra -pedantic -g
LDFLAGS= -pthread

ALL: googletest sample test_queue gtest_queue

# dependencies
sample: sample.o queue.o mymutex.o
queue.o: queue.c queue.h mymutex.h
sample.o: sample.c queue.h
test_queue: test_queue.o queue.o mymutex.o
test_queue.o: test_queue.c queue.h
mymutex.o: mymutex.c mymutex.h

gtest_queue: gtest_queue.o queue.o mymytex.o
	g++ -pthread -std=c++11 gtest_queue.o queue.o -o gtest_queue -L./googletest/build/lib -lgtest
gtest_queue.o: gtest_queue.cpp queue.h
	g++ -pthread -std=c++11 -I./googletest/googletest/include -g -c gtest_queue.cpp

.PHONY: clean test gtest

test: test_queue
	./test_queue init_free
	./test_queue push
	./test_queue pop
	./test_queue drop
	./test_queue length
	./test_queue empty

gtest: gtest_queue
	./gtest_queue

googletest:
	(git clone git://github.com/google/googletest.git ; cd googletest ; mkdir build ; cd build ; cmake .. ; make)

clean:
	rm -f *.o sample test_queue gtest_queue
	rm -rf ./googletest
