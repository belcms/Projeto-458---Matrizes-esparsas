#ifndef MATRIZ_HASH_HPP
#define MATRIZ_HASH_HPP

#include <iostream>
#include <vector>
#include <stdio.h> 

using namespace std;

#define MAX 1783

typedef struct NoColuna *p_no_coluna;

struct NoColuna
{
    int i, j, valor;
    p_no_coluna prox;

    NoColuna(int i, int j, int valor)
    {
        this->i = i;
        this->j = j;
        this->valor = valor;
        prox = nullptr;
    }
};

struct Hash_colunas
{
    int n_buckets;
    int qtd_armazenada;
    vector<p_no_coluna> tabela;

    Hash_colunas(int tamanho) : n_buckets(tamanho), qtd_armazenada(0), tabela(tamanho, nullptr)
    {
    }

    ~Hash_colunas()
    {
        for (int i = 0; i < n_buckets; ++i)
        {
            p_no_coluna atual = tabela[i];
            while (atual != nullptr)
            {
                p_no_coluna temp = atual;
                atual = atual->prox;
                delete temp; // Libera a memória do nó
            }
        }
    }

    int funcaoHash(int j)
    {
        return (31 * j) % this->n_buckets;
    }

    p_no_coluna inserir_lista(p_no_coluna lista, int i, int j, int valor)
    {
        if(valor == 0){
            if(lista == nullptr){
                return nullptr;
            }
            if(lista->i == i && lista->j == j){
                p_no_coluna proximo_no = lista->prox;
                delete lista;
                this->qtd_armazenada--;
                return proximo_no;
            }
            p_no_coluna aux = lista;
            while(aux->prox != nullptr){
                if(aux->prox->i == i && aux->prox->j == j){
                    p_no_coluna no_remover = aux->prox;
                    aux->prox = no_remover->prox;
                    delete no_remover;
                    this->qtd_armazenada--;
                    return lista;
                }
                aux = aux->prox;
            }
            return lista;
        }
        if (lista == NULL)
        {
            p_no_coluna novo = new NoColuna(i, j, valor);
            this->qtd_armazenada++;
            return novo;
        }
        p_no_coluna aux = lista;

        if (aux->i == i && aux->j == j)
        {
            aux->valor = valor;
            return lista;
        }
        while (aux->prox != NULL)
        {
            if (aux->prox->i == i && aux->prox->j == j)
            {
                aux->prox->valor = valor;
                return lista;
            }
            aux = aux->prox;
        }
        p_no_coluna novo = new NoColuna(i, j, valor);
        aux->prox = novo;
        this->qtd_armazenada++;
        return lista;
    }

    void rehash(bool encolher)
    {
        int antigo_n_buckets = this->n_buckets;
        if(encolher == true){
            this->n_buckets = max(5, antigo_n_buckets/2);
        }else{
            this->n_buckets = antigo_n_buckets * 2 + 1;
        }
        if (this->n_buckets == antigo_n_buckets) return;

        vector<p_no_coluna> tabela_nova(this->n_buckets, nullptr);

        int n_hash;
        for (int i = 0; i < antigo_n_buckets; i++)
        {
            p_no_coluna aux = this->tabela[i];

            while (aux != nullptr)
            {
                p_no_coluna prox = aux->prox;
                n_hash = funcaoHash(aux->j);
                aux->prox = tabela_nova[n_hash];
                tabela_nova[n_hash] = aux;
                aux = prox;
            }
        }
        this->tabela = tabela_nova;
    }

    void inserir(int valor, int i, int j)
    {
        int n = funcaoHash(j);
        int qntd_antiga = this->qtd_armazenada;
        this->tabela[n] = inserir_lista(this->tabela[n], i, j, valor);
        if (this->qtd_armazenada > qntd_antiga && this->qtd_armazenada > 0.75 * this->n_buckets)
        {
            rehash(false);
        }
        if(this->n_buckets > 16 && this->qtd_armazenada < 0.25 * this->n_buckets){
            rehash(true);
        }
    }

    int acharElemento(int i, int j)
    {
        int n = funcaoHash(j);
        p_no_coluna aux = this->tabela[n];
        while (aux != nullptr)
        {
            if (aux->i == i && aux->j == j)
            {
                return aux->valor;
            }
            aux = aux->prox;
        }
        return 0;
    }

    // Dentro da struct Hash_valores

    void imprimirDebug()
    {
        cout << "      (Interno: " << qtd_armazenada << " elems, " << n_buckets << " baldes)" << endl;

        for (int b_interno = 0; b_interno < n_buckets; b_interno++)
        {
            p_no_coluna aux_valor = tabela[b_interno];

            // Só imprime baldes que não estão vazios
            if (aux_valor == nullptr)
                continue;

            cout << "      -> Balde [" << b_interno << "]: ";
            while (aux_valor != nullptr)
            {
                // Imprime o nó de valor
                cout << "[(" << aux_valor->i << ", " << aux_valor->j << ") = " << aux_valor->valor << "] -> ";
                aux_valor = aux_valor->prox;
            }
            cout << "NULL" << endl;
        }
    }
};

typedef struct NoLinha *p_no_linha;
struct NoLinha
{
    int i;
    Hash_colunas *valores;
    p_no_linha prox;

    NoLinha(int i)
    {
        this->i = i;
        this->prox = nullptr;
        this->valores = new Hash_colunas(5);
    }

    ~NoLinha()
    {
        delete valores;
    }
};

struct Matriz
{
    int n_buckets;
    int qtd_armazenada;
    vector<p_no_linha> tabela_linhas;

    Matriz(int tamanho) : n_buckets(tamanho), qtd_armazenada(0), tabela_linhas(tamanho, nullptr)
    {
    }

    ~Matriz()
    {
        for (int i = 0; i < n_buckets; i++)
        {
            p_no_linha atual = tabela_linhas[i];
            while (atual != nullptr)
            {
                p_no_linha temp = atual;
                atual = atual->prox;
                delete temp;  
            }
        }
    }

    int funcaoHash(int i)
    {
        return (31 * i) % this->n_buckets;
    }

    void inserir(int valor, int i, int j)
    {
        int n = funcaoHash(i);
        p_no_linha aux = tabela_linhas[n];
        p_no_linha anterior = nullptr;


        while (aux != nullptr && aux->i != i)
        {
            anterior = aux;
            aux = aux->prox;
        }

        if(valor != 0){
            int qnt_antiga = this->qtd_armazenada;

            if(aux == nullptr){
                aux = new NoLinha(i);
                this->qtd_armazenada++;

                if(anterior == nullptr){
                    tabela_linhas[n] = aux;
                }else{
                    anterior->prox = aux;
                }
            }
            aux->valores->inserir(valor, i , j);

            if(this->qtd_armazenada > qnt_antiga && this->qtd_armazenada > 0.75 * this->n_buckets){
                rehash(false);
            }

        }
        else{
            if (aux == nullptr) {
                return;
            }
            aux->valores->inserir(valor, i, j);
            if(aux->valores->qtd_armazenada == 0){
                if(anterior == nullptr){
                    tabela_linhas[n] = aux->prox;
                }
                else{
                    anterior->prox = aux->prox;
                }
                delete aux;
                this->qtd_armazenada--;
                if (this->n_buckets > 16 && this->qtd_armazenada < 0.25 * this->n_buckets) {
                    rehash(true); 
                }
            }
        }  
    }

    void rehash(bool encolher)
    {
        int antigo_n_buckets = this->n_buckets;
        if(encolher == true){
            this->n_buckets = max(5, antigo_n_buckets/2);
        }else{
            this->n_buckets = antigo_n_buckets * 2 + 1;
        }
        if (this->n_buckets == antigo_n_buckets) return;
        vector<p_no_linha> tabela_nova(this->n_buckets, nullptr);

        int n_hash;
        for (int i = 0; i < antigo_n_buckets; i++)
        {
            p_no_linha aux = tabela_linhas[i];

            while (aux != nullptr)
            {
                p_no_linha prox = aux->prox;

                n_hash = funcaoHash(aux->i);
                aux->prox = tabela_nova[n_hash];
                tabela_nova[n_hash] = aux;
                aux = prox;
            }
        }
        this->tabela_linhas = tabela_nova;
    }

    int acharElemento(int i, int j)
    {
        int n = funcaoHash(i);
        p_no_linha aux = tabela_linhas[n];
        while (aux != nullptr)
        {
            if (aux->i == i)
            {
                return aux->valores->acharElemento(i, j);
            }
            aux = aux->prox;
        }
        return 0;
    }

    // Dentro da struct Matriz

    void imprimirDebug()
    {

        cout << "\n--- Início Dump Matriz ---" << endl;
        cout << "Baldes Principais: " << n_buckets << " | Linhas não-nulas: " << qtd_armazenada << endl;
        cout << "---------------------------------" << endl;

        for (int b = 0; b < n_buckets; b++)
        {
            p_no_linha aux_linha = tabela_linhas[b];

            if (aux_linha == nullptr)
            {
                cout << "Balde[" << b << "]: Vazio" << endl;
            }
            else
            {
                cout << "Balde[" << b << "]:" << endl;
            }

            // Itera pela lista ligada de NoLinha (colisões da tabela principal)
            while (aux_linha != nullptr)
            {
                cout << "  -> Linha " << aux_linha->i << ":" << endl;

                // Chama a função de impressão da tabela interna
                aux_linha->valores->imprimirDebug();

                aux_linha = aux_linha->prox;
            }
        }
        cout << "--- Fim Dump Matriz ---\n"
             << endl;
    }

    void multiplicaEscalar(int escalar)
    {
        for (int i = 0; i < this->n_buckets; i++)
        {
            p_no_linha aux_linha = this->tabela_linhas[i];
            while (aux_linha != nullptr)
            {
                Hash_colunas *hash_colunas = aux_linha->valores;
                for (int k = 0; k < hash_colunas->n_buckets; k++)
                {
                    p_no_coluna aux_valor = hash_colunas->tabela[k];
                    while (aux_valor != nullptr)
                    {
                        aux_valor->valor = aux_valor->valor * escalar;
                        aux_valor = aux_valor->prox;
                    }
                }
                aux_linha = aux_linha->prox;
            }
        }
    }
};

void somaMatrizes(Matriz* a, Matriz* b, Matriz* c)
{
    for (int i = 0; i < a->n_buckets; i++)
    {
        p_no_linha aux_linha = a->tabela_linhas[i];
        while (aux_linha != nullptr)
        {
            Hash_colunas *hash_colunas = aux_linha->valores;
            for (int k = 0; k < hash_colunas->n_buckets; k++)
            {
                p_no_coluna aux_valor = hash_colunas->tabela[k];
                while (aux_valor != nullptr)
                {
                    c->inserir(aux_valor->valor, aux_valor->i, aux_valor->j);
                    aux_valor = aux_valor->prox;
                }
            }
            aux_linha = aux_linha->prox;
        }
    }


    for (int j = 0; j < b->n_buckets; j++)
    {
        p_no_linha aux_linha = b->tabela_linhas[j];
        while (aux_linha != nullptr)
        {
            Hash_colunas *hash_colunas = aux_linha->valores;
            for (int m = 0; m < hash_colunas->n_buckets; m++)
            {
                p_no_coluna aux_valor = hash_colunas->tabela[m];
                while (aux_valor != nullptr)
                {
                    int valor_c = c->acharElemento(aux_valor->i, aux_valor->j);
                    c->inserir(valor_c + aux_valor->valor, aux_valor->i, aux_valor->j);
                    aux_valor = aux_valor->prox;
                }
            }
            aux_linha = aux_linha->prox;
        }
    }
}


void multiplicarMatrizes(Matriz* a, Matriz* b, Matriz *c){
    for(int i = 0; i < a->n_buckets; i++){
        p_no_linha aux_linha_a = a->tabela_linhas[i];
        while (aux_linha_a != nullptr){
            Hash_colunas *hash_colunas_a = aux_linha_a->valores;
            for (int k = 0; k < hash_colunas_a->n_buckets; k++)
            {
                p_no_coluna aux_valor_a = hash_colunas_a->tabela[k];
                while (aux_valor_a != nullptr)
                {
                    int k_a = aux_valor_a->j;
                    int n_hash_linha_b = b->funcaoHash(k_a);
                    p_no_linha aux_linha_b = b->tabela_linhas[n_hash_linha_b];

                    while(aux_linha_b != nullptr && aux_linha_b->i != k_a ){
                        aux_linha_b = aux_linha_b->prox;
                    }

                    if(aux_linha_b != nullptr){
                        Hash_colunas *hash_colunas_b = aux_linha_b->valores;
                        for(int m = 0; m < hash_colunas_b->n_buckets; m++){
                            p_no_coluna aux_valor_b = hash_colunas_b->tabela[m];
                            while (aux_valor_b != nullptr)
                            {
                                int produto = aux_valor_a->valor * aux_valor_b->valor;
                                int valor_c = c->acharElemento(aux_valor_a->i, aux_valor_b->j);
                                c->inserir(valor_c + produto, aux_valor_a->i, aux_valor_b->j);
                                aux_valor_b = aux_valor_b->prox;
                            }
                        }
                    }
                    aux_valor_a = aux_valor_a -> prox;
                }
            }
            aux_linha_a = aux_linha_a->prox;
        }
    }
}


struct MatrizComTransposta{
    Matriz* normal;
    Matriz* transposta;

    MatrizComTransposta(){
        normal = new Matriz(11);
        transposta = new Matriz(11);
    }

    ~MatrizComTransposta() {
        delete normal;
        delete transposta;
    }

    void inserir(int valor, int i, int j){
        normal->inserir(valor, i, j);
        transposta->inserir(valor, j, i);
    }

    int acharElemento(int i, int j){
        return normal->acharElemento(i, j);
    }

    Matriz* getTransposta(){
        return this->transposta;
    }

    void multiplicarEscalar(int escalar){
        normal->multiplicaEscalar(escalar);
        transposta->multiplicaEscalar(escalar);
    }
};

#endif