#ifndef BRAYCURTIS_H
#define BRAYCURTIS_H
/*
 *  sharedbraycurtis.h
 *  Mothur
 *
 *  Created by Sarah Westcott on 3/24/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */
#include "calculator.h"

/***********************************************************************/

class BrayCurtis : public Calculator  {
	
public:
	BrayCurtis() :  Calculator("braycurtis", 1, false) {};
	EstOutput getValues(SAbundVector*) {return data;};
	EstOutput getValues(vector<RAbundVector*>);
	string getCitation() { return "http://www.mothur.org/wiki/Braycurtis"; }
private:
	
};

/***********************************************************************/



#endif
