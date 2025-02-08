#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>

#include <mpi.h>

using namespace std;

unordered_map<string, int> count(string &texto) {

    unordered_map<string, int> word_count;
    string word;
    stringstream texto(texto);

    while(getline(texto, word, ' ')) { //le do arquivo texto e armazena em word delimitando por ' '
        word_count[word]++;
    }
    return word_count;
}

int main(int argc, char **argv[]) {

    MPI_Init(&argc, &argv);

    int rank, size_mpi;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size_mpi);

    vector<string> lines;
    int total_lines = 0;

    if(rank == 0) {
        ifstream file(argv[1]);
        if(!file) {
            cout << "ERRO" << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        string line;
        while(getline(file, line)) { //le uma linha do arquivo file e armazena em line
            lines.push_back(line);
        }
        file.close()
        total_lines = lines.size();
            
        }

        MPI_Bcast(&total_lines, 1, MPI_INT, 0, MPI_COMM_WORLD);

        int process_lines = (rank < total_lines % size_mpi) ? (total_lines / size_mpi + 1) : (total_lines / size_mpi);


    }
