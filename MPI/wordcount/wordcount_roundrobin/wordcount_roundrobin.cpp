#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <fstream>

#include <cereal/types/unordered_map.hpp>
#include <cereal/types/string.hpp>
#include <cereal/archives/binary.hpp>

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

void merging(unordered_map<string, int> &map1, unordered_map<string, int> &map2) {
    for(auto &word : map2) {
        map1[word.first] += word.second;
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
            cerr << "ERRO abrir arquivo" << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        int index = 2;

        string tword;
        while(getline(file, tword)) {
            int chunk_length = tword.size();
            MPI_Send(&chunk_length, 1, MPI_INT, index, 0, MPI_COMM_WORLD);
            MPI_Send(tword.data(), tword.size(), MPI_CHAR, index, 0, MPI_COMM_WORLD);
            index++;
            index = (index == size - 1) ? 2 : index + 1;
        }
        file.close();

        int stop = -1;
        for(int i = 2; i < size; i++) {
            MPI_Send(&stop, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }

    if(rank > 1) {

        while(true) {
            int chunk_length;
            MPI_Recv(&chunk_length, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if(chunk_length == -1) {
                break;
            }
            vector<char> buffer(chunk_length);
            MPI_Recv(buffer.data(), chunk_length, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            string local_text_string(buffer.begin(), buffer.end());
            unordered_map<string, int> local_count = count_words(local_text_string);

            stringstream ss;
            {
                cereal::BinaryOutputArchive archive(ss);
                archive(local_count);
            }
            string serialized_data = ss.str();

            int serialized_size = serialized_data.size();

            MPI_Send(&serialized_size, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
            MPI_Send(serialized_data.data(), serialized_size, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
    }

    int stop1 = -1;
    MPI_Send(&stop1, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
}

    if(rank == 1) {
        unordered_map<string, int> global_word_count;
        
        int workers = size - 2;
        while(workers > 0) {
            int recv_size;
            MPI_Recv(&recv_size, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if(recv_size == -1) {
                workers--;
            } else {
                vector<char> recv_buffer(recv_size);
                MPI_Recv(recv_buffer.data(), recv_size, MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                string serialized_data(recv_buffer.begin(), recv_buffer.end());
                stringstream ss(serialized_data);
                cereal::BinaryInputArchive archive(ss);

                unordered_map<string, int> received_map;
                archive(received_map);

                merging(global_word_count, received_map);
            }
        }

        for(auto &word : global_word_count) {
            cout << "Processo: " << rank << " " << word.first << ":" << word.second << endl;
        }
    }

    MPI_Finalize();

    return 0;
}