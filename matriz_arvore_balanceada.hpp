//Integrantes:
//Isabel Cristina Marras Salles - RA 188790
//Victor Luigi Roquetto - RA 250955
//Rafael Feltrin Lamen Rocha - RA 276246


#ifndef MATRIZ_ESPARSA_RB_HPP
#define MATRIZ_ESPARSA_RB_HPP

#include <iostream>
#include <vector>
#include <algorithm>
#include <set>

using namespace std;

class MatrizEsparsaArvoreBalanceada {
    private:
        enum cor {VERMELHO, PRETO};
        
        struct no {
            int i, j;
            double x;
            enum cor cor;
            struct no *esq, *dir;
            no(int linha, int coluna, double valor) : i(linha), j(coluna), x(valor), cor(VERMELHO), esq(nullptr), dir(nullptr) {}
        };

        typedef struct no *p_no;

        p_no raiz;
        p_no raiz_transposta;

        int ehVermelho(p_no no) {
            if (no == nullptr) return false;
            return no->cor == VERMELHO;
        }
        
        p_no rotaciona_para_esquerda(p_no raiz){
            p_no nova_raiz = raiz->dir;
            raiz->dir = nova_raiz->esq;
            nova_raiz->esq = raiz;
            nova_raiz->cor = raiz->cor;
            raiz->cor = VERMELHO;
            return nova_raiz;
        }

        p_no rotaciona_para_direita(p_no raiz){
            p_no nova_raiz = raiz->esq;
            raiz->esq = nova_raiz->dir;
            nova_raiz->dir = raiz;
            nova_raiz->cor = raiz->cor;
            raiz->cor = VERMELHO;
            return nova_raiz;
        }

        void sobe_vermelho(p_no raiz){
            raiz->cor = VERMELHO;
            raiz->esq->cor = PRETO;
            raiz->dir->cor = PRETO;
        }

        void desce_vermelho(p_no raiz){
            raiz->cor = PRETO;
            raiz->esq->cor = VERMELHO;
            raiz->dir->cor = VERMELHO;
        }

        p_no descer_vermelho_esq(p_no raiz){
            desce_vermelho(raiz);
            if (ehVermelho(raiz->dir->esq)){
                raiz->dir = rotaciona_para_direita(raiz->dir);
                raiz = rotaciona_para_esquerda(raiz);
                sobe_vermelho(raiz);
            }
            return raiz;
        }

        p_no descer_vermelho_dir(p_no raiz){
            desce_vermelho(raiz);
            if (ehVermelho(raiz->esq->esq)){
                raiz = rotaciona_para_direita(raiz);
                sobe_vermelho(raiz);
            }
            return raiz;
        }

        p_no min(p_no raiz){
            if (raiz->esq == nullptr){
                return raiz;
            } 
            else {
                return min(raiz->esq);
            }
        }

        p_no balancear(p_no raiz) {
            if (ehVermelho(raiz->dir))
                raiz = rotaciona_para_esquerda(raiz);
            
            if (ehVermelho(raiz->esq) && ehVermelho(raiz->esq->esq))
                raiz = rotaciona_para_direita(raiz);
            
            if (ehVermelho(raiz->esq) && ehVermelho(raiz->dir))
                sobe_vermelho(raiz);

            return raiz;
        }

        p_no deletar_min(p_no raiz){
            if (raiz->esq == nullptr){
                delete raiz;
                return nullptr;
            }
            if (!ehVermelho(raiz->esq) && !ehVermelho(raiz->esq->esq)){
                raiz = descer_vermelho_esq(raiz);
            }
            raiz->esq = deletar_min(raiz->esq);
            return balancear(raiz);
        }

        /**
         * @brief Insere ou atualiza um valor na posição (i, j) da matriz.
         * Se o valor x for 0.0, o nó correspondente será removido da árvore (se existir).
         * *Caso contrário, insere um novo nó ou atualiza o existente.
         * @param i Índice da linha (base 0).
         * @param j Índice da coluna (base 0).
         * @param x Valor a ser inserido.
         * @return Ponteiro para a raiz da árvore após a inserção.
         */

        p_no inserir_rec(p_no raiz, int i, int j, double x) {
            if (raiz == nullptr) {
                return new no(i, j, x);
            }
            
            if (i < raiz->i){
                raiz-> esq = inserir_rec(raiz->esq, i, j, x);
            } 
            else if (i > raiz->i){
                raiz-> dir = inserir_rec(raiz->dir, i, j, x);
            } 
            else {
                if (j < raiz->j){
                    raiz-> esq = inserir_rec(raiz->esq, i, j, x);
                } 
                else if (j > raiz->j){
                    raiz-> dir = inserir_rec(raiz->dir, i, j, x);
                } 
                else {
                    raiz->x = x; // ATUALIZA VALOR
                    return raiz;
                }
            }
            if (ehVermelho(raiz->dir) && !ehVermelho(raiz->esq)) {
                raiz = rotaciona_para_esquerda(raiz);
            }
            if (ehVermelho(raiz->esq) && ehVermelho(raiz->esq->esq)) {
                raiz = rotaciona_para_direita(raiz);
            }
            if (ehVermelho(raiz->esq) && ehVermelho(raiz->dir)) {
                sobe_vermelho(raiz);
            }

            return raiz;
        }

        /**
        * @brief Acessa o valor na posição (i, j) da matriz.
        * @param i Índice da linha (base 0).
        * @param j Índice da coluna (base 0).
        * @return Valor na posição (i, j) ou 0.0 se não existir.
        */
        double acessar_rec (p_no no_atual, int i, int j) const{
            if (no_atual == nullptr){
                return 0;
            }
            if (i < no_atual->i) {
                return acessar_rec(no_atual->esq, i, j);
            } 
            else if (i > no_atual->i) {
                return acessar_rec(no_atual->dir, i, j);
            } 
            else {
                if (j < no_atual->j) {
                    return acessar_rec(no_atual->esq, i, j);
                } 
                else if (j > no_atual->j) {
                    return acessar_rec(no_atual->dir, i, j);
                } 
                else {
                    return no_atual->x;
                }
            }
        }

        /**
         * @brief Remove o nó na posição (i, j) da matriz.
         * @param i Índice da linha (base 0).
         * @param j Índice da coluna (base 0).
         * @return Ponteiro para a raiz da árvore após a remoção.
         */
        p_no remover_rec(p_no raiz, int i, int j) {
            if (raiz == nullptr) {
                return nullptr;
            }

            if (i < raiz->i || (i == raiz->i && j < raiz->j)) {
                if (raiz->esq == nullptr) {
                    return raiz; 
                }
                if (!ehVermelho(raiz->esq) && !ehVermelho(raiz->esq->esq)) {
                    raiz = descer_vermelho_esq(raiz);
                }
                raiz->esq = remover_rec(raiz->esq, i, j);
            } 
            else {
                if (ehVermelho(raiz->esq)) {
                    raiz = rotaciona_para_direita(raiz);
                }
                if (i == raiz->i && j == raiz->j) {
                    if (raiz->dir == nullptr) {
                        delete raiz ;
                        return nullptr;
                    }
                    
                    if (!ehVermelho(raiz->dir) && !ehVermelho(raiz->dir->esq)) {
                        raiz = descer_vermelho_dir(raiz);
                    }

                    p_no sucessor = min(raiz->dir);
                    raiz->i = sucessor->i;
                    raiz->j = sucessor->j;
                    raiz->x = sucessor->x;
                    
                    raiz->dir = deletar_min(raiz->dir);
                }

            else {
                if (raiz->dir == nullptr) {
                    return raiz;
                }
                if (!ehVermelho(raiz->dir) && !ehVermelho(raiz->dir->esq)) {
                    raiz = descer_vermelho_dir(raiz);
                }
                raiz->dir = remover_rec(raiz->dir, i, j);
            }
        }

            return balancear(raiz);
        }

        void imprimir_rec(p_no no_atual) {
            if (no_atual == nullptr) {
                return;
            }
            imprimir_rec(no_atual->esq);
            std::cout << "Elemento na posicao (" << no_atual->i << ", " << no_atual->j << ") = " << no_atual->x << std::endl;
            imprimir_rec(no_atual->dir);
        }

        /**
         * @brief Multiplica todos os elementos da matriz por um escalar.
         * @param no_atual Nó atual na árvore.
         * @param escalar Valor escalar para multiplicação.
         */
        void multiplicar_por_escalar_rec(p_no no_atual, double escalar) {
            if (no_atual == nullptr) {
                return;
            }
            no_atual->x *= escalar;
            multiplicar_por_escalar_rec(no_atual->esq, escalar);
            multiplicar_por_escalar_rec(no_atual->dir, escalar);
        }

        /**
         * @brief Soma os elementos da matriz atual com outra matriz e armazena o resultado.
         * @param no_atual Nó atual na árvore.
         * @param matriz2 Segunda matriz para soma.
         * @param resultado Matriz onde o resultado será armazenado.
         */
        void somar_rec(p_no no_atual, const MatrizEsparsaArvoreBalanceada &matriz2, MatrizEsparsaArvoreBalanceada &resultado){
            if (no_atual == nullptr) {
                return;
            }
            if (resultado.acessar(no_atual->i,no_atual->j) != 0.0){
                return;
            }
            double valor_no_equivalente = matriz2.acessar(no_atual->i, no_atual->j);
            double valor_soma = no_atual->x + valor_no_equivalente;
            
            if (valor_soma != 0.0){
                resultado.inserir(no_atual->i, no_atual->j, valor_soma);
            }

            somar_rec(no_atual->esq, matriz2, resultado);
            somar_rec(no_atual->dir, matriz2, resultado);
        }

        /**
         * @brief Insere ou soma um valor na posição (i, j) da matriz.
         * Importante para a multiplicação, onde vamos somando produtos parciais
         * @param i Índice da linha (base 0).
         * @param j Índice da coluna (base 0).
         * @param x Valor a ser inserido ou somado.
         */
        p_no inserir_ou_somar_rec(p_no raiz, int i, int j, double valor_a_somar) {
            if (raiz == nullptr) {
                return new no(i, j, valor_a_somar);
            }
            
            if (i < raiz->i){
                raiz->esq = inserir_ou_somar_rec(raiz->esq, i, j, valor_a_somar);
            } 
            else if (i > raiz->i){
                raiz->dir = inserir_ou_somar_rec(raiz->dir, i, j, valor_a_somar);
            } 
            else {
                if (j < raiz->j){
                    raiz->esq = inserir_ou_somar_rec(raiz->esq, i, j, valor_a_somar);
                } 
                else if (j > raiz->j){
                    raiz->dir = inserir_ou_somar_rec(raiz->dir, i, j, valor_a_somar);
                } 
                else {
                    //soma direto no nó existente
                    raiz->x += valor_a_somar; 
                    if (raiz->x == 0.0) {
                        // Se o valor somado resultar em zero, remove o nó
                        return remover_rec(raiz, i, j);
                    }
                    return raiz;
                }
            }

            if (ehVermelho(raiz->dir) && !ehVermelho(raiz->esq)) {
                raiz = rotaciona_para_esquerda(raiz);
            }
            if (ehVermelho(raiz->esq) && ehVermelho(raiz->esq->esq)) {
                raiz = rotaciona_para_direita(raiz);
            }
            if (ehVermelho(raiz->esq) && ehVermelho(raiz->dir)) {
                sobe_vermelho(raiz);
            }

            return raiz;
        }

        void acumular(int i, int j, double x) {
            if (x == 0.0) return;
            raiz = inserir_ou_somar_rec(raiz, i, j, x);
            raiz->cor = PRETO;
        }

        /**
         * @brief Coleta todos os nós da linha i na árvore.
         * @param no_atual Nó atual na árvore.
         * @param i Índice da linha (base 0).
         * @param nos_encontrados Vetor para armazenar os nós encontrados.
         */
        void elementos_linha_i(p_no no_atual, int i, vector<p_no> &nos_encontrados){
            if (no_atual == nullptr){
                return;
            }
            if (i < no_atual->i){
                elementos_linha_i(no_atual->esq, i, nos_encontrados);
            } 
            else if (i > no_atual->i){
                elementos_linha_i(no_atual->dir, i, nos_encontrados);
            } 
            else {
                elementos_linha_i(no_atual->esq, i, nos_encontrados);
                nos_encontrados.push_back(no_atual);
                elementos_linha_i(no_atual->dir, i, nos_encontrados);
            }
        }
    
    
        /**
         * @brief Realiza a multiplicação recursiva dos elementos da matriz.
         * @param no_atual Nó atual na árvore da matriz.
         * @param matriz2 Segunda matriz para multiplicação.
         * @param resultado Matriz resultado da multiplicação.
         */
        void multi_rec(p_no no_atual, const MatrizEsparsaArvoreBalanceada &matriz2, MatrizEsparsaArvoreBalanceada &resultado){
            if (no_atual == nullptr){
                return;
            }
            int i = no_atual->i;
            int k = no_atual->j;
            vector<p_no> nos_linha_k;

            elementos_linha_i(matriz2.raiz, k, nos_linha_k);

            // Realiza a multiplicação e acumula os resultados
            for (size_t w = 0; w < nos_linha_k.size(); w++){
                p_no no_matriz2 = nos_linha_k[w];
                int j = no_matriz2->j;
                double produto = no_atual->x * no_matriz2->x;
                
                resultado.acumular(i, j, produto);
            }
            multi_rec(no_atual->esq, matriz2, resultado);
            multi_rec(no_atual->dir, matriz2, resultado);
        }

        void liberar_no(p_no no_atual){
            if (no_atual == nullptr){
                return;
            }
            liberar_no(no_atual->esq);
            liberar_no(no_atual->dir);
            delete no_atual;
        }

        MatrizEsparsaArvoreBalanceada(p_no nova_normal, p_no nova_transposta) : raiz(nova_normal), raiz_transposta(nova_transposta) {}

    public:
        MatrizEsparsaArvoreBalanceada() : raiz(nullptr), raiz_transposta(nullptr) {}

        void inserir(int i, int j, double x){
            if (x == 0.0) {
                raiz = remover_rec(raiz, i, j);
                raiz_transposta = remover_rec(raiz_transposta, j, i);
                if (raiz != nullptr) {
                    raiz->cor = PRETO;
                }
                if (raiz_transposta != nullptr) {
                    raiz_transposta->cor = PRETO;
                }
                return;
            }
            raiz = inserir_rec(raiz, i, j, x);
            raiz_transposta = inserir_rec(raiz_transposta, j, i, x);
            raiz->cor = PRETO;
            raiz_transposta->cor = PRETO;
        }
        
        double acessar(int i, int j) const{
            return acessar_rec(raiz, i, j);
        }

        void multiplicar_por_escalar(double escalar){
            if (escalar == 0.0) {
                limpar();
                return;
            }
            multiplicar_por_escalar_rec(raiz, escalar);
        }

        MatrizEsparsaArvoreBalanceada somar(const MatrizEsparsaArvoreBalanceada &matriz2) {
            MatrizEsparsaArvoreBalanceada resultado;
            somar_rec(raiz, matriz2, resultado);
            somar_rec(matriz2.raiz, *this, resultado);

            return resultado;
        }

        MatrizEsparsaArvoreBalanceada multiplicar(const MatrizEsparsaArvoreBalanceada &matriz2) {
            MatrizEsparsaArvoreBalanceada resultado;
            multi_rec(raiz, matriz2, resultado);

            return resultado;
        }

        MatrizEsparsaArvoreBalanceada transposta() {
            return MatrizEsparsaArvoreBalanceada(raiz_transposta, raiz);
        }

        void imprimir() {
            imprimir_rec(raiz);
        }

        ~MatrizEsparsaArvoreBalanceada() {
            limpar();
        }

        void limpar() {
            liberar_no(raiz);
            liberar_no(raiz_transposta);
            raiz = nullptr;
            raiz_transposta = nullptr;
        }
};

#endif
