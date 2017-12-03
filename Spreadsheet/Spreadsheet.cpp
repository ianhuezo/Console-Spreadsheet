#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <regex>

using namespace std;

//defining some functions that get used for recursion
int getDivision(char * &cstring);
int getMultiply(char * &cstring);
int getSubtract(char * &cstring);
int getAdd(char * &cstring);



vector<string> getInput(ifstream &input) {
	string row;
	vector<string> rowVector;
	if (input.is_open()) {
		while (getline(input, row)) {
			rowVector.push_back(row);
		}
input.close();
	}
	return rowVector;
}

void showInput(vector<string> rowVector) {
	cout << "Input: \n";
	for (unsigned int i = 0; i < rowVector.size(); i++) {
		cout << rowVector[i] << '\n';
	}
}

vector<vector<string>> toMatrix(vector<string> rowVector) {//convert each row to a 2d vector with more than 1 column
	string argumentString = "";
	vector <string> column;
	vector <vector <string> > gridMatrix;

	for (unsigned int i = 0; i < rowVector.size(); i++) {
		for (unsigned int j = 0; j < rowVector[i].size(); j++) {
			//case statements decided upon with tabs
			if (rowVector[i][j] != '\t') {
				argumentString += rowVector[i][j];
				if (rowVector[i][j + 1] == '\t' || j == rowVector[i].size() - 1) {
					column.push_back(argumentString);
					argumentString = "";
				}
			}
			if (rowVector[i][j] == '\t') {
				if (j != 0 && (rowVector[i][j + 1] == '\t' || rowVector[i][j - 1] == '\t')) {
					column.push_back("empty");
				}
				if (rowVector[i][0] == '\t' && j == 0) {
					column.push_back("empty");
				}
			}
		}
		gridMatrix.push_back(column);//push the vector to the 2d matrix
		column.clear();//clear the column for more inputs
	}
	return gridMatrix;
}

vector<int>getCoordinates(string variable) {//get the coordinates of the variables like A1 B1, etc
	vector<int> coordinateSystem;
	int coordinate;
	string number = "";
	for (unsigned int i = 0; i < variable.size(); i++) {
		if (i == 0) {
			coordinate = variable[i] - 65;
			coordinateSystem.push_back(coordinate);
		}
		else {
			number += variable[i];
			if (i == variable.size() - 1) {
				coordinate = stoi(number) - 1;
				coordinateSystem.push_back(coordinate);
			}
		}
	}
	return coordinateSystem;
}

string replaceVariables(vector<vector<string>> gridMatrix, string equation) {//Replaces the variable A1,B1, etc. with 1*2, 2+4, etc.
	string expression = equation.substr(1, equation.size());//expression that will be parsed every time for a new A1, B1, etc
	string replacedString = equation.substr(1, equation.size());//expression that will replace variable with a number
	regex e("([A-Z]\\d+)");//find the variables with a regular expression
	smatch m;//object for each variable
	string gridNumber;
	vector<int> coordinates;
	string currentVariable;
	while (regex_search(expression, m, e)) {
		currentVariable = m[0];
		coordinates = getCoordinates(currentVariable);
		gridNumber = gridMatrix[coordinates[1]][coordinates[0]];//get the value of the grid number
		regex replaceParameter("(" + currentVariable + ")");
		replacedString = regex_replace(replacedString, replaceParameter, gridNumber);
		replacedString = replacedString;
		expression = m.suffix();
	}
	return "(" + replacedString + ")";
}

vector < vector<string>> toAllNumberMatrix(vector<vector<string>> gridMatrix) {//replace all the variables to numbers and put into matrix
	string replacedVariable;
	for (unsigned int k = 0; k < gridMatrix.size(); k++) {
		for (unsigned int l = 0; l < gridMatrix[k].size(); l++) {
			if (gridMatrix[k][l].at(0) == '=') {
				replacedVariable = replaceVariables(gridMatrix, gridMatrix[k][l]);
				gridMatrix[k][l] = replacedVariable;
			}
		}
	}
	return gridMatrix;
}

//the following recursion int functions parse the string if there are equations
int makeNumber(char * &cstring) {
	if (*cstring == '(') {
		++cstring;
		int evaluateAgain = getAdd(cstring);
		++cstring;
		return evaluateAgain;
	}
	else if (*cstring == ')') {
		++cstring;
		int endNum = getAdd(cstring);
		++cstring;
		return endNum;
	}
	else {
		int num = 0;
		while (*cstring >= '0' && *cstring <= '9') {
			num = num * 10;
			num = num + *cstring - '0';
			++cstring;
		}
		return num;
	}
}

int getDivision(char * &cstring) {//division goes first in operations
	int dnum1 = makeNumber(cstring);
	while (*cstring == '/') {
		cstring++;//add first to look at next value
		int dnum2 = makeNumber(cstring);
		dnum1 = dnum1 / dnum2;
	}
	return dnum1;
}

int getMultiply(char * &cstring) {//multiplication second
	int num1 = getDivision(cstring);
	int num2;
	while (*cstring == '*') {
		++cstring;
		num2 = getDivision(cstring);//get num after
		num1 *= num2;
	}
	return num1;
}

int getSubtract(char * &cstring) {//Subtract Third
	int num1 = getMultiply(cstring);
	int num2;
	while (*cstring == '-') {
		++cstring;
		num2 = getMultiply(cstring);//get num after
		num1 -= num2;
	}
	return num1;
}

int getAdd(char * &cstring) {//Add last
	int num1 = getSubtract(cstring);
	int num2;
	while (*cstring == '+') {
		++cstring;
		num2 = getSubtract(cstring);//get num after 
		num1 += num2;
	}
	return num1;
}

string solveEquation(string equation) {//get the string and send through recursion 
	char* cstring = new char[equation.length() + 1];
	strcpy(cstring, equation.c_str());	
	return to_string(getAdd(cstring));
}

void mapMatrixPositions(vector<vector<string>> gridMatrix) {
	char characterPosition;
	int getCharacterNumber;
	int numberPosition;
	vector<vector<string>> numberMatrix = toAllNumberMatrix(gridMatrix);
	vector<string> rowEndMatrix;
	vector<vector<string>> endMatrix;
	for (unsigned int k = 0; k < numberMatrix.size(); k++) {//ypos
		for (unsigned int l = 0; l < numberMatrix[k].size(); l++) {//xpos
			if (numberMatrix[k][l] == "empty") {
				numberMatrix[k][l] = "";
			}
			else if (numberMatrix[k][l].find("=") != string::npos) {//if there is a reference to another cell that isn't known yet
				numberMatrix[k][l] = "#Error";
			}
			else if ((numberMatrix[k][l].find("empty") != string::npos) && numberMatrix[k][l].size() > 5) {//if empty is found and something else with it
				numberMatrix[k][l] = "#NAN";
			}
			else {
				numberMatrix[k][l] = solveEquation(numberMatrix[k][l]);//parse all the strings and convert them into a number
			}
			cout << numberMatrix[k][l] << "\t";
		}
		cout << endl;
	}
}


void showOutput(vector<string> rowVector) {//show the output of the matrix
	vector <vector <string> > gridMatrix;
	
	cout << "Output: \n";
	gridMatrix = toMatrix(rowVector);
	mapMatrixPositions(gridMatrix);

}


int main() {
	ifstream inputFile("input.txt");//get the file to read from
	vector<string> rowVector = getInput(inputFile);//store the file in a vector
	string equation;
	if (!rowVector.size() == 0) {//if there is data proceed
		showInput(rowVector);
		showOutput(rowVector);
		cout << endl;
		cout << equation << endl;
	}
	else {
		cout << "No data was found." << endl;
	}
	system("PAUSE");
	return 0;
}


