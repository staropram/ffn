CC=clang
EASYRNG=`pkg-config easyRNG --cflags --libs`
CFLAGS=-ldl -lm ${EASYRNG} -rdynamic -g

default: ffn_serialisation_test ffn_xor_test

OBJECTS=ffn.o array.o fio.o rng.o

ffn_serialisation_test: $(OBJECTS)

ffn_xor_test: $(OBJECTS)

clean:
	rm *.o ffn_serialisation_test ffn_xor_test > /dev/null
