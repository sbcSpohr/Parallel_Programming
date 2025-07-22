#include <iostream>
#include <fstream>
#include <chrono>

int main() {

    const int N = 10000000;

    // ----------- Teste com std::endl ----------- // flush a cada linha (endl)

    std::ofstream file1("saida_endl.txt");
    auto start1 = std::chrono::high_resolution_clock::now();

    for(int i = 0; i < N; i++) {
        file1 << "Linha " << i << std::endl;    
    }
    file1.close();

    auto end1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration1 = end1 - start1;

    std::cerr << "Tempo com std::endl: " << duration1.count() << " segundos\n";

    std::cout.clear();

    // ----------- Teste com \n ----------- // 

    std::ofstream file2("saida_n.txt");
    auto start2 = std::chrono::high_resolution_clock::now();

    for(int i = 0; i < N; i++) {
        file2 << "Linha " << i << '\n';
    }
    file2.close();

    auto end2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration2 = end2 - start2;

    std::cerr << "Tempo com \\n: " << duration2.count() << " segundos\n";

    return 0;
}