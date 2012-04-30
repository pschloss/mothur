#include "trialswap2.h"


//The sum_of_squares, havel_hakimi and calc_c_score algorithms have been adapted from I. Miklos and J. Podani. 2004. Randomization of presence-absence matrices: comments and new algorithms. Ecology 85:86-92.


/**************************************************************************************************/
int TrialSwap2::sim1(vector<vector<int> > &co_matrix){ 
    try {
        vector<int> randRow;
        vector<vector<int> > tmpmatrix;
        int nrows = co_matrix.size();
        int ncols = co_matrix[0].size();
        
        //clear co_matrix
        //     for(i=0;i<nrows;i++)
        //     {
        //         co_matrix.clear();
        //     }
        
        //cout << "building matrix" << endl;
        for(int i=0;i<nrows;i++){
            if (m->control_pressed) { break; }
            
            for(int j=0;j<ncols;j++){
                double randNum = rand() / double(RAND_MAX);
                //cout << randNum << endl;
                
                if(randNum > 0.5) {
                    randRow.push_back(1);
                }else{
                    randRow.push_back(0);
                }
            }
            tmpmatrix.push_back(randRow);
            randRow.clear();
            //cout << endl;
        }
        co_matrix = tmpmatrix;
        
        return 0;
    }
	catch(exception& e) {
		m->errorOut(e, "TrialSwap2", "sim1");
		exit(1);
	}
}
/**************************************************************************************************/
/*
 *row sums fixed, columns equiprobable 
 */
void TrialSwap2::sim2(vector<vector<int> > &co_matrix)
{ 
    try {
        
        for(int i=0;i<co_matrix.size();i++)
        {
            if (m->control_pressed) { break; }
            random_shuffle( co_matrix[i].begin(), co_matrix[i].end() ); 
        }
    }
	catch(exception& e) {
		m->errorOut(e, "TrialSwap2", "sim2");
		exit(1);
	}
}
/**************************************************************************************************/
int TrialSwap2::sim2plus(vector<int> rowtotal, vector<vector<int> > &co_matrix)
{
    try {
        int nrows = co_matrix.size();
        int ncols = co_matrix[0].size();
        double cellprob = 1.0/ncols;
        vector<double> cellprobvec;
        vector<int> tmprow;
        vector<vector<int> > tmpmatrix;
        //double randNum;
        
        double start = 0.0;
        
        for(int i=0; i<ncols; i++)
        {
            if (m->control_pressed) { return 0; }
            cellprobvec.push_back(start + cellprob);
            start = cellprobvec[i];
        }
        
        for(int i=0; i<nrows; i++)
        {
            tmprow.assign(ncols, 0);
            
            while( accumulate( tmprow.begin(), tmprow.end(), 0 ) < rowtotal[i])
            {
                if (m->control_pressed) { return 0; }
                double randNum = rand() / double(RAND_MAX);
                //cout << randNum << endl;
                if(randNum <= cellprobvec[0])
                {
                    tmprow[0] = 1;
                    continue;
                }
                for(int j=1;j<ncols;j++)
                {
                    //cout << range[j] << endl;
                    if(randNum <= cellprobvec[j] && randNum > cellprobvec[j-1] && tmprow[j] != 1)
                    {
                        tmprow[j] = 1;
                    }
                }
            }
            tmpmatrix.push_back(tmprow);
            tmprow.clear();
        }
        co_matrix = tmpmatrix;
        tmpmatrix.clear();
        cellprobvec.clear();
        
        return 0;
    }
	catch(exception& e) {
		m->errorOut(e, "TrialSwap2", "sim2plus");
		exit(1);
	}
}
/**************************************************************************************************/
/*
 * same as sim2 but using initmatrix which is the initial co-occurrence matrix before transposition
 * may have to be changed depending on what matrix 'seed' is used. One way to use is to transpose
 * every null matrix before using an index and use the random matrix as a seed for the next null.
 */
/**************************************************************************************************/
void TrialSwap2::sim3(vector<vector<int> > &initmatrix)
{
    try {
        for(int i=0;i<initmatrix.size();i++)
        {
            if (m->control_pressed) { break; }
            random_shuffle( initmatrix[i].begin(), initmatrix[i].end() ); 
        }
        
    }
	catch(exception& e) {
		m->errorOut(e, "TrialSwap2", "sim3");
		exit(1);
	}
}
/**************************************************************************************************/
/*
 *
 *
 *
 */
/**************************************************************************************************/
int TrialSwap2::sim4(vector<int> columntotal, vector<int> rowtotal, vector<vector<int> > &co_matrix)
{   
    try {
        vector<double> colProb;
        vector<int> tmprow;//(ncols, 7);
        vector<vector<int> > tmpmatrix;
        vector<double> range;
        vector<double> randNums;
        int ncols = columntotal.size();
        int nrows = rowtotal.size();
        tmprow.clear();
        
        double colSum = accumulate( columntotal.begin(), columntotal.end(), 0 );
        //cout << "col sum: " << colSum << endl;
        for(int i=0;i<ncols;i++)
        {
            if (m->control_pressed) { return 0; }
            colProb.push_back(columntotal[i]/colSum);
        }
        
        double start = 0.0;
        
        for(int i=0;i<ncols;i++)
        {
            if (m->control_pressed) { return 0; }
            range.push_back(start + colProb[i]);
            start = range[i];
        }
        
        for(int i=0;i<nrows;i++)
        {
            tmprow.assign(ncols, 0);
            if (m->control_pressed) { return 0; }
            
            while ( accumulate( tmprow.begin(), tmprow.end(), 0 ) < rowtotal[i])
            {
                if (m->control_pressed) { return 0; }
                
                double randNum = rand() / double(RAND_MAX);
                if(randNum <= range[0])
                {
                    tmprow[0] = 1;
                    continue;
                }
                for(int j=1;j<ncols;j++)
                {
                    if(randNum <= range[j] && randNum > range[j-1] && tmprow[j] != 1)
                    {
                        tmprow[j] = 1;
                    }
                    
                }
            }
            tmpmatrix.push_back(tmprow);
            tmprow.clear();
        }
        
        co_matrix = tmpmatrix;
        
        return 0;
    }
	catch(exception& e) {
		m->errorOut(e, "TrialSwap2", "sim4");
		exit(1);
	}
}
/**************************************************************************************************/
/*
 * inverse of sim4, MUST BE TRANSPOSED BEFORE CO-OCCURRENCE ANALYSIS
 *
 *
 */
/**************************************************************************************************/
int TrialSwap2::sim5(vector<int> initcolumntotal,vector<int> initrowtotal, vector<vector<int> > &initmatrix)
{
    try {
        vector<double> colProb;
        vector<int> tmprow;//(ncols, 7);
        vector<vector<int> > tmpmatrix;
        vector<double> range;
        vector<double> randNums;
        int ncols = initcolumntotal.size();
        int nrows = initrowtotal.size();
        
        tmprow.clear();
        
        double colSum = accumulate( initcolumntotal.begin(), initcolumntotal.end(), 0 );
        //cout << "col sum: " << colSum << endl;
        for(int i=0;i<ncols;i++)
        {
            if (m->control_pressed) { return 0; }
            colProb.push_back(initcolumntotal[i]/colSum);
        }
        
        double start = 0.0;
        
        for(int i=0;i<ncols;i++)
        {
            if (m->control_pressed) { return 0; }
            range.push_back(start + colProb[i]);
            start = range[i];
        }
        
        for(int i=0;i<nrows;i++)
        {
            tmprow.assign(ncols, 0);
            if (m->control_pressed) { return 0; }
            
            while ( accumulate( tmprow.begin(), tmprow.end(), 0 ) < initrowtotal[i])
            {
                if (m->control_pressed) { return 0; }
                
                double randNum = rand() / double(RAND_MAX);
                if(randNum <= range[0])
                {
                    tmprow[0] = 1;
                    continue;
                }
                for(int j=1;j<ncols;j++)
                {
                    if(randNum <= range[j] && randNum > range[j-1] && tmprow[j] != 1)
                    {
                        tmprow[j] = 1;
                    }
                    
                }
            }
            tmpmatrix.push_back(tmprow);
            tmprow.clear();
        }
        
        initmatrix = tmpmatrix;
        return 0;
    }
	catch(exception& e) {
		m->errorOut(e, "TrialSwap2", "sim5");
		exit(1);
	}
}
/**************************************************************************************************/
/*
 *
 *
 *
 */
/**************************************************************************************************/
int TrialSwap2::sim6(vector<int> columntotal, vector<vector<int> > &co_matrix)
{
    try {
        vector<vector<int> > tmpmatrix;
        vector<double> colProb;
        vector<int> tmprow;
        vector<double> range;
        int ncols = columntotal.size();
        int nrows = co_matrix.size();
        
        int colSum = accumulate( columntotal.begin(), columntotal.end(), 0 );
        
        for(int i=0;i<ncols;i++)
        {
            if (m->control_pressed) { return 0; }
            colProb.push_back(columntotal[i]/double (colSum));
        }
        
        double start = 0.0;
        
        for(int i=0;i<ncols;i++)
        {
            if (m->control_pressed) { return 0; }
            range.push_back(start + colProb[i]);
            start = range[i];
        }
        
        for(int i=0;i<nrows;i++)
        {
            if (m->control_pressed) { return 0; }
            tmprow.assign(ncols, 0);
            int tmprowtotal;
            tmprowtotal = (rand() / double (RAND_MAX)) * 10;
            while ( tmprowtotal > ncols) {
                if (m->control_pressed) { return 0; }
                tmprowtotal = (rand() / double (RAND_MAX)) * 10;
            }
            //cout << tmprowtotal << endl;
            //cout << accumulate( tmprow.begin(), tmprow.end(), 0 ) << endl;
            
            while ( accumulate( tmprow.begin(), tmprow.end(), 0 ) < tmprowtotal)
            {
                if (m->control_pressed) { return 0; }
                double randNum = rand() / double(RAND_MAX);
                //cout << randNum << endl;
                if(randNum <= range[0])
                {
                    tmprow[0] = 1;
                    continue;
                }
                for(int j=1;j<ncols;j++)
                {
                    //cout << range[j] << endl;
                    if(randNum <= range[j] && randNum > range[j-1] && tmprow[j] != 1)
                    {
                        tmprow[j] = 1;
                    }
                    
                }
                
                
            }
            
            tmpmatrix.push_back(tmprow);
            tmprow.clear();
        }
        
        co_matrix = tmpmatrix;
        tmpmatrix.clear();
        
        return 0;
    }
	catch(exception& e) {
		m->errorOut(e, "TrialSwap2", "sim6");
		exit(1);
	}
}
/**************************************************************************************************/
/*
 * MUST BE TRANSPOSED BEFORE CO-OCCURRENCE ANALYSIS
 *
 *
 */
/**************************************************************************************************/
int TrialSwap2::sim7(vector<int> initrowtotal, vector<vector<int> > &co_matrix)
{
    try {
        vector<vector<double> > probmatrix;
        vector<vector<int> > tmpmatrix;
        vector<double> colProb;
        vector<double> probrow;
        vector<int> tmprow;
        vector<double> range;
        double nc;
        int ncols = co_matrix[0].size(); int nrows = co_matrix.size(); 
        
        tmpmatrix.assign(nrows, vector<int>(ncols, 0.));
        
        int rowsum = accumulate( initrowtotal.begin(), initrowtotal.end(), 0 );
        
        nc = rowsum * ncols;
        //cout << nc << endl;
        
        //assign null matrix based on probabilities
        
        double start = 0.0; // don't reset start -- probs should be from 0-1 thoughout the entire matrix 
        
        for(int i=0;i<nrows;i++)
        {
            if (m->control_pressed) { return 0; }
            //cout << initrowtotal[i]/double(nc) << endl;
            double cellprob = initrowtotal[i]/double(nc);
            //cout << cellprob << endl;
            for(int j=0;j<ncols;j++)
            {
                
                probrow.push_back(start + cellprob);
                //cout << probrow[j] << endl;
                //cout << start << endl;
                start = start + cellprob;
            }
            probmatrix.push_back(probrow);
            probrow.clear();
        }
        
        
        //while(tmprowsum < rowsum)
        //for(int k=0;k<rowsum;k++)
        int k = 0;
        while(k < rowsum)
        {
            if (m->control_pressed) { return 0; }
        done:
            //cout << k << endl;
            //tmprowsum = accumulate( tmprowtotal.begin(), tmprowtotal.end(), 0 );
            double randNum = rand() / double(RAND_MAX);
            //cout << randNum << "+" << endl;
            //special case for the first entry
            if(randNum <= probmatrix[0][0] && tmpmatrix[0][0] != 1)
            {
                tmpmatrix[0][0] = 1;
                k++;
                //cout << k << endl;
                continue;
            }
            
            
            for(int i=0;i<nrows;i++)
            {
                if (m->control_pressed) { return 0; }
                for(int j=0;j<ncols;j++)
                {
                    //cout << probmatrix[i][j] << endl;
                    if(randNum <= probmatrix[i][j] && randNum > probmatrix[i][j-1] && tmpmatrix[i][j] != 1)
                    {
                        tmpmatrix[i][j] = 1;
                        k++;
                        //cout << k << endl;
                        goto done;
                    }
                    //else
                    //k = k-1;
                }
                
            }
            
        }
        
        co_matrix = tmpmatrix;
        return 0;
    //build probibility matrix
    /* for(int i=0;i<nrows;i++)
     {
     for(int j=0;j<ncols;j++)
     {
     probrow.push_back(rowtotal[i]/nc);
     }
     probmatrix.pushback(probrow);
     probrow.clear;
     }
     */
    
    /* int colSum = accumulate( initcolumntotal.begin(), initcolumntotal.end(), 0 );
        
        for(int i=0;i<ncols;i++)
        {
            colProb.push_back(initcolumntotal[i]/double (colSum));
        }
        
        double start = 0.0;
        
        for(int i=0;i<ncols;i++)
        {
            range.push_back(start + colProb[i]);
            start = range[i];
        }
        
        for(int i=0;i<nrows;i++)
        {
            tmprow.assign(ncols, 0);
            int tmprowtotal;
            tmprowtotal = (rand() / double (RAND_MAX)) * 10;
            while ( tmprowtotal > ncols)
                tmprowtotal = (rand() / double (RAND_MAX)) * 10;
            //cout << tmprowtotal << endl;
            //cout << accumulate( tmprow.begin(), tmprow.end(), 0 ) << endl;
            
            while ( accumulate( tmprow.begin(), tmprow.end(), 0 ) < tmprowtotal)
            {
                double randNum = rand() / double(RAND_MAX);
                //cout << randNum << endl;
                if(randNum <= range[0])
                {
                    tmprow[0] = 1;
                    continue;
                }
                for(int j=1;j<ncols;j++)
                {
                    //cout << range[j] << endl;
                    if(randNum <= range[j] && randNum > range[j-1] && tmprow[j] != 1)
                    {
                        tmprow[j] = 1;
                    }
                }
            }
            
            tmpmatrix.push_back(tmprow);
            tmprow.clear();
        }

/**************************************************************************************************/
double TrialSwap2::calc_c_score (vector<vector<int> > &co_matrix, vector<int>  rowtotal, int ncols, int nrows)
{
    try {
        double cscore = 0.0;
        double maxD;
        double D;
        double normcscore = 0.0;
        int nonzeros = 0;
        //int ncols = co_matrix[0].size(); int nrows = rowtotal.size(); 
        vector<vector<double> > s; s.resize(nrows);
        for (int i = 0; i < nrows; i++) { s[i].resize(nrows,0.0); }//only fill half the matrix

        
        for(int i=0;i<nrows-1;i++)
        {
            
            for(int j=i+1;j<nrows;j++)
            {
                if (m->control_pressed) { return 0; }
                for(int k=0;k<ncols;k++)
                {
                    if((co_matrix[i][k]==1)&&(co_matrix[j][k]==1)) //if both are 1s ie co-occurrence
                        s[i][j]++; //s counts co-occurrences
                }
                
                //rowtotal[i] = A, rowtotal[j] = B, ncols = P, s[i][j] = J
                cscore += (rowtotal[i]-s[i][j])*(rowtotal[j]-s[i][j]);///(nrows*(nrows-1)/2);
                D = (rowtotal[i]-s[i][j])*(rowtotal[j]-s[i][j]);
                
                if(ncols < (rowtotal[i] + rowtotal[j]))
                {
                    maxD = (ncols-rowtotal[i])*(ncols-rowtotal[j]);
                }
                else
                {
                    maxD = rowtotal[i] * rowtotal[j];
                }
                
                if(maxD != 0)
                {
                    normcscore += D/maxD;
                    nonzeros++;    
                }            
            }
        }
        
        cscore = cscore/(double)(nrows*(nrows-1)/2);
        //cout << "normalized c score: " << normcscore/nonzeros << endl;
        
        return cscore;
    }
	catch(exception& e) {
		m->errorOut(e, "TrialSwap2", "calc_c_score");
		exit(1);
	}
}
/**************************************************************************************************/
int TrialSwap2::calc_checker (vector<vector<int> > &co_matrix, vector<int>  rowtotal, int ncols, int nrows)
{
    try {
        int cunits=0;
        //int s[nrows][ncols];
        //int ncols = co_matrix[0].size(); int nrows = rowtotal.size(); 
        vector<vector<int> > s; s.resize(nrows);
        for (int i = 0; i < nrows; i++) { s[i].resize(nrows,0); }//only fill half the matrix
        
        for(int i=0;i<nrows-1;i++)
        {
            for(int j=i+1;j<nrows;j++)
            {
                if (m->control_pressed) { return 0; }
                //s[i][j]=0;
                for(int k=0;k<ncols;k++)
                {
                    //cout << s[i][j] << endl;
                    //iterates through the row and counts co-occurrences. The total number of co-occurrences for each row pair is kept in matrix s at location s[i][j].
                    if((co_matrix[i][k]==1)&&(co_matrix[j][k]==1)) //if both are 1s ie co-occurrence
                        s[i][j]++; //s counts co-occurrences
                    
                }
                //cout << "rowtotal: " << rowtotal[i] << endl;
                //cout << "co-occurrences: " << s[i][j] << endl;
                //cunits+=(rowtotal[i]-s[i][j])*(rowtotal[j]-s[i][j]);
                if (s[i][j] == 0)
                {
                    cunits+=1;
                }
                //cunits+=s[i][j];
            }
        }
        
        return cunits;   
    }
	catch(exception& e) {
		m->errorOut(e, "TrialSwap2", "calc_checker");
		exit(1);
	}
}
/**************************************************************************************************/
double TrialSwap2::calc_vratio (int nrows, int ncols, vector<int> rowtotal, vector<int> columntotal)
{
    try {
        //int nrows = rowtotal.size();
        //int ncols = columntotal.size();
        int sumCol = accumulate(columntotal.begin(), columntotal.end(), 0 );
       // int sumRow = accumulate(rowtotal.begin(), rowtotal.end(), 0 );
        
        double colAvg = (double) sumCol / (double) ncols;
 //       double rowAvg = (double) sumRow / (double) nrows;
        
        double p = 0.0;
        
 //       double totalRowVar = 0.0;
        double rowVar = 0.0;
        double colVar = 0.0;
        
        for(int i=0;i<nrows;i++)
        {
            if (m->control_pressed) { return 0; }
            p = (double) rowtotal[i]/(double) ncols;
            rowVar += p * (1.0-p);
        } 
        
        for(int i=0;i<ncols;i++)
        {
            if (m->control_pressed) { return 0; }
            colVar += pow(((double) columntotal[i]-colAvg),2);
        }
        
        colVar = (1.0/(double)ncols) * colVar;
        
        return colVar/rowVar;
    }
    catch(exception& e) {
        m->errorOut(e, "TrialSwap2", "calc_vratio");
        exit(1);
    }
         
}
/**************************************************************************************************/
int TrialSwap2::calc_combo (int nrows, int ncols, vector<vector<int> > &nullmatrix)
{
    try {
        //need to transpose so we can compare rows (row-major order)
        int tmpnrows = nrows;
        vector<vector<int> > tmpmatrix;

        vector<int> tmprow;
        if(!tmpmatrix.empty())
            tmpmatrix.clear();
        for (int i=0;i<ncols;i++)
        {       
            for (int j=0;j<nrows;j++)
            {
                tmprow.push_back(nullmatrix[j][i]);
            }

            tmpmatrix.push_back(tmprow);
            tmprow.clear();
        }

        int unique = 0;
        int match = 0;
        for(int j=0;j<ncols;j++)
        {
            match = 0;
            for(int i=j+1;i<=ncols;i++)
            {
                //comparing matrix rows
                if( (tmpmatrix[j] == tmpmatrix[i])) 
                {
                    match++;
                    break;
                }
            }        

            //on the last iteration of a previously matched row it will add itself because it doesn't match any following rows, so that combination is counted
            if (match == 0)
                unique++;
    }
    return unique;
    }
    catch(exception& e) {
        m->errorOut(e, "TrialSwap2", "calc_combo");
        exit(1);
    }
} 
/**************************************************************************************************/
int TrialSwap2::swap_checkerboards (vector<vector<int> > &co_matrix)
{
    try {
        int ncols = co_matrix[0].size(); int nrows = co_matrix.size(); 
        int i, j, k, l;
        i = m->getRandomIndex(nrows-1);
        while((j = m->getRandomIndex(nrows-1) ) == i ) {;if (m->control_pressed) { return 0; }}
        k = m->getRandomIndex(ncols-1);
        while((l = m->getRandomIndex(ncols-1)) == k ) {;if (m->control_pressed) { return 0; }}

        if((co_matrix[i][k]*co_matrix[j][l]==1 && co_matrix[i][l]+co_matrix[j][k]==0)||(co_matrix[i][k]+co_matrix[j][l]==0 && co_matrix[i][l]*co_matrix[j][k]==1)) //checking for checkerboard value and swap
        {
            co_matrix[i][k]=1-co_matrix[i][k];
            co_matrix[i][l]=1-co_matrix[i][l];
            co_matrix[j][k]=1-co_matrix[j][k];
            co_matrix[j][l]=1-co_matrix[j][l];

        }

        return 0;
    }
    catch(exception& e) {
        m->errorOut(e, "TrialSwap2", "swap_checkerboards");
        exit(1);
    }
}
/**************************************************************************************************/
double TrialSwap2::calc_pvalue_greaterthan (vector<double> scorevec, double initialscore)
{
    try {
        int runs = scorevec.size();
        double p = 0.0;
        for( int i=0;i<runs;i++)
        {
            if (m->control_pressed) { return 0; }
            if(scorevec[i]>=initialscore)
                p++;
        }
        return p/(double)runs;
    }
    catch(exception& e) {
        m->errorOut(e, "TrialSwap2", "calc_pvalue_greaterthan");
        exit(1);
    }
}
/**************************************************************************************************/
double TrialSwap2::calc_pvalue_lessthan (vector<double> scorevec, double initialscore)
{
    try {
        int runs = scorevec.size();
        double p = 0.0;
        for( int i=0;i<runs;i++)
        {
            if (m->control_pressed) { return 0; }
            if(scorevec[i]<=initialscore)
                p++;
        }
        return p/(double)runs;
    }
    catch(exception& e) {
        m->errorOut(e, "TrialSwap2", "calc_pvalue_lessthan");
        exit(1);
    }
}
/**************************************************************************************************/
double TrialSwap2::t_test (double initialscore, int runs, double nullMean, vector<double> scorevec)
{
    try {
        double t;
        double sampleSD;
        double sum = 0;
        
        for(int i=0;i<runs;i++)
        {
            if (m->control_pressed) { return 0; }
            sum += pow((scorevec[i] - nullMean),2);
            //cout << "scorevec[" << i << "]" << scorevec[i] << endl;
        }
        
        m->mothurOut("nullMean: " + toString(nullMean)); m->mothurOutEndLine();
        
        m->mothurOut("sum: " + toString(sum));  m->mothurOutEndLine();
        
        sampleSD = sqrt( (1/runs) * sum );
        
        m->mothurOut("samplSD: " + toString(sampleSD));  m->mothurOutEndLine();
        
        t = (nullMean - initialscore) / (sampleSD / sqrt(runs));
        
        return t;
    }
    catch(exception& e) {
        m->errorOut(e, "TrialSwap2", "t_test");
        exit(1);
    }
}
/**************************************************************************************************/
int TrialSwap2::print_matrix(vector<vector<int> > &matrix, int nrows, int ncols)
{
    try {
         m->mothurOut("matrix:");  m->mothurOutEndLine();
        
        for (int i = 0; i < nrows; i++)
        {
            if (m->control_pressed) { return 0; }
            for (int j = 0; j < ncols; j++)
            {
                m->mothurOut(toString(matrix[i][j]));            
            }    
            m->mothurOutEndLine();
        }
        return 0;
    }
    catch(exception& e) {
        m->errorOut(e, "TrialSwap2", "print_matrix");
        exit(1);
    }
}
/**************************************************************************************************/





