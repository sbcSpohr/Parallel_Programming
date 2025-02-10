#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <sstream>
#include <mpi.h>

using namespace std;
 
unordered_map<string, int> countWords(string &texto) {

    unordered_map<string, int> word_count;
    string word;
    stringstream text(texto);

    while(getline(text, word, ' ')) {
        word_count[word]++;
    }
    return word_count;
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    string full_text;
    int total_size = 0;
    
    if(rank == 0) {
        ifstream file(argv[1]);
        if(!file) {
            cout << "ERRO arquivo" << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        stringstream store_text;
        store_text << file.rdbuf();     //armazena todo conteudo do arquivo na ss store_text (usar uma ss para facilitar o fluxo de dados com operador "<<")
        full_text = store_text.str();   //converte para uma string o store_text e armazena em full_text
        file.close();
        total_size = full_text.size();
    }

    MPI_Bcast(&total_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    full_text.resize(total_size);
    MPI_Bcast(&full_text[0], total_size, MPI_CHAR, 0, MPI_COMM_WORLD);

    int start = (total_size  / size) * rank;
    int end = (rank == size - 1) ? total_size : (total_size / size) * (rank + 1);

    while(start > 0 && &full_text[start] != ' ') {
        start++;
    }
    while(end < total_size  && &full_text[end] != ' ') {
        end++;
    }

    string local_text = full_text.substr(start, end - start);   //string substr(size_t pos, size_t len) const; pos(posicao que comeca a extrair o texto) len(comprimento da substring, quantos caracteres queremos extrair)
    unordered_map<string, int> local_word_count = countWords(local_text);

    if(rank == 0) {

        unordered_map<string, int> global_word_count = local_word_count;
        for(int i = 1 i < size; i++) {
        

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
