#ifndef GENOTYPE_H_
#define GENOTYPE_H_
/* \breif genotypic likelihoods. Arbitrary ploidy
 */
#include "quartet.h"
#include "typedef.h"
#include "binomial.h"
#include "stdint.h"
#include <string>

class genotype {

private:
	std::string name_;
	uint8_t ploidy_;
	std::vecotor<float_t> probabilities_;
	binomial bin;
public:
	genotype (uint8_t ploidy){
		ploidy_=ploidy;
		probabilities_=std::vector <float>(ploidy_+1);
	};

	~genotype (){
		probabilities_.clear();
	};

	void set_probabilities(const allele_stat &, const quartet_t &);
	float_t set_probabilities(const size_t x) const;
};
#endif
