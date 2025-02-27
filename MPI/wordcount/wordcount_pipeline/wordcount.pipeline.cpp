#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <functional>

#include <mpi.h>

using namespace std;

unordered_map<string, int> count_words(string &text) {

    unordered_map<string, int> count;
    stringstream ss(text);
    string word;
    while(ss >> word) {
        count[word]++;
    }
    return count;
}
