C++     = g++
CC	= $(C++)
NNF	= nnf
CFLAGS	= -Wall -O3 -I$(NNF) #-DMP
LDFLAGS = -L$(NNF) -lnnf #-lgmp

all:		models bestmodel

models:
		$(C++) -o models models.o $(LDFLAGS)

bestmodel:
		$(C++) -o bestmodel bestmodel.o $(LDFLAGS)

$(NNF)/libnnf.a:
		(cd $(NNF); make)

.cc.o:
		$(C++) $(CFLAGS) -c $<

models:		models.o $(NNF)/libnnf.a
bestmodel:	bestmodel.o $(NNF)/libnnf.a
models.o:	$(NNF)/nnf.h $(NNF)/satlib.h
bestmodel.o:	$(NNF)/nnf.h $(NNF)/satlib.h

clean:
		(cd $(NNF); make clean)
		rm -fr *.o *~ core models bestmodel

