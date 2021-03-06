/* 

Program estimatePooled.cpp:

	1) input from a list of site-specific quartets from a pooled population sample;

	2) identify the major and minor alleles, obtain the maximum-likelihood estimates of allele frequencies, and significance levels.

The designated major nucleotide is simply the one with the highest rank, and the minor nucleotide the one with the second highest rank.
	If the top three ranks are all equal, the site is treated as unresolvable, with both major and minor nucelotides designated in the output by a *.
	If the second and third ranks are equal but lower than the major-nucleotide count, the site is treated as monomorphic, with the minor nucleotide designated by a *.
	
Input File: six columns; first two are arbitrary identifiers of the site (e.g., chromosome and position); final four are integer numbers of reads of A, C, G, T.
	Columns are tab delimited.
	Default name is "datain.txt".

Output File: two columns of site identifiers; major allele; minor allele; major-allele frequency; minor-allele frequency; error rate; likeilhood-ratio test of polymorphism. 
	Columns are comma delimited.
	Default name is "dataout.txt".

***** Note that significance at the 0.05, 0.01, 0.001 levels requires that the likelihood-ratio test statistic exceed 3.841, 6.635, and 10.827, respectively. 

***** The 95% support interval can be obtained by determining the changes in the estimate of p in both directions required to reduce the log likelihood by 3.841
      although this is not currently implemented. 
*/

#include "estimate-pooled.h"

int estimatePooled(int argc, char *argv[])
{

	/* variables that can be set from the command locus */
	std::string infile="";
	std::string outfile="";

	bool verbose=false;
	bool quite=false;
	float_t EMLMIN=0.001;
	float_t a=0.00;

	std::vector <size_t> pop;

	/* sets up the help messages and options */

	Environment env;
	env.set_name("mapgd ep");
	env.set_version(VERSION);
	env.set_author("Matthew Ackerman and Michael Lynch");
	env.set_description("Uses a maximum likelihood approach to estimates allele frequencies in pooled population genomic data");

	env.optional_arg('i',"input", 	infile,	"an error occured while setting the name of the input file", "sets the input file for the program (default 'datain.txt')");
	env.optional_arg('o',"output", 	outfile,	"an error occured while setting the name of the output file", "sets the output file for the program (default 'dataout.txt')");
	env.optional_arg('p',"populations", pop, 	"please provide a list of integers", "choose populations to compare");
	env.optional_arg('m',"minerror", EMLMIN, 	"please provide a float", "minimum error rate.");
	env.optional_arg('a',"alpha",   a, 		"please provide a float", "alpha.");
	env.flag(	'h',"help", 	&env, 		&flag_help, 	"an error occured while displaying the help message", "prints this message");
	env.flag(	'v',"version", 	&env, 		&flag_version, 	"an error occured while displaying the version message", "prints the program version");
	env.flag(	'V',"verbose", 	&verbose,	&flag_set, 	"an error occured", "prints more information while the command is running");
	env.flag(	'q',"quite", 	&quite,		&flag_set, 	"an error occured", "prints less information while the command is running");

	/* gets any command locus options */
	if ( parsargs(argc, argv, env) ) print_usage(env);
	/* Point to the input and output files. */


	Indexed_file <Pooled_data> out_pooled;
	Indexed_file <Locus> in_locus;
	Pooled_data site;

	if (infile.size()!=0) { 
		in_locus.open(infile.c_str(), std::fstream::in); 
	} else {
		in_locus.open(std::fstream::in);
	}

	if (outfile.size()!=0) {
		out_pooled.open(outfile.c_str(), std::ofstream::out);
	} else {
		out_pooled.open(std::fstream::out);
	}

	Locus locus=in_locus.read_header();
	out_pooled.set_index(in_locus.get_index() );
	site.set_sample_names(locus.get_sample_names() );
	out_pooled.write_header(site);

	float_t pmaj=0, maxll=0;					/* fraction of putative major reads among the total of major and minor */
	float_t popN=0, eml=0;						/* maximum-likelihood estimates of the error rate, null error rate, and major-allele frequency */
	float_t *llhoodM, *llhoodP, *llhoodF, *llstat, *llstatc,*pmlP;	/* loglikelihood sums */

	/* Open the output and input files. */

	if ( pop.size()==0 ) { 
		pop.clear();
		for (size_t x=0; x<locus.get_sample_names().size(); ++x) pop.push_back(x);
	};

	for (size_t x=0; x<pop.size(); ++x) unmask(locus.get_quartet(pop[x]) );

	/* Start inputting and analyzing the data locus by locus. */
	/* The profile format can contain quartets for many populations and these populations need to be
	 * iterated across.
	 */

	llhoodP=new float_t[pop.size()];
	llhoodF=new float_t[pop.size()];
	llhoodM=new float_t[pop.size()];
	pmlP=new float_t[pop.size()];
	llstat=new float_t[pop.size()];
	llstatc=new float_t[pop.size()];	//

	lnmultinomial multi(4);			//a class for our probability function (which is just a multinomial distribution).

	while(in_locus.read(locus).table_is_open() ){
		//sorts the reads at the site from most frequenct (0) to least frequenct (3) (at metapopulation level);

		/* Calculate the ML estimates of the major / minor allele frequencies and the error rate. */
                locus.sort();

		site.set_abs_pos(locus.get_abs_pos() );

		site.coverage=locus.getcoverage();

		site.major.base=locus.getindex(0);
		site.minor.base=locus.getindex(1);

                /* Calculate the ML estimates of the major pooled allele frequency and the error rate. */
		popN=locus.getcoverage();

		site.error= (float_t) (popN-locus.getcount(0)-locus.getcount(1) )*3. / ( 2.* popN ) ;
		if (site.error<EMLMIN) site.error=EMLMIN;

                /* Calculate the likelihoods under the reduced model assuming no variation between populations. */

                for (size_t x=0; x<pop.size(); ++x) {
			popN=locus.getcoverage();
			site.error= (float_t) (popN-locus.getcount(0)-locus.getcount(1) )*3. / ( 2.* popN ) ;
			if (site.error<EMLMIN) site.error=EMLMIN;
			eml=site.error;

                        pmaj = (float_t) locus.getcount(pop[x],0) / (float_t) ( locus.getcount(pop[x], 1) + locus.getcount(pop[x],0) );	
                        pmlP[x] = (pmaj * (1.0 - (2.0 * eml / 3.0) ) ) - (eml / 3.0);
                        pmlP[x] = pmlP[x] / (1.0 - (4.0 * eml / 3.0) );

			if (pmlP[x]<0) pmlP[x]=0.;
			if (pmlP[x]>1) pmlP[x]=1.;

			popN=locus.getcoverage(pop[x]);
			site.error=(float_t) (popN-locus.getcount(pop[x],1 ) ) / ( popN ) ;
			if (site.error<EMLMIN) site.error=EMLMIN;
			if (site.error>0.5) site.error=0.5;
//			std::cerr << "[" << site.error << ", ";
			multi.set(&fixedmorphicmodel, site.to_allele(x) );
                        llhoodF[x]=multi.lnprob(locus.get_quartet(pop[x]).base );

			site.error=(float_t) (popN-locus.getcount(pop[x],0 ) ) / ( popN ) ;
			if (site.error<EMLMIN) site.error=EMLMIN;
			if (site.error>0.5) site.error=0.5;
//	std::cerr << site.error << ", ";
			multi.set(&monomorphicmodel, site.to_allele(x) );
                        llhoodM[x]=multi.lnprob(locus.get_quartet(pop[x]).base );

			popN=locus.getcoverage();
			site.error=(float_t) (popN-locus.getcount(0)-locus.getcount(1) )*3. / ( 2.* popN ) ;
			if (site.error<EMLMIN) site.error=EMLMIN;
			site.p[x]=pmlP[x];
			popN=locus.getcoverage(pop[x]);
//			std::cerr << site.error << "] ";
			multi.set(&polymorphicmodel, site.to_allele(x) );
                        llhoodP[x]=multi.lnprob(locus.get_quartet(pop[x]).base );

			site.cov[x]=popN;
                };
		popN=locus.getcoverage();
		site.error= (float_t) (popN-locus.getcount(0)-locus.getcount(1) )*3. / ( 2.* popN ) ;
		if (site.error<EMLMIN) site.error=EMLMIN;

                /* Likelihood ratio test statistic; asymptotically chi-square distributed with one degree of freedom. */

                maxll=0;
                for (size_t x=0; x<pop.size(); ++x){
                        //llhoodMS=0;
                        //llhoodPS=0;
                        //llhoodFS=0;
			/*
                        for (int y=0; y<pop.size(); ++y){
                                llhoodMS+=llhoodM[y];
                                if (x!=y){
					llhoodPS+=llhoodM[y];
					llhoodFS+=llhoodM[y];
				}
                                else {
					llhoodPS+=llhoodP[y];
					llhoodFS+=llhoodF[y];
				};
                        };*/
			site.polyll[x] =(2.0 * (llhoodP[x] - std::max(llhoodM[x], llhoodF[x]) ) );
			site.fixedll[x]=(2.0 * (llhoodF[x] - llhoodM[x] ) );
                        if (site.polyll[x]>maxll) maxll=site.polyll[x];
                };

                if (maxll>=a){
			out_pooled.write(site);
                };
	}
        delete [] llhoodP;
        delete [] llhoodF;
        delete [] llhoodM;
        delete [] pmlP;
        delete [] llstat;
        delete [] llstatc;
	out_pooled.close();
	/* Ends the computations when the end of file is reached. */
	exit(0);
}
