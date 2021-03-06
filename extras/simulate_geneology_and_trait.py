import random
import numpy
import os
import math

Pfreq=0.2
SIZE=100
bp=1
GEN=20

File=open("seq.gcf", 'w')
File05=open("seq05.gcf", 'w')
File10=open("seq10.gcf", 'w')
File15=open("seq15.gcf", 'w')
File20=open("seq20.gcf", 'w')
File25=open("seq25.gcf", 'w')
File30=open("seq30.gcf", 'w')
File35=open("seq35.gcf", 'w')
File40=open("seq40.gcf", 'w')
File45=open("seq45.gcf", 'w')

def seq(File, P, e, F, N):
	global File2
	global bp
	lnc=math.log(1.0-e)
	lnch=math.log((1.0-e)/2.0+e/6)
	lne=math.log(e/3.)

        File.write( '\t'.join(map(str, [1, bp, "C", "A", P, e] ) ) )
	bp+=1
	for f in F:
	        if f.gen==2:
	                [M, m, E]=numpy.random.multinomial(N,[(1.-e), e/3., 2.*e/3.])
	        elif f.gen==1:
	                [M, m, E]=numpy.random.multinomial(N,[(1.-e)/2.0+e/6., (1.-e)/2.0+e/6, 2.*e/3.])
	        else:
	                [M, m, E]=numpy.random.multinomial(N,[e/3.0, (1.-e), 2.*e/3.])

		MM=lnc*M+lne*(m+E)#-math.log(P**2)
		Mm=lnch*(M+m)+lne*E#-math.log(2*P*Q)
		mm=lnc*m+lne*(M+E)#-math.log(Q**2)

		norm=math.log(math.exp(MM)+math.exp(Mm)+math.exp(mm) )

		File.write('\t'+'\t'.join(map(str, [-MM+norm, -Mm+norm, -mm+norm, N] ) ) )
	File.write('\n')

def f(h, q):
	return ( h-2*q)**2 /(2.*q*(1.-q) )-1. 

def color(num):
	if num>0:
		if (abs(num)>1):
			num=1
		HEX=int(255*abs(1-abs(num) ) )
		return "\"#"+format(HEX, "02x")+format(HEX, "02x")+"ff\""
	else:
		if (abs(num)>1):
			num=1
		HEX=int(255*abs(1-abs(num) ) )
		return "\"#ff"+format(HEX, "02x")+format(HEX, "02x")+"\""

def get_freq (array):
	Sum=0
	for ind in array:
		Sum+=ind.gen
	return Sum

class individual:
	def __init__ (self, name, mother=None, father=None):
		self.mother=mother
		self.father=father
		self.name=name
		if mother is None:
			if random.random()>Pfreq:
				self.maternal_hap=1
			else:
				self.maternal_hap=0
		else:
			if mother.gen==1:
				self.maternal_hap=random.randint(0,1)
			elif mother.gen==2:
				self.maternal_hap=1
			elif mother.gen==0:
				self.maternal_hap=0
		if father is None:
			if random.random()>Pfreq:
				self.paternal_hap=1
			else:
				self.paternal_hap=0
		else:
			if father.gen==1:
				self.paternal_hap=random.randint(0,1)
			elif father.gen==2:
				self.paternal_hap=1
			elif father.gen==0:
				self.paternal_hap=0
				
		self.gen=self.paternal_hap+self.maternal_hap
		self.labeled=False
		self.Num=[ 0 for y in range(SIZE*2) ]
		self.Den=[ 0 for y in range(SIZE*2) ]
		self.z=0

	def rand (self, P):
		mother=self.mother
		father=self.father
		if mother is None:
			if random.random()>P:
				self.maternal_hap=1
			else:
				self.maternal_hap=0
		else: 
			if mother.gen==1:
				self.maternal_hap=random.randint(0,1)
			elif mother.gen==2:
				self.maternal_hap=1
			elif mother.gen==0:
				self.maternal_hap=0
		if father is None:
			if random.random()>P:
				self.paternal_hap=1
			else:
				self.paternal_hap=0
		else:
			if father.gen==1:
				self.paternal_hap=random.randint(0,1)
			elif father.gen==2:
				self.paternal_hap=1
			elif father.gen==0:
				self.paternal_hap=0
		self.gen=self.paternal_hap+self.maternal_hap
		return self
				
F = [ [] for y in range(GEN)]

for x in range(0, SIZE):
	F[0].append(individual("P") )

for y in range(1, GEN):
	for x in range(0, SIZE):
		z=random.randint(0, SIZE-1)
		w=random.randint(0, SIZE-1)
		F[y].append(individual("F", F[y-1][z], F[y-1][w]))
		if y==1:
			if ( not(F[0][z].labeled) ):
				Q=float(get_freq(F[0]) )/float(2*SIZE)
				F[0][z].labeled=True
			if ( not(F[0][w].labeled) ):
				Q=float(get_freq(F[0]) )/float(2*SIZE)
				F[0][w].labeled=True
		F[y][x].labeled=True

count = [[ [0,0,0] for x in range(3)] for y in range(SIZE*2+1)]

a=0

LIM=10000

while a<LIM:
	if a%1000==0:
		print a
	Pfreq=random.random()
	for y in range(0, GEN):
		for x in range(0, SIZE):
			F[y][x].rand(Pfreq)
	if get_freq(F[-1])==0 or get_freq(F[-1])==SIZE*2:
		continue
	a+=1

	Q=get_freq(F[-1]) 

	seq(File, float(Q)/float(SIZE*2), 0.01, F[-1], 50)

LIM=80

s=0.5
h=0
E=2

for a in range(0, LIM):
	Pfreq=0.5#random.random()
	for y in range(0, GEN):
		for x in range(0, SIZE):
			F[y][x].rand(Pfreq)
			F[y][x].z+=F[y][x].gen*s#+(F[y][x].gen==1)*h
#	Va=2*P*Q*alpha**2
#	Vd=(2*P*Q*d)**2
#	Vg=Va+Vd

	a+=1

for x in range(0, SIZE):
	print x, F[-1][x].z
