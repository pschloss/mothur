/*
 *  catchallcommand.cpp
 *  Mothur
 *
 *  Created by westcott on 5/11/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "catchallcommand.h"
#include "globaldata.hpp"

//**********************************************************************************************************************
vector<string> CatchAllCommand::getValidParameters(){	
	try {
		string AlignArray[] =  {"sabund","shared","label","inputdir","outputdir"};
		vector<string> myArray (AlignArray, AlignArray+(sizeof(AlignArray)/sizeof(string)));
		return myArray;
	}
	catch(exception& e) {
		m->errorOut(e, "CatchAllCommand", "getValidParameters");
		exit(1);
	}
}
//**********************************************************************************************************************
CatchAllCommand::CatchAllCommand(){	
	try {
		//initialize outputTypes
		vector<string> tempOutNames;
		outputTypes["csv"] = tempOutNames;
		outputTypes["summary"] = tempOutNames;
	}
	catch(exception& e) {
		m->errorOut(e, "CatchAllCommand", "CatchAllCommand");
		exit(1);
	}
}
//**********************************************************************************************************************
vector<string> CatchAllCommand::getRequiredParameters(){	
	try {
		string AlignArray[] =  {"sabund","shared","or"};
		vector<string> myArray (AlignArray, AlignArray+(sizeof(AlignArray)/sizeof(string)));
		return myArray;
	}
	catch(exception& e) {
		m->errorOut(e, "CatchAllCommand", "getRequiredParameters");
		exit(1);
	}
}
//**********************************************************************************************************************
vector<string> CatchAllCommand::getRequiredFiles(){	
	try {
		vector<string> myArray;
		return myArray;
	}
	catch(exception& e) {
		m->errorOut(e, "CatchAllCommand", "getRequiredFiles");
		exit(1);
	}
}
/**************************************************************************************/
CatchAllCommand::CatchAllCommand(string option)  {	
	try {
		globaldata = GlobalData::getInstance();
		abort = false;
		allLines = 1;
		
		//allow user to run help
		if(option == "help") { help(); abort = true; }
		
		else {
			//valid paramters for this command
			string Array[] =  {"shared","sabund","label","inputdir","outputdir"};
			vector<string> myArray (Array, Array+(sizeof(Array)/sizeof(string)));
			
			OptionParser parser(option);
			map<string,string> parameters = parser.getParameters();
			
			ValidParameters validParameter;
			map<string, string>::iterator it;
		
			//check to make sure all parameters are valid for command
			for (it = parameters.begin(); it != parameters.end(); it++) { 
				if (validParameter.isValidParameter(it->first, myArray, it->second) != true) {  abort = true;  }
			}
			
			//initialize outputTypes
			vector<string> tempOutNames;
			outputTypes["csv"] = tempOutNames;
			outputTypes["summary"] = tempOutNames;
			
			//if the user changes the input directory command factory will send this info to us in the output parameter 
			string inputDir = validParameter.validFile(parameters, "inputdir", false);		
			if (inputDir == "not found"){	inputDir = "";		}
			else {
				string path;
				it = parameters.find("sabund");
				//user has given a template file
				if(it != parameters.end()){ 
					path = m->hasPath(it->second);
					//if the user has not given a path then, add inputdir. else leave path alone.
					if (path == "") {	parameters["sabund"] = inputDir + it->second;		}
				}
				
				it = parameters.find("shared");
				//user has given a template file
				if(it != parameters.end()){ 
					path = m->hasPath(it->second);
					//if the user has not given a path then, add inputdir. else leave path alone.
					if (path == "") {	parameters["shared"] = inputDir + it->second;		}
				}
			}

			//check for required parameters
			sabundfile = validParameter.validFile(parameters, "sabund", true);
			if (sabundfile == "not open") { sabundfile = ""; abort = true; }
			else if (sabundfile == "not found") { sabundfile = "";  }
			else { globaldata->setSabundFile(sabundfile); globaldata->setFormat("sabund"); }
			
			sharedfile = validParameter.validFile(parameters, "shared", true);
			if (sharedfile == "not open") { sharedfile = ""; abort = true; }
			else if (sharedfile == "not found") { sharedfile = "";   }
			
			//check for shared file loaded during read.otu
			if (sharedfile == "") {
				if (globaldata->getSharedFile() != "") { sharedfile = globaldata->getSharedFile(); }
			}
			
			string label = validParameter.validFile(parameters, "label", false);			
			if (label == "not found") { label = ""; }
			else { 
				if(label != "all") {  m->splitAtDash(label, labels);  allLines = 0;  }
				else { allLines = 1;  }
			}
		
			if ((sharedfile == "") && (sabundfile == "")) { m->mothurOut("You must provide a sabund or shared file for the catchall command."); m->mothurOutEndLine(); abort=true; }

			//if the user changes the output directory command factory will send this info to us in the output parameter 
			outputDir = validParameter.validFile(parameters, "outputdir", false);		
			if (outputDir == "not found"){	
				if (sabundfile != "") {  outputDir = m->hasPath(sabundfile); }
				else { outputDir = m->hasPath(sharedfile); }
			}
		}

	}
	catch(exception& e) {
		m->errorOut(e, "CatchAllCommand", "CatchAllCommand");
		exit(1);
	}
}
//**********************************************************************************************************************

void CatchAllCommand::help(){
	try {
		m->mothurOut("The catchall command interfaces mothur with the catchall program written by Linda Woodard, Sean Connolly and John Bunge.\n");
		m->mothurOut("For more information about catchall refer to http://www.northeastern.edu/catchall/index.html \n");
		m->mothurOut("The catchall executable must be in the same folder as your mothur executable. \n");
		m->mothurOut("If you are a MAC or Linux user you must also have installed mono, a link to mono is on the webpage. \n");
		m->mothurOut("The catchall command parameters are shared, sabund and label.  shared or sabund is required. \n");
		m->mothurOut("The label parameter is used to analyze specific labels in your input.\n");
		m->mothurOut("The catchall command should be in the following format: \n");
		m->mothurOut("catchall(sabund=yourSabundFile) \n");
		m->mothurOut("Example: catchall(sabund=abrecovery.fn.sabund) \n");	
	}
	catch(exception& e) {
		m->errorOut(e, "CatchAllCommand", "help");
		exit(1);
	}
}

/**************************************************************************************/
int CatchAllCommand::execute() {	
	try {
		
		if (abort == true) { return 0; }
		
		//prepare full output directory
		outputDir = m->getFullPathName(outputDir);
		
		//get location of catchall
		GlobalData* globaldata = GlobalData::getInstance();
		path = globaldata->argv;
		path = path.substr(0, (path.find_last_of('m')));
		path = m->getFullPathName(path);

		string catchAllCommandExe = ""; 
		#if defined (__APPLE__) || (__MACH__) || (linux) || (__linux)
			catchAllCommandExe += "mono " + path + "CatchAllcmdL.exe ";
		#else
			catchAllCommandExe += "\"" + path + "CatchAllcmdW.exe\"" + " ";
		#endif
		
		vector<string> inputFileNames;
		if (sharedfile != "") { inputFileNames = parseSharedFile(sharedfile);  globaldata->setFormat("sabund");  }
		else {  inputFileNames.push_back(sabundfile);  }		
		
		for (int p = 0; p < inputFileNames.size(); p++) {
			if (inputFileNames.size() > 1) {
				m->mothurOutEndLine(); m->mothurOut("Processing group " + groups[p]); m->mothurOutEndLine(); m->mothurOutEndLine();
			}
			
			InputData* input = new InputData(inputFileNames[p], "sabund");
			SAbundVector* sabund = input->getSAbundVector();
			string lastLabel = sabund->getLabel();
							
			set<string> processedLabels;
			set<string> userLabels = labels;
			
			string summaryfilename = outputDir + m->getRootName(m->getSimpleName(inputFileNames[p])) + "catchall.summary";
			summaryfilename = m->getFullPathName(summaryfilename);
			
			ofstream out;
			m->openOutputFile(summaryfilename, out);	
			
			out << "label\tmodel\testimate\tlci\tuci" << endl;
			
			//for each label the user selected
			while((sabund != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {

						
				if(allLines == 1 || labels.count(sabund->getLabel()) == 1){
						m->mothurOut(sabund->getLabel());  m->mothurOutEndLine();
						
						//create catchall input file from mothur's inputfile
						string filename = process(sabund, inputFileNames[p]);
						string outputPath = m->getPathName(filename);
					
						//create system command
						string catchAllCommand = "";
						#if defined (__APPLE__) || (__MACH__) || (linux) || (__linux)
							catchAllCommand += catchAllCommandExe + filename + " " + outputPath + " 1";
						#else
							if (outputPath.length() > 0) { outputPath = outputPath.substr(0, outputPath.length()-1); }
							catchAllCommand += catchAllCommandExe + "\"" + filename + "\" \""  + outputPath + "\" 1";
							//wrap entire string in ""
							catchAllCommand = "\"" + catchAllCommand + "\"";
						#endif
										//run catchall
						system(catchAllCommand.c_str());
					
						remove(filename.c_str());
					
						filename = m->getRootName(filename); filename = filename.substr(0, filename.length()-1); //rip off extra .
					
						outputNames.push_back(filename + "_Analysis.csv"); outputTypes["csv"].push_back(filename + "_Analysis.csv");
						outputNames.push_back(filename + "_BestModelsAnalysis.csv"); outputTypes["csv"].push_back(filename + "_BestModelsAnalysis.csv");
						outputNames.push_back(filename + "_BestModelsFits.csv"); outputTypes["csv"].push_back(filename + "_BestModelsFits.csv");
						outputNames.push_back(filename + "_BubblePlot.csv"); outputTypes["csv"].push_back(filename + "_BubblePlot.csv");
					
						createSummaryFile(filename + "_BestModelsAnalysis.csv", sabund->getLabel(), out);
											
						if (m->control_pressed) { out.close(); for (int i = 0; i < outputNames.size(); i++) {remove(outputNames[i].c_str());	}  delete input;  delete sabund;  return 0; }

						processedLabels.insert(sabund->getLabel());
						userLabels.erase(sabund->getLabel());
				}
				
				if ((m->anyLabelsToProcess(sabund->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
						string saveLabel = sabund->getLabel();
						
						delete sabund;		
						sabund = (input->getSAbundVector(lastLabel));
						
						m->mothurOut(sabund->getLabel());  m->mothurOutEndLine();
						

						//create catchall input file from mothur's inputfile
						string filename = process(sabund, inputFileNames[p]);
						string outputPath = m->getPathName(filename);
						
						//create system command
						string catchAllCommand = "";
						#if defined (__APPLE__) || (__MACH__) || (linux) || (__linux)
							catchAllCommand += catchAllCommandExe + filename + " " + outputPath + " 1";
						#else
							if (outputPath.length() > 0) { outputPath = outputPath.substr(0, outputPath.length()-1); }
							catchAllCommand += catchAllCommandExe + "\"" + filename + "\" \""  + outputPath + "\" 1";
							catchAllCommand = "\"" + catchAllCommand + "\"";
						#endif

						//run catchall
						system(catchAllCommand.c_str());
					
						remove(filename.c_str());
					
						filename = m->getRootName(filename); filename = filename.substr(0, filename.length()-1); //rip off extra .
					
						outputNames.push_back(filename + "_Analysis.csv"); outputTypes["csv"].push_back(filename + "_Analysis.csv");
						outputNames.push_back(filename + "_BestModelsAnalysis.csv"); outputTypes["csv"].push_back(filename + "_BestModelsAnalysis.csv");
						outputNames.push_back(filename + "_BestModelsFits.csv"); outputTypes["csv"].push_back(filename + "_BestModelsFits.csv");
						outputNames.push_back(filename + "_BubblePlot.csv"); outputTypes["csv"].push_back(filename + "_BubblePlot.csv");
					
						createSummaryFile(filename + "_BestModelsAnalysis.csv", sabund->getLabel(), out);
					
						if (m->control_pressed) { out.close(); for (int i = 0; i < outputNames.size(); i++) {remove(outputNames[i].c_str());	}  delete input;  delete sabund;  return 0; }

						processedLabels.insert(sabund->getLabel());
						userLabels.erase(sabund->getLabel());
						
						//restore real lastlabel to save below
						sabund->setLabel(saveLabel);
				}
				
				
				lastLabel = sabund->getLabel();	
				
				delete sabund;		
				sabund = (input->getSAbundVector());
			}
			
			//output error messages about any remaining user labels
			set<string>::iterator it;
			bool needToRun = false;
			for (it = userLabels.begin(); it != userLabels.end(); it++) {  
				m->mothurOut("Your file does not include the label " + *it); 
				if (processedLabels.count(lastLabel) != 1) {
					m->mothurOut(". I will use " + lastLabel + ".");  m->mothurOutEndLine();
					needToRun = true;
				}else {
					m->mothurOut(". Please refer to " + lastLabel + ".");  m->mothurOutEndLine();
				}
			}
			
			//run last label if you need to
			if (needToRun == true)  {
				if (sabund != NULL) {	delete sabund;	}
				sabund = (input->getSAbundVector(lastLabel));
				
				m->mothurOut(sabund->getLabel());  m->mothurOutEndLine();
				
				//create catchall input file from mothur's inputfile
				string filename = process(sabund, inputFileNames[p]);
				string outputPath = m->getPathName(filename);
				
				//create system command
				string catchAllCommand = "";
				#if defined (__APPLE__) || (__MACH__) || (linux) || (__linux)
					catchAllCommand += catchAllCommandExe + filename + " " + outputPath + " 1";
				#else
					if (outputPath.length() > 0) { outputPath = outputPath.substr(0, outputPath.length()-1); }
					catchAllCommand += catchAllCommandExe + "\"" + filename + "\" \""  + outputPath + "\" 1";
					catchAllCommand = "\"" + catchAllCommand + "\"";
				#endif
				
				//run catchall
				system(catchAllCommand.c_str());
				
				remove(filename.c_str());
				
				filename = m->getRootName(filename); filename = filename.substr(0, filename.length()-1); //rip off extra .
				
				outputNames.push_back(filename + "_Analysis.csv"); outputTypes["csv"].push_back(filename + "_Analysis.csv");
				outputNames.push_back(filename + "_BestModelsAnalysis.csv"); outputTypes["csv"].push_back(filename + "_BestModelsAnalysis.csv");
				outputNames.push_back(filename + "_BestModelsFits.csv"); outputTypes["csv"].push_back(filename + "_BestModelsFits.csv");
				outputNames.push_back(filename + "_BubblePlot.csv"); outputTypes["csv"].push_back(filename + "_BubblePlot.csv");	
				
				createSummaryFile(filename + "_BestModelsAnalysis.csv", sabund->getLabel(), out);
				
				delete sabund;
			}
			
			out.close();
			delete input; 
			
			if (m->control_pressed) { for (int i = 0; i < outputNames.size(); i++) {remove(outputNames[i].c_str());	} return 0; }
				
		}
		
		if (sharedfile == "") {  
			string summaryfilename = outputDir + m->getRootName(m->getSimpleName(inputFileNames[0])) + "catchall.summary";
			summaryfilename = m->getFullPathName(summaryfilename);
			outputNames.push_back(summaryfilename); outputTypes["summary"].push_back(summaryfilename);
		}else { //combine summaries
			vector<string> sumNames;
			for (int i = 0; i < inputFileNames.size(); i++) {
				sumNames.push_back(m->getFullPathName(outputDir + m->getRootName(m->getSimpleName(inputFileNames[i])) + "catchall.summary"));
			}
			string summaryfilename = combineSummmary(sumNames);
			outputNames.push_back(summaryfilename); outputTypes["summary"].push_back(summaryfilename);
		}
		
		m->mothurOutEndLine();
		m->mothurOut("Output File Names: "); m->mothurOutEndLine();
		for (int i = 0; i < outputNames.size(); i++) {	m->mothurOut(outputNames[i]); m->mothurOutEndLine();	}	
		m->mothurOutEndLine();
		

		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "CatchAllCommand", "execute");
		exit(1);
	}
}
//**********************************************************************************************************************
string CatchAllCommand::process(SAbundVector* sabund, string file1) {
	try {
		string filename = outputDir + m->getRootName(m->getSimpleName(file1)) + sabund->getLabel() + ".csv";
		filename = m->getFullPathName(filename);
	
		ofstream out;
		m->openOutputFile(filename, out);
		
		for (int i = 1; i <= sabund->getMaxRank(); i++) {
			int temp = sabund->get(i);
			
			if (temp != 0) {
				out << i << "," << temp << endl;
			}
		}
		out.close();
		
		return filename;
	
	}
	catch(exception& e) {
		m->errorOut(e, "CatchAllCommand", "process");
		exit(1);
	}
}
//*********************************************************************************************************************
string CatchAllCommand::combineSummmary(vector<string>& outputNames) {
	try {
		
		ofstream out;
		string combineFileName = outputDir + m->getRootName(m->getSimpleName(sharedfile)) + "catchall.summary";
		
		//open combined file
		m->openOutputFile(combineFileName, out);
		
		out << "label\tgroup\tmodel\testimate\tlci\tuci" << endl;
		
		//open each groups summary file
		string newLabel = "";
		int numLines = 0;
		map<string, vector<string> > files;
		for (int i=0; i<outputNames.size(); i++) {
			vector<string> thisFilesLines;
			
			ifstream temp;
			m->openInputFile(outputNames[i], temp);
			
			//read through first line - labels
			m->getline(temp);			
			m->gobble(temp);
			
			//for each label
			while (!temp.eof()) {
				
				string thisLine = "";
				string tempLabel;
				
				for (int j = 0; j < 5; j++) {  
					temp >> tempLabel; 
					
					//save for later
					if (j == 1) { thisLine += groups[i] + "\t" + tempLabel + "\t";	}
					else{  thisLine += tempLabel + "\t";	}
				}
				
				thisLine += "\n";
				
				thisFilesLines.push_back(thisLine);
				
				m->gobble(temp);
			}
			
			files[outputNames[i]] = thisFilesLines;
			
			numLines = thisFilesLines.size();
			
			temp.close();
			remove(outputNames[i].c_str());
		}
		
		//for each label
		for (int k = 0; k < numLines; k++) {
			
			//grab summary data for each group
			for (int i=0; i<outputNames.size(); i++) {
				out << files[outputNames[i]][k];
			}
		}	
		
		
		out.close();
		
		//return combine file name
		return combineFileName;
		
	}
	catch(exception& e) {
		m->errorOut(e, "CatchAllCommand", "combineSummmary");
		exit(1);
	}
}
//**********************************************************************************************************************
int CatchAllCommand::createSummaryFile(string file1, string label, ofstream& out) {
	try {
		
		ifstream in;
		int able = m->openInputFile(file1, in, "noerror");
		
		if (able == 1) {  m->mothurOut("[ERROR]: the catchall program did not run properly. Please check to make sure it is located in the same folder as your mothur executable.");m->mothurOutEndLine();  m->control_pressed = true; return 0; }
			
		if (!in.eof()) {
			
			string header = m->getline(in); m->gobble(in);
			
			int pos = header.find("Total Number of Observed Species =");
			string numString = "";
			
			
			if (pos == string::npos) { m->mothurOut("[ERROR]: cannot parse " + file1); m->mothurOutEndLine(); }
			else {
				//pos will be the position of the T in total, so we want to count to the position of =
				pos += 34;
				char c=header[pos];
				while (c != ','){
					if (c != ' ') {
						numString += c;
					}
					pos++;
					c=header[pos];
					
					//sanity check
					if (pos > header.length()) { m->mothurOut("Cannot find number of OTUs in " + file1); m->mothurOutEndLine(); in.close(); return 0; }
				}
			}
															  
			string firstline = m->getline(in); m->gobble(in);
			vector<string> values;
			m->splitAtComma(firstline, values);
			
			values.pop_back(); //last value is always a blank string since the last character in the line is always a ','
			
			if (values.size() == 1) { //grab next line if firstline didn't have what you wanted
				string secondline = m->getline(in); m->gobble(in);
				values.clear();
				m->splitAtComma(secondline, values);
				
				values.pop_back(); //last value is always a blank string since the last character in the line is always a ','
			}
			
			if (values.size() == 1) { //still not what we wanted fill values with numOTUs
				values.resize(8, "");
				values[1] = "Sobs";
				values[4] = numString;
				values[6] = numString;
				values[7] = numString;
			}
			
			if (values.size() < 8) { values.resize(8, ""); }
			
			out << label << '\t' << values[1] << '\t' << values[4] << '\t' << values[6] << '\t' << values[7] << endl;
		}
		
		in.close();
		
		return 0;
		
	}
	catch(exception& e) {
		m->errorOut(e, "CatchAllCommand", "createSummaryFile");
		exit(1);
	}
}
//**********************************************************************************************************************
vector<string> CatchAllCommand::parseSharedFile(string filename) {
	try {
		vector<string> filenames;
		
		//read first line
		InputData* input = new InputData(filename, "sharedfile");
		vector<SharedRAbundVector*> lookup = input->getSharedRAbundVectors();
		
		string sharedFileRoot = outputDir + m->getRootName(m->getSimpleName(filename));
		
		//clears file before we start to write to it below
		for (int i=0; i<lookup.size(); i++) {
			remove((sharedFileRoot + lookup[i]->getGroup() + ".sabund").c_str());
			filenames.push_back((sharedFileRoot + lookup[i]->getGroup() + ".sabund"));
			groups.push_back(lookup[i]->getGroup());
		}
		
		while(lookup[0] != NULL) {
			
			for (int i = 0; i < lookup.size(); i++) {
				SAbundVector sav = lookup[i]->getSAbundVector();
				ofstream out;
				m->openOutputFileAppend(sharedFileRoot + lookup[i]->getGroup() + ".sabund", out);
				sav.print(out);
				out.close();
			}
			
			for (int i = 0; i < lookup.size(); i++) {  delete lookup[i];  } 
			lookup = input->getSharedRAbundVectors();
		}
		
		delete input;
		
		return filenames;
	}
	catch(exception& e) {
		m->errorOut(e, "CatchAllCommand", "parseSharedFile");
		exit(1);
	}
}
/**************************************************************************************/
