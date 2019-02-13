
DEBUG = 
# DEBUG = -DLOCAL_DEBUG=1

INCLUDES = -Ihdr

CPP = g++
CPP_OPTS = -g -c

LD = g++
LD_OPTS = -g

base_nn:	\
	obj \
	obj/main.o \
	obj/BaseNN.o \
	obj/StdStringUtils.o \
	obj/RegexMatch.o
	$(LD) $(LD_OPTS) -o base_nn \
		obj/main.o \
		obj/BaseNN.o \
		obj/StdStringUtils.o \
		obj/RegexMatch.o

obj:
	mkdir -p obj

obj/main.o:	\
	src/main.C \
	hdr/BaseNN.H
	$(CPP) $(CPP_OPTS) \
		$(INCLUDES) \
		$(DEBUG) \
		-o obj/main.o \
		src/main.C

obj/BaseNN.o: \
	src/BaseNN.C \
	hdr/BaseNN.H \
	hdr/ExtUseCntPtr.H \
	hdr/Atomic.H
	$(CPP) $(CPP_OPTS) \
		$(INCLUDES) \
		$(DEBUG) \
		-o obj/BaseNN.o \
		src/BaseNN.C

obj/StdStringUtils.o: \
	src/StdStringUtils.C \
	hdr/StdStringUtils.H \
	hdr/StdStringUtils.I
	$(CPP) $(CPP_OPTS) \
		$(INCLUDES) \
		$(DEBUG) \
		-o obj/StdStringUtils.o \
		src/StdStringUtils.C

obj/RegexMatch.o: \
	src/RegexMatch.C \
	hdr/RegexMatch.H 
	$(CPP) $(CPP_OPTS) \
		$(INCLUDES) \
		$(DEBUG) \
		-o obj/RegexMatch.o \
		src/RegexMatch.C

clean:
	-(rm -fr obj base_nn);
	
