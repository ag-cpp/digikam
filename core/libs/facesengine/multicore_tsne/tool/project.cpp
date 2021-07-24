#include <utility>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include "splittree.h"
#include "tsne.h"


/**
 * @brief loadData: load data from csv file into a train dataset and a leftout dataset to simulate unknown label
 */ 
std::pair<std::vector<int>, std::vector<std::vector<float>>> loadData(const std::string& fileName) 
{
    std::vector<std::vector<float>> predictors;
    std::vector<int> labels;

    std::ifstream file(fileName);
    if(!file.is_open()) {
        std::cout << "Fail to open file" << std::endl;
        return std::make_pair(labels, predictors);
    }

    std::string line;
    while(std::getline(file, line))
    {
        // Create a stringstream of the current line
        std::istringstream stream(line);
        std::string data;
        // Keep track of the current column index
        getline(stream, data, ',');
        labels.push_back(std::stoi(data));
        std::vector<float> predictor;
        while (getline(stream, data, ','))
        {  
            predictor.push_back(std::stof(data));
        }

        predictors.push_back(predictor);
    }

    file.close();

    return std::make_pair(labels, predictors);
}


void save(const std::pair<std::vector<int>, std::vector<std::vector<float>>>& data, const std::string& fileName) {
    std::ofstream myfile;
    myfile.open(fileName);

    for (size_t i = 0; i < data.first.size(); ++i) 
    {
        myfile << data.first[i] << ",";

        for (size_t j = 0; j < data.second[i].size(); ++j) 
        {
            myfile << data.second[i][j];
            if(j != data.second[i].size() - 1) 
            {
                myfile << ","; 
            }
        }

        myfile << "\n";
    }

    myfile.close();
}

double* flatten(const std::vector<std::vector<float>>& data) {
    double* array = new double[data.size() * data[0].size()];

    for (size_t i = 0; i < data.size(); ++i) 
    {
        for (size_t j = 0; j < data[i].size(); ++j) 
        {
            array[i*data[0].size()+j] = data[i][j];
        }
    }

    return array;
}


std::vector<std::vector<float>> reshape(double* data, size_t ncols, size_t nrows)
{
    std::vector<std::vector<float>> mat(nrows, std::vector<float>(ncols));

    for (size_t i = 0; i < nrows; ++i) 
    {
        for (size_t j = 0; j < ncols; ++j) 
        {
            mat[i][j] = data[i*ncols+j];
        }
    }

    return mat;
}

int main() 
{
    std::pair<std::vector<int>, std::vector<std::vector<float>>> data = loadData("facenet_data.txt");
    double* flattenData = flatten(data.second);
    double* Y = new double[data.second.size() * 2];

    TSNE::tsne_run_double(flattenData, data.second.size(), data.second[0].size(), Y, 2, 30, 0.5, 4);
    data.second = reshape(Y, 2, data.second.size());

    save(data, "facenet_tsne_data.txt");
    delete flattenData;
    delete Y;
}