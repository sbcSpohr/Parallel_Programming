#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>

#include <mpi.h>

using namespace std;

unordered_map<string, int> count_words(string &text) {

    unordered_map<string, int> count;

    stringstream ss(text);
    string word;

    while(getline(ss, word, ' ')) {
        count[word]++;
    }
    
    return count;
}

void merging (unordered_map<string, int>&global_map, unordered_map<string, int>&local_map) {

    for(auto &word : local_map) {
        global_map[word.first] += word.second;
    }
}

int main(int argc, char **argv) {


    MPI_Init(&argc, &argv);
    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int total_size;
    string full_text;

    if(rank == 0) {
        ifstream file(argv[1]);

        if(!file) {
            cerr << "Erro abrir arquivo" << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        stringstream store_text;

        store_text << file.rdbuf();
        full_text = store_text.str();

        file.close();

        total_size = full_text.size();
        int chunk_size = total_size / (size - 2);

        int index = 0;
        while(index < full_text.size()) {
            for(int i = 2; i < size; i++) {
               int start = index;
               int end = start + chunk_size;

               if (end >= total_size) {end = total_size;}

               while(end < total_size && full_text[end] != ' ') {end++;}

               string chunk = full_text.substr(start, end - start);

               int chunk_length = chunk.size();

               MPI_Send(&chunk_length, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
               MPI_Send(chunk.data(), chunk_length, MPI_CHAR, i, 0, MPI_COMM_WORLD);

               index = end;
            }
        }
    }

        MPI_Bcast(&total_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
        full_text.resize(total_size);          
        MPI_Bcast(&full_text[0], total_size, MPI_CHAR, 0, MPI_COMM_WORLD);

        if(rank > 1) {

            int start = (rank == 2) ? 0 : (total_size / size) * rank;
            int end = (rank == size - 1) ? total_size : (total_size / size) * (rank + 1);

            while(start > 0 && full_text[start] != ' ') { start++;}
            while(end < total_size && full_text[end] != ' ') { end++;}

            string local_text = full_text.substr(start, end - start);

            unordered_map<string, int> local_count = count_words(local_text);

            int send_size = local_text.size();
            MPI_Send(&send_size, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
            MPI_Send(local_text.data(), send_size, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
        }

        if(rank == 1) {

            unordered_map<string, int> global_word_count;

            for(int i = 2; i < size; i++) {
                int recv_size;
                MPI_Recv(&recv_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                vector<char> recv_words(recv_size);
                MPI_Recv(recv_words.data(), recv_size, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                string word_recieved(recv_words.begin(), recv_words.end());
                unordered_map<string, int> received_map = count_words(word_recieved);
                merging(global_word_count, received_map);
            }

            for(auto &word : global_word_count) {
                cout << word.first << ": " << word.second << endl;
            }
        }

            MPI_Finalize();

            return 0;
        }
