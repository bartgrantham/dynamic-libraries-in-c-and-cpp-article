ALL = dlopen foo.so bar.so baz.so

all: $(ALL)

clean:
	rm -f $(ALL)

dlopen: dlopen.cc
	$(CXX) dlopen.cc -o $@

foo.so: foo.cc
	$(CXX) -fPIC -shared foo.cc -o foo.so

bar.so: bar.cc
	$(CXX) -fPIC -shared bar.cc -o bar.so

baz.so: baz.cc
	$(CXX) -fPIC -shared baz.cc -o baz.so

test: $(ALL)
	./dlopen foo.so
	./dlopen bar.so
	./dlopen baz.so
