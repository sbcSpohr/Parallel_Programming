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

//fazer o unordered_map


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

            index++;
            if(index == size - 1) {
                index = 2;
            }
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
            
            for(auto &word : local_count) { 

                int word_size = word.first.size();
                MPI_Send(&word_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
                MPI_Send(word.first.data(), word.first.size(), MPI_CHAR, 0, 0, MPI_COMM_WORLD);  
            }
        }

            int stop_signal = -1;
            MPI_Send(&stop_signal, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    if(rank == 0) {
        
        int workers_finish = 1;
    
        while (workers_finish < size - 1) {

            MPI_Status status;
            int recv_size;
    
            MPI_Recv(&recv_size, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
    
            if (recv_size == -1) { 
                workers_finish++;
            } else {
                vector<char> word_recvf(recv_size);

                MPI_Recv(word_recvf.data(), recv_size, MPI_CHAR, status.MPI_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    
                string received_word(word_recvf.begin(), word_recvf.end());  

                hash<string> hasher;
                size_t hash_value = hasher(received_word);
                size_t hash_value_m = 2 + (hash_value % (size - 2));
                //cout << received_word << " " << "hash value modulo: " << hash_value_m << endl;

                int size_word = received_word.size();
                MPI_Send(&size_word, 1, MPI_INT, hash_value_m, 0, MPI_COMM_WORLD);
                MPI_Send(received_word.data(), received_word.size(), MPI_CHAR, hash_value_m, 0, MPI_COMM_WORLD);
            }                

        }

        int stop_signal = -1;
        for(int i = 2; i < size; i++) {
            MPI_Send(&stop_signal, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }

    if(rank > 1) {

        unordered_map<string, int> accumulate;

        while(true) {

            int size_word;
            MPI_Recv(&size_word, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if(size_word == -1) {
                break;
            }
            
            vector<char> received_wordd(size_word);
            MPI_Recv(received_wordd.data(), size_word, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            string word(received_wordd.begin(), received_wordd.end());

            //cout << rank << " recebeu a palavra ( " << word << " )" << endl;

            accumulate[word]++;

            int sizee_word = word.size();
            MPI_Send(&sizee_word, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
            MPI_Send(word.data(), word.size(), MPI_CHAR, 1, 0, MPI_COMM_WORLD);
        }

        // for(auto &word : accumulate) {
        //     cout << "Rank: " << rank << " - " << word.first << "(" << word.second << ") " << endl;
        // }

        int stop_signal = -1;
        MPI_Send(&stop_signal, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    }

    if(rank == 1) {

        unordered_map<string, int> final_map;

        int workers_finish = 0;

        while(workers_finish < size - 2) {

            MPI_Status status;
            int word_size;
            MPI_Recv(&word_size, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

            if(word_size == -1) {
                workers_finish++;
            } else {
                vector<char>word_recv(word_size);
                MPI_Recv(word_recv.data(), word_size, MPI_CHAR, status.MPI_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    
                string final_word(word_recv.begin(), word_recv.end());
    
                final_map[final_word]++;
            }
        }

        for(auto &word : final_map) {
            cout << "Palavra: " << word.first << "(" << word.second << ") " << endl;
        }

    }

    MPI_Finalize();

    return 0;
}