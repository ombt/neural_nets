
basenn:	\
	main.o \
	BaseNN.o \
	StdStringUtils.o \
	RegexMatch.o
	g++ -I. -o basenn \
		main.o \
		BaseNN.o \
		StdStringUtils.o \
		RegexMatch.o

main.o:	\
	main.C \
	BaseNN.H
	g++ -c -I. -o main.o main.C

BaseNN.o: \
	BaseNN.C \
	BaseNN.H \
	ExtUseCntPtr.H \
	Atomic.H
	g++ -c -I. -o BaseNN.o BaseNN.C

StdStringUtils.o: \
	StdStringUtils.C \
	StdStringUtils.H \
	StdStringUtils.I
	g++ -c -I. -o StdStringUtils.o StdStringUtils.C

RegexMatch.o: \
	RegexMatch.C \
	RegexMatch.H 
	g++ -c -I. -o RegexMatch.o RegexMatch.C

clean:
	-(rm -f *.o basaenn);
	
