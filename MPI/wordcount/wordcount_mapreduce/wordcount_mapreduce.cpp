#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <functional>

#include <mpi.h>

using namespace std;


vector<pair<string, int>> count_words(string &text) {

    vector<pair<string, int>> count;

    stringstream ss(text);
    string word;
    while(ss >> word) {
        count.push_back({word, 1});
    }
    return count;
}

int main(int argc, char **argv) {

    MPI_Init(&argc, &argv);
    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    vector<pair<string, int>> local_count;

    if(rank == 0) {

        ifstream file(argv[1]);
        if(!file) {
            cerr << "ERROR open file" << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        string word;
        int index = 2;
        while(getline(file, word)) {
            int chunk_size = word.size();
            MPI_Send(&chunk_size, 1, MPI_INT, index, 0, MPI_COMM_WORLD);
            MPI_Send(word.data(), word.size(), MPI_CHAR, index, 0, MPI_COMM_WORLD);
            index = (index == size - 1) ? 2 : index + 1;    
        }

        for(int i = 2; i < size; i++) {
            int stop_signal = -1;
            MPI_Send(&stop_signal, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }

    if(rank > 1) {

        while(true) {
            
            int chunk_size;
            MPI_Recv(&chunk_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if(chunk_size == -1) {
                break;
            }

            vector<char> word_recv(chunk_size);

            MPI_Recv(word_recv.data(), chunk_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            string phrase_text(word_recv.begin(), word_recv.end());

            local_count = count_words(phrase_text);
        }

            for(auto &word : local_count) { 
                hash<string> hasher;
                size_t hash_value = hasher(word.first);
                size_t hash_value_m = 2 + (hash_value % (size - 2));
            
                cout << "hash value modulo: " << hash_value_m << endl;

                int word_size = word.first.size();
                MPI_Send(&word_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
                MPI_Send(word.first.data(), word.first.size(), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
                
            }

        
            int stop_signal = -1;
            MPI_Send(&stop_signal, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    if(rank == 0) {

        while (true) {
            int recv_size;
            MPI_Recv(&recv_size, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if(recv_size == -1) {
                break;
            }

            vector<char> word_recvf(recv_size);
            MPI_Recv(word_recvf.data(), recv_size, MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }


    MPI_Finalize();

    return 0;
}