#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <sstream>
#include <mpi.h>

using namespace std;

unordered_map<string, int> countWords(const string &texto) {

    unordered_map<string, int> word_count;
    string word;
    stringstream text(texto);
   
    while(getline(text, word, ' ')) {
        word_count[word]++;
    }

    return word_count;
}

void mergeMaps(unordered_map<string, int> &global_map, const unordered_map<string, int> &local_map) {
    for (const auto &entry : local_map) {
        global_map[entry.first] += entry.second;
    }
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    string full_text;
    int total_size = 0;

    if (rank == 0) {
        ifstream file(argv[1]);
        if (!file) {
            cout << "ERRO arquivo" << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        stringstream buffer;
        buffer << file.rdbuf();         //armazena todo conteudo do arquivo na ss buffer (usar uma ss para o fluxo de dados com operador "<<")
        full_text = buffer.str();       //converte para uma string o buffer e armazena em full_text
        file.close();                   
        total_size = full_text.size();  
    }

    MPI_Bcast(&total_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    full_text.resize(total_size);
    MPI_Bcast(&full_text[0], total_size, MPI_CHAR, 0, MPI_COMM_WORLD);

    int start = (total_size / size) * rank;
    int end = (rank == size - 1) ? total_size : (total_size / size) * (rank + 1);

    // Ajustar início e fim para evitar dividir palavras ao meio
    while (start > 0 && full_text[start] != ' ') start++;
    while (end < total_size && full_text[end] != ' ') end++;

    string local_text = full_text.substr(start, end - start);
    unordered_map<string, int> local_word_count = countWords(local_text);

    // Enviar resultados para o processo 0
    if (rank == 0) {
        unordered_map<string, int> global_word_count = local_word_count;
        for (int i = 1; i < size; ++i) {
            int recv_size;
            MPI_Recv(&recv_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            vector<char> recv_buffer(recv_size);
            MPI_Recv(recv_buffer.data(), recv_size, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            string received_string(recv_buffer.begin(), recv_buffer.end());
            unordered_map<string, int> received_map = countWords(received_string);
            mergeMaps(global_word_count, received_map);
        }

        for (const auto &entry : global_word_count) {
            cout << entry.first << ": " << entry.second << endl;
        }
    } else {
        string local_text_string(local_text);
        int send_size = local_text_string.size();
        MPI_Send(&send_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(local_text_string.data(), send_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
