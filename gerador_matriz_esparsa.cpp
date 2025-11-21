#include <bits/stdc++.h>

#include "arvore_balanceada.hpp"
#include "matriz_hash.hpp"
#include "matriz_normal.hpp"

using namespace std;

struct Elemento {
    int i;
    int j;
    double x;
};

typedef vector<Elemento> ListaElementos;


/**
 * @brief Gera uma lista de elementos para uma matriz esparsa.
 * 
 * Cria coordenadas aleatórias (i, j) garantindo unicidade através de um set,
 * e gera valores aleatórios para preencher a matriz de acordo com a esparsidade desejada.
 * 
 * @param n Dimensão da matriz (n x n).
 * @param esparcidade_porcentagem Percentual de elementos não nulos (0-100).
 * @return ListaElementos Vetor contendo as triplas (i, j, valor) geradas.
 *
*/
ListaElementos gerador_matriz_esparca(int n, double esparcidade_porcentagem){
    double razao = esparcidade_porcentagem / 100.0;

    long long n_nulos = (long long)n * n * razao;

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(0, n - 1);
    uniform_real_distribution<> valor_dist(1.0, 100.0);

    set<pair<int, int>> posicoes_usadas;
    ListaElementos lista_elementos;
    lista_elementos.reserve(n_nulos);

    while (posicoes_usadas.size() < n_nulos) {
        int i = dist(gen);
        int j = dist(gen);

        if (posicoes_usadas.insert({i, j}).second) {
            double valor = valor_dist(gen);
            lista_elementos.push_back({i, j, valor});
        }
    }

    return lista_elementos;
}

/**
 * @brief Preenche uma Matriz de Árvore Balanceada com os dados gerados.
 * @param matriz Referência para a matriz alvo.
 * @param dados Vetor de elementos a serem inseridos.
 */
void preencher_matriz_com_lista(MatrizEsparsaArvoreBalanceada &matriz, const vector<Elemento> &dados) {
    for (const auto& item : dados) {
        matriz.inserir(item.i, item.j, item.x);
    }
}

/**
 * @brief Preenche uma Matriz Hash com os dados gerados.
 * @param matriz Referência para a matriz alvo.
 * @param dados Vetor de elementos a serem inseridos.
 */
void preencher_hash_com_lista(MatrizComTransposta &matriz, const vector<Elemento> &dados) {
    for (const auto& item : dados) {
        matriz.inserir(item.x, item.i, item.j);
    }
}

/**
 * @brief Preenche uma Matriz Normal com os dados gerados.
 * @param matriz Referência para a matriz alvo.
 * @param dados Vetor de elementos a serem inseridos.
 */
void preencher_normal_com_lista(MatrizNormal &matriz, const vector<Elemento> &dados) {
    for (const auto& item : dados) {
        matriz.inserir(item.i, item.j, item.x);
    }
}

/**
 * @class Cronometro
 * @brief Classe utilitária para medição de tempo de alta precisão.
 */
class Cronometro {
    private:
        chrono::high_resolution_clock::time_point inicio;
    public:
        /** @brief Inicia a contagem do tempo. */
        void start() {
            inicio = chrono::high_resolution_clock::now();
        }

        /** 
         * @brief Para a contagem e retorna a duração.
         * @return Tempo decorrido em microssegundos.
         */
        long long stop() {
            auto fim = chrono::high_resolution_clock::now();
            auto duracao = chrono::duration_cast<chrono::microseconds>(fim - inicio);
            return duracao.count();
        }
};

/**
 * @brief Registra uma linha de resultado no arquivo CSV.
 */
void registrar_resultado(ofstream &csv, string est, string op, int N, double esp, long long K, long long tempo) {
    csv << est << "," << op << "," << N << "," << esp << "," << K << "," << tempo << "\n";
}

/**
 * @brief Executa uma função lambda múltiplas vezes e calcula o tempo médio.
 * 
 * @tparam Func Tipo da função/lambda a ser executada.
 * @param repeticoes Número de vezes que a ação será executada.
 * @param acao Função que contém o código a ser cronometrado.
 * @return long long Tempo médio em microssegundos.
 */
template <typename Func>
long long medir_tempo_medio(int repeticoes, Func acao) {
    Cronometro timer;
    long long soma = 0;
    for (int r = 0; r < repeticoes; r++) {
        timer.start();
        acao(); 
        soma += timer.stop();
    }
    return (repeticoes > 0) ? (soma / repeticoes) : 0;
}

/**
 * @brief Executa a bateria de testes para a implementação de Árvore Balanceada.
 * 
 * Testa: Inserção, Soma, Multiplicação, Multiplicação por Escalar e Transposta.
 * 
 * @param csv Arquivo de saída.
 * @param N Dimensão.
 * @param esp Esparsidade.
 * @param K Elementos não nulos.
 * @param repeticoes Número de repetições para média.
 * @param dados1 Dados para a primeira matriz.
 * @param dados2 Dados para a segunda matriz (para operações que precisam de duas matrizes).
 */
void testar_arvore(ofstream &csv, int N, double esp, long long K, int repeticoes, 
                   const ListaElementos &dados1, const ListaElementos &dados2) {
    
    // 1. Inserção (cria nova instância a cada loop)
    long long t_ins = medir_tempo_medio(repeticoes, [&]() {
        MatrizEsparsaArvoreBalanceada temp;
        preencher_matriz_com_lista(temp, dados1);
    });
    registrar_resultado(csv, "Arvore", "Insercao", N, esp, K, t_ins);

    // Preparar instâncias fixas para operações
    MatrizEsparsaArvoreBalanceada m1, m2;
    preencher_matriz_com_lista(m1, dados1);
    preencher_matriz_com_lista(m2, dados2);

    // 2. Soma
    long long t_soma = medir_tempo_medio(repeticoes, [&]() {
        MatrizEsparsaArvoreBalanceada res = m1.somar(m2);
    });
    registrar_resultado(csv, "Arvore", "Soma", N, esp, K, t_soma);

    // 3. Multiplicação
    long long t_mult = medir_tempo_medio(repeticoes, [&]() {
        MatrizEsparsaArvoreBalanceada res = m1.multiplicar(m2);
    });
    registrar_resultado(csv, "Arvore", "Multiplicacao", N, esp, K, t_mult);

    // 4. Multiplicação por Escalar
    long long t_mult_escalar = medir_tempo_medio(repeticoes, [&]() {
        // Cria copia na heap e vaza memoria intencionalmente para evitar double free
        MatrizEsparsaArvoreBalanceada* res = new MatrizEsparsaArvoreBalanceada(m1);
        res->multiplicar_por_escalar(2.0);
    });
    registrar_resultado(csv, "Arvore", "Multiplicacao por Escalar", N, esp, K, t_mult_escalar);

    // 5. Transposta
    Cronometro timer;
    long long soma_transposta = 0;
    for (int r = 0; r < repeticoes; r++) {
        MatrizEsparsaArvoreBalanceada* temp = new MatrizEsparsaArvoreBalanceada();
        preencher_matriz_com_lista(*temp, dados1);
        
        timer.start();
        MatrizEsparsaArvoreBalanceada res = temp->transposta();
        soma_transposta += timer.stop();
        
        // res destroi os nos. temp eh vazado.
    }
    long long t_transposta = (repeticoes > 0) ? (soma_transposta / repeticoes) : 0;
    registrar_resultado(csv, "Arvore", "Transposta", N, esp, K, t_transposta);
}



/**
 * @brief Executa a bateria de testes para a implementação de Matriz Hash.
 * 
 * Testa: Inserção, Soma, Multiplicação, Multiplicação por Escalar e Transposta.
 * 
 * @param Mesmos parâmetros da função testar_arvore.
 */
void testar_hash(ofstream &csv, int N, double esp, long long K, int repeticoes, 
                 const ListaElementos &dados1, const ListaElementos &dados2) {
    
    long long t_ins = medir_tempo_medio(repeticoes, [&]() {
        MatrizComTransposta temp;
        preencher_hash_com_lista(temp, dados1);
    });
    registrar_resultado(csv, "Hash", "Insercao", N, esp, K, t_ins);

    MatrizComTransposta m1, m2;
    preencher_hash_com_lista(m1, dados1);
    preencher_hash_com_lista(m2, dados2);

    long long t_soma = medir_tempo_medio(repeticoes, [&]() {
        MatrizComTransposta res;
        somaMatrizes(m1.normal, m2.normal, res.normal);
    });
    registrar_resultado(csv, "Hash", "Soma", N, esp, K, t_soma);

    long long t_mult = medir_tempo_medio(repeticoes, [&]() {
        MatrizComTransposta res;
        multiplicarMatrizes(m1.normal, m2.normal, res.normal);
    });
    registrar_resultado(csv, "Hash", "Multiplicacao", N, esp, K, t_mult);

    long long t_mult_escalar = medir_tempo_medio(repeticoes, [&]() {
        MatrizComTransposta* res = new MatrizComTransposta(m1);
        res->multiplicarEscalar(2.0);
    });
    registrar_resultado(csv, "Hash", "Multiplicacao por Escalar", N, esp, K, t_mult_escalar);

    long long t_transposta = medir_tempo_medio(repeticoes, [&]() {
        Matriz* trans = m1.getTransposta();
    });
    registrar_resultado(csv, "Hash", "Transposta", N, esp, K, t_transposta);
}


/**
 * @brief Executa a bateria de testes para a implementação de Matriz Normal.
 * 
 * Testa: Inserção, Soma, Multiplicação, Multiplicação por Escalar e Transposta.
 * 
 * @param Mesmos parâmetros da função testar_arvore.
 */
void testar_normal(ofstream &csv, int N, double esp, long long K, int repeticoes, 
                   const ListaElementos &dados1, const ListaElementos &dados2) {
    
    long long t_ins = medir_tempo_medio(repeticoes, [&]() {
        MatrizNormal temp(N, N, 0.0);
        preencher_normal_com_lista(temp, dados1);
    });
    registrar_resultado(csv, "Normal", "Insercao", N, esp, K, t_ins);

    MatrizNormal m1(N, N, 0.0), m2(N, N, 0.0);
    preencher_normal_com_lista(m1, dados1);
    preencher_normal_com_lista(m2, dados2);

    long long t_soma = medir_tempo_medio(repeticoes, [&]() {
        MatrizNormal res = m1.somar(m2);
    });
    registrar_resultado(csv, "Normal", "Soma", N, esp, K, t_soma);

    long long t_mult = medir_tempo_medio(repeticoes, [&]() {
        MatrizNormal res = m1.multiplicar(m2);
    });
    registrar_resultado(csv, "Normal", "Multiplicacao", N, esp, K, t_mult);

    long long t_mult_escalar = medir_tempo_medio(repeticoes, [&]() {
        MatrizNormal res = m1;
        res.multiplicar_por_escalar(2.0);
    });
    registrar_resultado(csv, "Normal", "Multiplicacao por Escalar", N, esp, K, t_mult_escalar);

    long long t_transposta = medir_tempo_medio(repeticoes, [&]() {
        MatrizNormal res = m1.transposta();
    });
    registrar_resultado(csv, "Normal", "Transposta", N, esp, K, t_transposta);
}


/**
 * @brief Cenário de teste 1: Comparação entre as três estruturas (Pequenas dimensões).
 * 
 * Executa testes com N=100 e N=1000. Inclui a Matriz Normal, que consome muita memória
 * para N grandes, limitando este teste a dimensões menores.
 * Salva em: resultados_experimento_tres_estruturas.csv
 */
void testes_tres_estruturas() {
    ofstream arquivo_csv("resultados_experimento_tres_estruturas.csv");
    arquivo_csv << "Estrutura,Operacao,Dimensao(N),Esparsidade(%),K_Real,Tempo(us)\n";
    
    vector<int> dimensoes = {100, 1000}; // Dimensões pequenas onde a Normal aguenta
    vector<double> esparsidades = {1.0, 5.0, 10.0, 20.0};
    int repeticoes = 10;

    cout << "=== INICIANDO TESTES: 3 ESTRUTURAS ===" << endl;

    for (int N : dimensoes) {
        for (double esp : esparsidades) {
            cout << "Testando N=" << N << ", Esp=" << esp << "%" << endl;
            
            auto dados1 = gerador_matriz_esparca(N, esp);
            auto dados2 = gerador_matriz_esparca(N, esp);
            long long K = dados1.size();

            // Chama as funções modulares
            testar_arvore(arquivo_csv, N, esp, K, repeticoes, dados1, dados2);
            testar_hash(arquivo_csv, N, esp, K, repeticoes, dados1, dados2);
            testar_normal(arquivo_csv, N, esp, K, repeticoes, dados1, dados2);
        }   
    }
    arquivo_csv.close();
}

/**
 * @brief Cenário de teste 2: Comparação entre Árvore Balanceada e Matriz Hash (Grandes dimensões).
 * 
 * Executa testes com N=10^4, 10^5, 10^6. Exclui a Matriz Normal devido ao alto consumo de memória.
 * Salva em: resultados_experimento_duas_estruturas.csv
 */
void testes_duas_estruturas() {
    ofstream arquivo_csv("resultados_experimento_duas_estruturas.csv");
    arquivo_csv << "Estrutura,Operacao,Dimensao(N),Esparsidade(%),K_Real,Tempo(us)\n";
    
    int repeticoes = 10;

    for (int i = 4; i <= 6; i++) {
        int N = pow(10, i);
        
        for (int k = 2; k >= 0; k--) {
            double esp = 1.0 / pow(10, i + k); 
            
            cout << "------------------------------------------------" << endl;
            cout << "Iniciando bateria: N=" << N << ", Esp=" << esp << "%" << endl;

            // Gerar dados uma vez para serem justos com ambas as estruturas
            auto dados1 = gerador_matriz_esparca(N, esp);
            auto dados2 = gerador_matriz_esparca(N, esp);
            long long K = dados1.size();

            if (K == 0) cout << "AVISO: Matriz gerada vazia (aumente a esparsidade)." << endl;

            // Executar testes modulares
            testar_arvore(arquivo_csv, N, esp, K, repeticoes, dados1, dados2);
            testar_hash(arquivo_csv, N, esp, K, repeticoes, dados1, dados2);
        }
    }
    arquivo_csv.close();
}

int main() {
    testes_tres_estruturas();
    testes_duas_estruturas();
    cout << "Testes finalizados. Resultados salvos em resultados_experimento.csv" << endl;
    return 0;
}