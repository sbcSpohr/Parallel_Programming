#include <mpi.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <cstring>

using namespace std;

unordered_map<string, int> count_words(const string &text) {
    unordered_map<string, int> word_count;
    string word;
    stringstream texto(text);
    
    while (texto >> word) {
        word_count[word]++;
    }
    
    return word_count;
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    vector<string> lines;
    int total_lines = 0;
    
    if (rank == 0) {
        ifstream file(argv[1]);
        if (!file) {
            cerr << "Erro ao abrir o arquivo!" << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        
        string line;
        while (getline(file, line)) {
            lines.push_back(line);
        }
        file.close();
        total_lines = lines.size();
    }
    
    MPI_Bcast(&total_lines, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    int base_lines_count = total_lines / size;
    int extra = total_lines % size;
    vector<int> send_counts(size, base_lines_count);
    for (int i = 0; i < extra; i++) {
        send_counts[i]++;
    }
    
    vector<int> displacements(size, 0);
    for (int i = 1; i < size; i++) {
        displacements[i] = displacements[i - 1] + send_counts[i - 1];
    }
    
    int local_lines_count = send_counts[rank];
    vector<string> local_lines(local_lines_count);
    vector<char> buffer;
    
    if (rank == 0) {
        for (const auto &line : lines) {
            buffer.insert(buffer.end(), line.begin(), line.end());
            buffer.push_back('\n');
        }
    }
    
    int buffer_size = buffer.size();
    MPI_Bcast(&buffer_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    buffer.resize(buffer_size);
    MPI_Bcast(buffer.data(), buffer_size, MPI_CHAR, 0, MPI_COMM_WORLD);
    
    string local_text;
    for (int i = displacements[rank]; i < displacements[rank] + local_lines_count; i++) {
        local_text += lines[i] + " ";
    }
    
    unordered_map<string, int> local_word_count = count_words(local_text);
    
    if (rank == 0) {
        unordered_map<string, int> global_word_count = local_word_count;

        for (int i = 1; i < size; i++) {
            int word_count_size;
            MPI_Recv(&word_count_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            for (int j = 0; j < word_count_size; j++) {
                char word[100];
                int count;
                MPI_Recv(word, 100, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&count, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                global_word_count[word] += count;
            }
        }

        for (const auto &[word, count] : global_word_count) {
            cout << word << ": " << count << endl;
        }
    } else {
        int word_count_size = local_word_count.size();
        MPI_Send(&word_count_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

        for (const auto &[word, count] : local_word_count) {
            MPI_Send(word.c_str(), 100, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
            MPI_Send(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return 0;
}
