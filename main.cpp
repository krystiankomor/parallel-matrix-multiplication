#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>

using namespace std;

bool canBeMultiplied(vector<vector<double>> const &matrixA,
                     vector<vector<double>> const &matrixB);
int getNoOfRows(vector<vector<double>> const &matrix);
int getNoOfCols(vector<vector<double>> const &matrix);
vector<string> explode(string const &s, char delim);
vector<vector<double>> getMatrixFromFile(string const &filename);

int main() {
  string text;
  vector<vector<double>> matrixA = getMatrixFromFile("a.csv");
  vector<vector<double>> matrixB = getMatrixFromFile("b.csv");

  if (canBeMultiplied(matrixA, matrixB)) {
    vector<vector<double>> resultMatrix;
    int rowsNumber = getNoOfRows(matrixA);
    int colsNumber = getNoOfCols(matrixA);

    for (int row = 0; row < rowsNumber; row++) {
      vector<double> rowMatrix;

      for (int col = 0; col < rowsNumber; col++) {
        double sum = 0.0;

        for (int i = 0; i < colsNumber; i++) {
          sum += matrixA[row][i] * matrixB[i][col];
        }

        rowMatrix.push_back(sum);
      }

      resultMatrix.push_back(rowMatrix);
    }

  } else {
    cout << "Multiplication cannot be performed." << endl;
  }

  return 0;
}

vector<vector<double>> getMatrixFromFile(string const &filename) {
  string line;
  ifstream file(filename);
  vector<vector<double>> matrix;
  int noOfCols = 0;

  getline(file, line); // rows, just ignore

  getline(file, line);
  noOfCols = stoi(line);

  while (getline(file, line)) {
    vector<double> row;
    vector<string> currentRowValues = explode(line, ';');

    for (int i = 0; i < noOfCols; i++) {
      row.push_back(stod(currentRowValues[i]));
    }

    matrix.push_back(row);
  }

  file.close();

  return matrix;
}

bool canBeMultiplied(vector<vector<double>> const &matrixA,
                     vector<vector<double>> const &matrixB) {
  int matrixANoOfCols = getNoOfCols(matrixA);

  if (matrixANoOfCols == 0)
    return false;

  int matrixBNoOfRows = getNoOfRows(matrixB);

  if (matrixBNoOfRows == 0)
    return false;

  if (getNoOfCols(matrixB) == 0)
    return false;

  if (matrixANoOfCols != matrixBNoOfRows)
    return false;

  return true;
}

int getNoOfRows(vector<vector<double>> const &matrix) { return matrix.size(); }

int getNoOfCols(vector<vector<double>> const &matrix) {
  if (getNoOfRows(matrix) == 0)
    return 0;

  return matrix[0].size();
}

vector<string> explode(string const &s, char delim) {
  vector<string> result;
  istringstream iss(s);

  for (string token; getline(iss, token, delim);) {
    result.push_back(move(token));
  }

  return result;
}
