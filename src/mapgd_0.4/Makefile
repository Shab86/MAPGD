#Testing with different standards. 
#c90 c89 c99 c9x c11 c1x gnu90 gnu89 gnu99 gnu9x gnu11 gnu1x c++98 c++03 gnu++98 gnu++03 c++11 c++0x gnu++11 gnu++0x c++1y gnu++1y
#Right now I need c++11 to support constructor delegation, which I like a lot.

MODE=-O3
STD=-std=c++11
DESTDIR=/usr/local/bin
LOCAL=../../bin/mapgd
WARNINGS=-Wall -Werror=return-type -Wno-write-strings
VERSION=-D VERSION=\"0.4.1\"
CXXFLAGS=$(VERSION) $(STD) $(MODE) $(WARNINGS) -fopenmp -fno-exceptions -pedantic
DEBUG=-g -D DEBUG
NOOMP=-D NOOMP
OBJDIR=obj
CXX=g++

#>! Just a list of flags to try at some later date.

# -oFast -ffloat-store -fexcess-precision=style -ffast-math -fno-rounding-math -fno-signaling-nans
# -fcx-limited-range -fno-math-errno -funsafe-math-optimizations -fassociative-math -freciprocal-math
# -fassociative-math -freciprocal-math -ffinite-math-only -fno-signed-zeros -fno-trapping-math
# -frounding-math -fsingle-precision-constant -fcx-limited-range -fcx-fortran-rules

#>! TODO ORGANIZE THESE!!

OBJS=allele_stat.o \
	base.o \
	binomial.o \
	estimate-individual.o \
	file-index.o \
	filter.o \
	genotype.o \
	individual-likelihood.o \
	interface.o \
	lnmultinomial.o \
	locus.o \
	newton-method-theta.o \
	mapgd.o \
	map-file.o \
	models.o \
	map2genotype.o \
	pooled-likelihood.o \
	proview.o \
	PopLD.o \
	quartet.o \
	relatedness.o \
	relatedness_data.o \
	sam2idx.o \
	sample_gof.o \
	stream-tools.o

#	proview.o \
	compare-pooled.o \
	estimate-pooled.o \
	readtable.o \
	convert.o \
	sql\db_api.o \

all: $(OBJS)
	$(CXX) $^ $(CXXFLAGS) -o $(LOCAL) 

noomp: CXXFLAGS:=$(NOOMP) $(VERSION) $(STD) $(MODE) $(WARNINGS)
noomp: all

debug: CXXFLAGS:=$(DEBUG) $(VERSION) $(STD) $(MODE) $(WARNINGS)
debug: all

install: all
	install -m 0755 $(LOCAL) $(DESTDIR)
.PHONY: install
	
clean:
	rm -f *.o
	rm -f ../../bin/*
.PHONY: clean
