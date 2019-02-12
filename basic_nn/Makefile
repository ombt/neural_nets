
basenn:	\
	obj/main.o \
	obj/BaseNN.o \
	obj/StdStringUtils.o \
	obj/RegexMatch.o
	g++ -g -o basenn \
		obj/main.o \
		obj/BaseNN.o \
		obj/StdStringUtils.o \
		obj/RegexMatch.o

obj/main.o:	\
	src/main.C \
	hdr/BaseNN.H
	g++ -c -g -Ihdr -o obj/main.o src/main.C

obj/BaseNN.o: \
	src/BaseNN.C \
	hdr/BaseNN.H \
	hdr/ExtUseCntPtr.H \
	hdr/Atomic.H
	g++ -c -g -Ihdr -o obj/BaseNN.o src/BaseNN.C

obj/StdStringUtils.o: \
	src/StdStringUtils.C \
	hdr/StdStringUtils.H \
	hdr/StdStringUtils.I
	g++ -c -g -Ihdr -o obj/StdStringUtils.o src/StdStringUtils.C

obj/RegexMatch.o: \
	src/RegexMatch.C \
	hdr/RegexMatch.H 
	g++ -c -g -Ihdr -o obj/RegexMatch.o src/RegexMatch.C

clean:
	-(rm -f obj/*.o basenn);
	
