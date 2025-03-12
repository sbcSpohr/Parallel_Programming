#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <functional>

#include <mpi.h>

using namespace std;

vector<string> split_phrase(string &text) {

    vector<string> split;

    stringstream ss(text);
    string word;

    while(ss >> word) {
        split.push_back(word);
    }
    return split;
}


int main(int argc, char **argv) {

    MPI_Init(&argc, &argv);
    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 7) {
        if (rank == 0) {
            cerr << "ERRO: O número de processos deve ser exatamente 7" << endl;
        }
        MPI_Finalize();
        return 1;
    }


    if(rank == 0) {

        ifstream file(argv[1]);
        if(!file) {
            cerr << "ERRO" << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        string phrase;
        int index = 1;
        while(getline(file, phrase)) {
            int chunk_size = phrase.size();
            MPI_Send(&chunk_size, 1, MPI_INT, index, 0, MPI_COMM_WORLD);
            MPI_Send(phrase.data(), phrase.size(), MPI_CHAR, index, 0, MPI_COMM_WORLD);

            index++;
            if(index == size - 3) {
                index = 1;
            }
        }

        int stop_signal = -1;
        for(int i = 1; i < size - 3; i++) {
            MPI_Send(&stop_signal, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }

    if(rank > 0 && rank < 4) {

        while(true) {

            int recv_size;
            MPI_Recv(&recv_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if(recv_size == -1) {
                break;
            }

            vector<char> recv_phrase(recv_size);
            MPI_Recv(recv_phrase.data(), recv_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            string phrase(recv_phrase.begin(), recv_phrase.end());

            vector<string> words = split_phrase(phrase);

            hash<string> hasher;

            for(string &word : words) {
                size_t hash_value = hasher(word);
                size_t hash_value_m = hash_value % 2;

                if(hash_value_m == 0) {
                    hash_value_m = 4;
                } else {
                    hash_value_m = 5;
                }
                //cout << hash_value_m << endl;
                int chunk_size = word.size();
                MPI_Send(&chunk_size, 1, MPI_INT, hash_value_m, 0, MPI_COMM_WORLD);
                MPI_Send(word.data(), word.size(), MPI_CHAR, hash_value_m, 0, MPI_COMM_WORLD);
            }
        }

        int stop_signal = -1;
        MPI_Send(&stop_signal, 1, MPI_INT, 4, 0, MPI_COMM_WORLD);
        MPI_Send(&stop_signal, 1, MPI_INT, 5, 0, MPI_COMM_WORLD);
    }

    if (rank > 3 && rank < 6) {

        unordered_map<string, int> local_count;

        int stop = 0;

        while (stop < 3) {
            MPI_Status status;

            int recv_size;
            MPI_Recv(&recv_size, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
    
            if (recv_size == -1) {
                stop++;
                continue;
            }
    
            vector<char> word_recv(recv_size);
            MPI_Recv(word_recv.data(), recv_size, MPI_CHAR, status.MPI_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    
            string word(word_recv.begin(), word_recv.end());

            local_count[word]++;
        }

        int word_count = local_count.size();

        MPI_Send(&word_count, 1, MPI_INT, 6, 0, MPI_COMM_WORLD);

        for (auto &var : local_count) {
            int size_word = var.first.size();
            MPI_Send(&size_word, 1, MPI_INT, 6, 0, MPI_COMM_WORLD);
            MPI_Send(var.first.data(), size_word, MPI_CHAR, 6, 0, MPI_COMM_WORLD);
            MPI_Send(&var.second, 1, MPI_INT, 6, 0, MPI_COMM_WORLD);
        }
    }

    if(rank == 6) {

        unordered_map<string, int> final_count;

        for(int i = 4; i <= 5; i++) {

            int word_count;
            MPI_Recv(&word_count, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 

            for(int j = 0; j < word_count; j++) {
                int size_word;
                MPI_Recv(&size_word, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                vector<char> word_recv(size_word);
                MPI_Recv(word_recv.data(), size_word, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                string word(word_recv.begin(), word_recv.end());

                int count;
                MPI_Recv(&count, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                final_count[word] += count;
            }
        }

        cout << "======= RESULTADO =======" << endl;
        for (auto &word : final_count) {
            cout << "Palavra: " << word.first << " (" << word.second << ")" << endl;
        }   
    }
    
    MPI_Finalize();
    return 0;
}