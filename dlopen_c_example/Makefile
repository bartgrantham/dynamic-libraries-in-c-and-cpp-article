ALL = dlopen testlib1.so testlib2.so

all: $(ALL)

clean:
	rm -f $(ALL)

dlopen: dlopen.c
	$(CC) dlopen.c -o $@

testlib1.so: testlib1.c
	$(CC) -shared testlib1.c -o testlib1.so

testlib2.so: testlib2.c
	$(CC) -shared testlib2.c -o testlib2.so

test: all
	./dlopen testlib1.so testa
	./dlopen testlib1.so testb
	./dlopen testlib2.so testa
	./dlopen testlib2.so testb
