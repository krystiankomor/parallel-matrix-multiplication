#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>
#include <omp.h>
#include <chrono>

using namespace std;
using chrono::high_resolution_clock;
using chrono::duration_cast;
using chrono::milliseconds;
using chrono::time_point;

bool canBeMultiplied(vector<vector<double>> const &matrixA,
                     vector<vector<double>> const &matrixB);
int getNoOfRows(vector<vector<double>> const &matrix);
int getNoOfCols(vector<vector<double>> const &matrix);
int getMilisecondsBetweenTwoTimePoints(time_point<high_resolution_clock> t1, time_point<high_resolution_clock> t2);
vector<string> explode(string const &s, char delim);
vector<vector<double>> getMatrixFromFile(string const &filename);
vector<vector<double>> multiplyMatrixes(vector<vector<double>> const &matrixA, vector<vector<double>> const &matrixB, bool useSingleThread);

int main() {
  vector<vector<double>> matrixA = getMatrixFromFile("a.csv");
  vector<vector<double>> matrixB = getMatrixFromFile("b.csv");

  if (canBeMultiplied(matrixA, matrixB)) {
    time_point<high_resolution_clock> startTimeParallel = high_resolution_clock::now();
    vector<vector<double>> resultMatrixParallel = multiplyMatrixes(matrixA, matrixB, false);
    time_point<high_resolution_clock> endTimeParallel = high_resolution_clock::now();

    int parallelTime = getMilisecondsBetweenTwoTimePoints(startTimeParallel, endTimeParallel);

    time_point<high_resolution_clock> startTimeSingleThread = high_resolution_clock::now();
    vector<vector<double>> resultMatrixSingleThread = multiplyMatrixes(matrixA, matrixB, false);
    time_point<high_resolution_clock> endTimeSingleThread = high_resolution_clock::now();

    int singleThreadTime = getMilisecondsBetweenTwoTimePoints(startTimeSingleThread, endTimeSingleThread);

    string resultFileName;
    resultFileName.append("C_").append(to_string(parallelTime)).append("_").append(to_string(singleThreadTime)).append(".csv");

    ofstream resultFile(resultFileName);

    resultFile << getNoOfRows(resultMatrixParallel) << endl;
    resultFile << getNoOfCols(resultMatrixParallel) << endl;

    for(vector<double> row : resultMatrixParallel) {
      for(double col : row) {
        resultFile << col << ";";
      }

      resultFile << endl;
    }

    resultFile.close();

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

int getNoOfRows(vector<vector<double>> const &matrix) { 
  return matrix.size(); 
}

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

vector<vector<double>> multiplyMatrixes(vector<vector<double>> const &matrixA, vector<vector<double>> const &matrixB, bool useSingleThread) {
  vector<vector<double>> resultMatrix;
  const int rowsNumber = getNoOfRows(matrixA);
  const int colsNumber = getNoOfCols(matrixA);

  double **result = new double*[rowsNumber];
  for(int i = 0; i < rowsNumber; ++i) {
      result[i] = new double[rowsNumber];
  }

  int noOfThreadsFirstClause = useSingleThread ? 1 : rowsNumber;
  int NoOfThreadsSecondClause = useSingleThread ? 1 : colsNumber;

  /**
   * Collapse dla lepszego rozkładu użycia wątków procesora
  */
  #pragma omp parallel for collapse(2) num_threads(noOfThreadsFirstClause)
  for (int row = 0; row < rowsNumber; row++) {
    for (int col = 0; col < rowsNumber; col++) {
      double sum = 0.0;

      /**
       * Pozwala na zrównoleglenie w zwrównoleglonym wątku. Poprawia wydajność
       * poprzez lepsze wykorzystanie mocy obliczeniowej i nieużywanych wątków procesora.
      */
      #pragma omp parallel for reduction(+:sum) num_threads(NoOfThreadsSecondClause) 
      for (int i = 0; i < colsNumber; i++) {
        sum += matrixA[row][i] * matrixB[i][col];
      }

      result[row][col] = sum;
    }
  }

  for(int row = 0; row < rowsNumber; row++) {
    vector<double> rowMatrix;

    for(int col = 0; col < rowsNumber; col++) {
      rowMatrix.push_back(result[row][col]);
    }

    resultMatrix.push_back(rowMatrix);
  }

  for(int i = 0; i < rowsNumber; ++i) {
      delete [] result[i];
  }

  delete [] result;

  return resultMatrix;
}

int getMilisecondsBetweenTwoTimePoints(time_point<high_resolution_clock> t1, time_point<high_resolution_clock> t2) {
    auto duration = duration_cast<milliseconds>(t2 - t1);

    return duration.count();
}
