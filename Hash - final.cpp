//Integrantes:
//Isabel Cristina Marras Salles - RA 188790
//Victor Luigi Roquetto - RA 250955
//Rafael Feltrin Lamen Rocha - RA 276246

#include <iostream>
#include <vector>
#include <stdio.h> 

using namespace std;

typedef struct NoColuna *p_no_coluna;

struct NoColuna //representa o nó do elemento não nulo
{
    int i, j, valor; //armazena a posição i e j na matriz e o valor do elemento
    p_no_coluna prox; //ponteiro para o próximo no, criando uma lista ligada no caso de colisões

    NoColuna(int i, int j, int valor)
    {
        this->i = i;
        this->j = j;
        this->valor = valor;
        prox = nullptr;
    }
};

struct Hash_colunas //hash mais interno, onde os nós dos elementos não nulos serão armazenados (cada linha terá sua própria hash)
{
    int n_buckets; //guarda o numero total de buckets da linha
    int qtd_armazenada; //guarda o número de elementos não nulos de determinada linha que foram armazenados
    vector<p_no_coluna> tabela; //vetor com as buckets que representa a tabela hash, na qual são colocadas as listas ligadas dos nós com os elementos não nulos

    Hash_colunas(int tamanho) : n_buckets(tamanho), qtd_armazenada(0), tabela(tamanho, nullptr)
    {
    }

    ~Hash_colunas() //libera a memoria do hash_colunas
    {
        for (int i = 0; i < n_buckets; ++i)
        {
            p_no_coluna atual = tabela[i];
            while (atual != nullptr)
            {
                p_no_coluna temp = atual;
                atual = atual->prox;
                delete temp; 
            }
        }
    }

    int funcaoHash(int j) //função que calcula qual será a posição do elemento no hash
    {
        return (31 * j) % this->n_buckets;
    }

    p_no_coluna inserir_lista(p_no_coluna lista, int i, int j, int valor) //função para inserir o elemento na lista ligada da posição encontrada
    {
        if(valor == 0){ //se o valor do elemento a ser adicionado for 0:
            if(lista == nullptr){  //se a lista estiver vazia, como o elemento é nuloe não deve ser adicionado, retorna a lista, ou seja, nullptr
                return nullptr; 
            }
            if(lista->i == i && lista->j == j){ //se o primeiro elemento da lista tiver a mesma posição i,j, significa que queremos atualizar o valor dele para 0, e isso significa removê-lo da lista
                p_no_coluna proximo_no = lista->prox; //um nó auxilar proximo_no recebe o segundo elemento
                delete lista; //deletamos o primeiro elemento da lista, equivalente a mudar seu valor para 0 já que não queremos armazenar elementos nulos
                this->qtd_armazenada--; //diminui um da quantidade armazenada já que um elemento foi removido
                return proximo_no; //retorna o segundo nó que agora é o primeiro elemento da lista ligada
            }
            p_no_coluna aux = lista; //se não for o primeiro elemento da lista, é preciso qual a posição na lista e removê-lo; o aux recebe o primeiro elemento
            while(aux->prox != nullptr){ //enquanto o proximo não for null
                if(aux->prox->i == i && aux->prox->j == j){ //se o próximo tiver a mesma posição do elemento que queremos atualizar para 0:
                    p_no_coluna no_remover = aux->prox; //o no a ser removido é o próximo
                    aux->prox = no_remover->prox; //o proximo do atual recebe o proximo do elemento a ser removido
                    delete no_remover; //deleta o nó que teria valor atualizado para 0
                    this->qtd_armazenada--; //diminui a quantidade armazenada já que um nó foi removido 
                    return lista; //retorna a lista ligada agora sem o elelemento
                }
                aux = aux->prox;
            }
            return lista; //retornamos a lista que ou terá o elemento removido ou ficará da forma que estava inicialmente (se não tiver nenhum elemento com posição i j, não é preciso adicionar um novo nó já que o valor será 0)
        } //se o valor não for 0
        if (lista == NULL) //se a lista estiver vazia, o primeiro elemento da lista será um novo nó com o valor do elemento que queremos adicionar
        { 
            p_no_coluna novo = new NoColuna(i, j, valor); //cria um novo nó com valor do elemento a ser adicionado
            this->qtd_armazenada++; //aumenta a quantidade armazenada
            return novo; //retorna o novo nó já que ele é o novo elemento da lista
        } 
        p_no_coluna aux = lista; //se não for vazia, um nó auxiliar recebe o primeiro elemento da lista ligada
 
        if (aux->i == i && aux->j == j) //se aux tiver a posição igual ao do que queremos adicionar, então atualizamos o valor de aux e retornamos 
        {
            aux->valor = valor;
            return lista;
        }
        while (aux->prox != NULL) //se não for o primeiro, precisamos verificar se há algum nó com mesma posição i j, para sabermos se é preciso atualizar o valor ou adicionar um novo nó 
        {
            if (aux->prox->i == i && aux->prox->j == j) //se o próximo tiver a mesma posição, atualizamos o valor do próximo e retornamos a lista apenas com o valor do nó alterado
            {
                aux->prox->valor = valor;
                return lista;
            }
            aux = aux->prox; 
        } 
        p_no_coluna novo = new NoColuna(i, j, valor); //se nao tiver nenhum elemento com a mesma posição, aux estará apontando para o ultimo elemento da lista e precisamos criar um novo nó 
        aux->prox = novo; //o proximo do ultimo elemento será o novo nó com o valor a ser adicionado
        this->qtd_armazenada++; // a quantidade armazenada aumenta
        return lista; //retorna a lista com o novo nó
    }

    void rehash(bool encolher) //função para alterar o tamanho do vetor com as buckets caso estejam muito cheias (o que torna o número de colisões muito alto) ou muito vazias (espaço armazenado desperdiçado)
    {
        int antigo_n_buckets = this->n_buckets; //guarda o número antigo de buckets
        if(encolher == true){ //se encolher for verdadeiro, significa que tinha muito espaço não sendo utilizado e é necessário diminuir o tamanho do vetor (fizemos para garantir que se muitos valores fossem atualizados para 0, ou se muitas operações resultassem em valor 0 o espaço armazenado ainda fosse proporcional ao número de elementos não nulos)
            this->n_buckets = max(5, antigo_n_buckets/2); //diminui o número de buckets na metade (mas garante que não seja menor do que 5, para que mesmo com poucos elementos não haja tanta colisão nem tenha que imediatamente aumentar o número de buckets)
        }else{ //encolher é falso e é preciso aumentar o tamanho do vetor para diminuir o fator de carga (número de elementos armazenados / número de buckets) e consequentemente a probabilidade de colisão
            this->n_buckets = antigo_n_buckets * 2 + 1; //dobra o tamanho do número de buckets e soma um para manter o número ímpar (pode ajudar a diminuir colisões)
        }
        if (this->n_buckets == antigo_n_buckets) return; //evita um rehash desnecessário se o tamanho calculado for igual ao atual
       
        vector<p_no_coluna> tabela_nova(this->n_buckets, nullptr); //cria um vetor com o tamanho do novo número de buckets que foi calculado. Esse vetor será a nova tabela hash
        int n_hash;
        for (int i = 0; i < antigo_n_buckets; i++) //para cada posição do vetor antigo, passa pela lista ligada (se houver) e calcula a nova posição de cada elemento, o adicionando no inicio da lista da nova posição (movendo os nós existentes ao invés de criar novos nós)
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
        this->tabela = tabela_nova; //atualiza a tabela do hash para a nova tabela
    }

    void inserir(int valor, int i, int j) //função para inserir ou atualizar um elemento
    {
        int n = funcaoHash(j); //calcula qual a posição do hash na qual o elemento deve ser inserido
        int qntd_antiga = this->qtd_armazenada; //guarda a quantidade armazenada 
        this->tabela[n] = inserir_lista(this->tabela[n], i, j, valor); //insere na lista ligada da posição correspondente do vetor encontrada pela função de hash
        if (this->qtd_armazenada > qntd_antiga && this->qtd_armazenada > 0.75 * this->n_buckets) //se a quantidade atual for maior que a anterior(um novo nó foi criado e adicionado) e o fator de carga for maior que 75%, aumenta o tamanho da tabela
        {
            rehash(false);
        }
        if(this->n_buckets > 16 && this->qtd_armazenada < 0.25 * this->n_buckets){ //se o número de buckets for maior que 16 (para garantir que o tamanho da tabela não fique muito pequeno e evitar que diminua logo nas primeiras inserções) e o fator de carga for menor que 25% diminui o tamanho da tabela
            rehash(true); 
        }
    }

    int acharElemento(int i, int j) //função para encontrar qual o valor do elemento está na posição i j
    {
        int n = funcaoHash(j); //calcula em qual posiçào da tabela hash o elemento deve estar com base na função de hash
        p_no_coluna aux = this->tabela[n]; //aux recebe o primeiro elemento da lista ligada
        while (aux != nullptr) //percorre a lista para encontrar o elemento
        { 
            if (aux->i == i && aux->j == j) //se o no tiver posição igual a buscada retorna seu valor
            {
                return aux->valor;
            }
            aux = aux->prox;
        }
        return 0; //se chegou nessa linha significa que aux é null e não há elemento com essa posição, logo deve ser um elemento com valor nulo que não foi armazenado na tabela
    }
};

typedef struct NoLinha *p_no_linha;
struct NoLinha //estrutura para guardar os hashs de cada linha
{
    int i; //armazena qual a linha que representa
    Hash_colunas *valores; //o hash para todos os elementos com linha i
    p_no_linha prox; //a próxima linha (será uma lista ligada de hashs, um para cada linha i que tenha caído no mesmo bucket do hash principal)

    NoLinha(int i) //inicializador
    {
        this->i = i;
        this->prox = nullptr;
        this->valores = new Hash_colunas(5); //inicia o hash de cada linha com tamanho 5;
    }

    ~NoLinha() //liberar a memória
    {
        delete valores;
    }
};

struct Matriz //estrutura que representa a matriz apenas com os elementos não nulos
{
    int n_buckets; //armazena o número de buckets (essas são as buckets nas quais cairão as linhas)
    int qtd_armazenada; //quantidade de linhas que contém pelo menos um elemento (linhas não nulas)
    vector<p_no_linha> tabela_linhas; //tabela hash na qual ficarão as listas ligadas que representam as linhas 

    Matriz(int tamanho) : n_buckets(tamanho), qtd_armazenada(0), tabela_linhas(tamanho, nullptr) //inicializador
    {
    }

    ~Matriz() //deletar a memória
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

    int funcaoHash(int i) //calcula a posição no hash de acordo com a linha do elemento que será adicionado
    {
        return (31 * i) % this->n_buckets;
    }

    void inserir(int valor, int i, int j) //função para inserir um novo elemento
    {
        int n = funcaoHash(i); //calcula qual a posição do hash de linhas o elemento estára com base na posição i dele
        p_no_linha aux = tabela_linhas[n]; //o auxiliar recebe a primeira linha que está armazenada na tabela na posição encontrada pelo hash
        p_no_linha anterior = nullptr; //anterior inicia em null 


        while (aux != nullptr && aux->i != i) //encontra a linha buscada na lista ligada de linhas daquela bucket
        {
            anterior = aux;
            aux = aux->prox;
        }
        //nesse momento aux é igual a linha buscada (ou null se não tiver alguma com mesmo i) e anterior é igual a linha anterior se existir (não i - 1, mas a linha anterior naquela bucket)
        if(valor != 0){  //se o valor a ser adicionado for diferente de 0
            int qnt_antiga = this->qtd_armazenada; //qnt_antiga recebe o número de linhas não nulas que eram armazenadas anteriormente

            if(aux == nullptr){ //se aux for null, significa que o while acabou sem encontrar uma linha com i igual ao buscado, então uma nova deve ser criada
                aux = new NoLinha(i); //cria uma nova NoLinha que será referente a linha i e guardará todos os elementos com linha i em seu hash
                this->qtd_armazenada++; //incrementa a qtd armazenada, pois uma nova linha não nula foi adicionada

                if(anterior == nullptr){  //se anterior for null significa que naquela bucket ainda não tinha nenhuma linha sendo armazenada
                    tabela_linhas[n] = aux; //o primeiro elemento daquela bucket passa a ser aux
                }else{ //se não for o primeiro, então a nova linha será adicionada no final
                    anterior->prox = aux;
                }
            } 
            aux->valores->inserir(valor, i , j); //aux é então ou a nova linha criada ou a linha encontrada com mesmo i e na hash dessa linha deve ser adicionado o novo elemento

            if(this->qtd_armazenada > qnt_antiga && this->qtd_armazenada > 0.75 * this->n_buckets){ //verifica se o fator de carga está muito alto caso tenha havido a criação de uma nova linha e se estiver chama rehash para aumentar o tamanho do vetor tabela_linhas
                rehash(false);
            }

        } // se o valor a ser adicionada for 0 
        else{
            if (aux == nullptr) { //se aux for null significa que não linha ainda com esse i, mas como o valor é 0, não há nada a ser feito
                return;
            }
            aux->valores->inserir(valor, i, j); //se não for null, chama inserir a linha correspondente (a função inserir tratará o caso de verificar se é uma atualização de valor ou se não há nada a ser adicionado)
            if(aux->valores->qtd_armazenada == 0){ //se tiver sido uma atualização de valor, corre o risco de não existir mais nenhum elemento com posição i e o hash dessa linha deve ser deletado
                if(anterior == nullptr){ //se for null, significa que era a primeira linha, então a tabela na posição n deve apontar para o segundo elemento
                    tabela_linhas[n] = aux->prox;
                }
                else{ //se não for o primeiro, o anterior deve apontar para o próximo do que será removido
                    anterior->prox = aux->prox;
                }
                delete aux; //deleta o hash da linha i que agora é uma linha nula
                this->qtd_armazenada--; //diminui o número de linhas não nulas
                if (this->n_buckets > 16 && this->qtd_armazenada < 0.25 * this->n_buckets) { //verifica se o fator de carga é menor do que 25% (se o número de buckets for maior que 16 para evitar que fique muito pequeno ou que haja redução no início do código ao inserir os primeiros elementos)
                    rehash(true);  //chama rehash para diminuir o tamanho do vetor
                }
            }
        }  
    }

    void rehash(bool encolher) //função para alterar o tamanho do vetor com as buckets caso estejam muito cheias (o que torna o número de colisões muito alto) ou muito vazias (espaço armazenado desperdiçado)
    {
        int antigo_n_buckets = this->n_buckets; //guarda o número antigo de buckets
        if(encolher == true){ //se encolher for verdadeiro, significa que tinha muito espaço não sendo utilizado e é necessário diminuir o tamanho do vetor (fizemos para garantir que se muitos valores fossem atualizados para 0, ou se muitas operações resultassem em valor 0 o espaço armazenado ainda fosse proporcional ao número de elementos não nulos)
            this->n_buckets = max(5, antigo_n_buckets/2);  //diminui o número de buckets na metade (mas garante que não seja menor do que 5, para que mesmo com poucas linhas não nulas não haja tanta colisão nem tenha que imediatamente aumentar o número de buckets)
        }else{ //encolher é falso e é preciso aumentar o tamanho do vetor para diminuir o fator de carga (número de elementos armazenados / número de buckets) e consequentemente a probabilidade de colisão
            this->n_buckets = antigo_n_buckets * 2 + 1; //dobra o tamanho do número de buckets e soma um para manter o número ímpar (pode ajudar a diminuir colisões)
        }
        if (this->n_buckets == antigo_n_buckets) return; //evita um rehash desnecessário se o tamanho calculado for igual ao atual
        
        vector<p_no_linha> tabela_nova(this->n_buckets, nullptr); //cria um vetor com o tamanho do novo número de buckets que foi calculado. Esse vetor será a nova tabela hash para as linhas
        int n_hash;
        for (int i = 0; i < antigo_n_buckets; i++) // para cada posição da tabela antiga
        {
            p_no_linha aux = tabela_linhas[i]; //auxiliar recebe a primeira linha que está armazenada na posição i 
            while (aux != nullptr) //passa por todas as linhas que estão naquela posição do hash
            {
                p_no_linha prox = aux->prox; //o proximo recebe o proximo de aux
                n_hash = funcaoHash(aux->i); //calcula a nova posição para aquela linha que representa a linha i da matriz
                aux->prox = tabela_nova[n_hash]; //insere o aux no começo da nova posição calculada pela função hash (reaproveita o nó existente na memória, sem precisar fazer new/delete)
                tabela_nova[n_hash] = aux;
                aux = prox;
            }
        }
        this->tabela_linhas = tabela_nova; //troca a tabela para a nova tabela
    }

    int acharElemento(int i, int j) //função para encontrar um elemento com posição (i,j)
    {
        int n = funcaoHash(i); //calcula o hash para a linha i do elemento 
        p_no_linha aux = tabela_linhas[n]; //aux recebe o primeiro elemento da tabela na posição calculada pelo hash
        while (aux != nullptr) //percorre a lista ligada de linhas que colidiram no mesmo bucket
        {
            if (aux->i == i) //verifica se o nó atual corresponde à linha buscada (i)
            {
                return aux->valores->acharElemento(i, j); //chama achar_elemento da hash interna daquela linha (busca da coluna j)
            }
            aux = aux->prox;
        }
        return 0; //se nada for retornado, significa que não tem nenhum elemento com linha i, logo é um elemento nulo e retorna 0
    }

    void multiplicaEscalar(int escalar) //função que multiplica todos os elementos armazenados por um escalar
    {
        if(escalar == 0){
            this->limpar();
            return;
        }
        for (int i = 0; i < this->n_buckets; i++) //percorre todos as posições do hash externo
        {
            p_no_linha aux_linha = this->tabela_linhas[i]; //aux recebe a primeira linha que está armazenada na posição i do hash
            while (aux_linha != nullptr) //enquanto for diferente de null
            {
                Hash_colunas *hash_colunas = aux_linha->valores; //hash colunas recebe o hash da linha para a qual aux está apontando
                for (int k = 0; k < hash_colunas->n_buckets; k++) //percorre todas as buckets hash interno
                {
                    p_no_coluna aux_valor = hash_colunas->tabela[k]; //aux recebe o primeiro elemento da lista ligada de cada posição k do hash interno 
                    while (aux_valor != nullptr) //enquanto for difente de null
                    {
                        aux_valor->valor = aux_valor->valor * escalar; //multiplica o valor pelo escalar
                        aux_valor = aux_valor->prox;
                    }
                }
                aux_linha = aux_linha->prox;
            }
        }
    }

    void limpar(){ //usada caso todos os valores sejam multiplicados por um escalar 0
        for(int i = 0; i < this->n_buckets; i++){ //percorre todos os buckets do hash externo
            p_no_linha aux = this->tabela_linhas[i]; //recebe a primeira lista do bucket
            while(aux != nullptr){ //enquanto for diferente de null
                p_no_linha deletar = aux; //o no linha a ser deletado recebe aux
                aux = aux->prox; //aux recebe o prox
                delete deletar; //deleta o no linha
            }
            tabela_linhas[i] = nullptr; //posição i do hash externo agora aponta para null
        }
        this->qtd_armazenada = 0; //quantidade armazenada é 0
        rehash(true); //diminui o tamanho do hash externo;
    }
};

void somaMatrizes(Matriz* a, Matriz* b, Matriz* c) //função que soma duas matrizes a, b e adiciona a soma na nova matriz c
{
    for (int i = 0; i < a->n_buckets; i++) //percorre todas as buckets da hash externa da matriz A
    {
        p_no_linha aux_linha = a->tabela_linhas[i]; //aux recebe o primeiro elemento da bucket i, que representa uma linha
        while (aux_linha != nullptr) //enquanto for diferente de null (percorre todas as linhas que caíram no mesmo bucket)
        {
            Hash_colunas *hash_colunas = aux_linha->valores; //ponteiro para hash interno de aux
            for (int k = 0; k < hash_colunas->n_buckets; k++) //percorre todas as buckets do hash interno 
            {
                p_no_coluna aux_valor = hash_colunas->tabela[k]; //recebe o primeiro elemento da bucket interna k
                while (aux_valor != nullptr) //enquanto for diferente de null (percorre todos os elementos que caíram no mesmo bucket interno)
                { 
                    c->inserir(aux_valor->valor, aux_valor->i, aux_valor->j); //insere em c o valor do elemento que foi encontrado
                    aux_valor = aux_valor->prox;
                }
            }
            aux_linha = aux_linha->prox; 
        }
    }


    for (int j = 0; j < b->n_buckets; j++) //realiza o mesmo processo para percorrer a matriz b
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
                    int valor_c = c->acharElemento(aux_valor->i, aux_valor->j); //acha qual o valor que está em c na posição (i,j) dos elementos em b
                    c->inserir(valor_c + aux_valor->valor, aux_valor->i, aux_valor->j); //insere em c o valor antigo somado ao valor do elemento de b
                    aux_valor = aux_valor->prox;
                }
            }
            aux_linha = aux_linha->prox;
        }
    }
}


void multiplicarMatrizes(Matriz* a, Matriz* b, Matriz *c){ //função que multiplica duas matrizes A e B e coloca o resultado em C.
    for(int i = 0; i < a->n_buckets; i++){ //percorre todas as buckets da matriz A
        p_no_linha aux_linha_a = a->tabela_linhas[i]; //aux recebe o primeiro elemento da bucket i, que representa uma linha de A
        while (aux_linha_a != nullptr){ //enquanto for diferente de null (percorre todas as linhas que caíram no mesmo bucket)
            Hash_colunas *hash_colunas_a = aux_linha_a->valores; //ponteiro para hash interno de aux
            for (int k = 0; k < hash_colunas_a->n_buckets; k++) //percorre todas as buckets do hash interno 
            {
                p_no_coluna aux_valor_a = hash_colunas_a->tabela[k]; //recebe o primeiro elemento da bucket interna k
                while (aux_valor_a != nullptr) //enquanto for diferente de null (percorre todos os elementos que caíram no mesmo bucket interno)
                { // a fórmula utilizada como base foi Cij = Somatório de K = 1 até n de (Aik x Bkj)
                    int k_a = aux_valor_a->j; //k_a recebe o valor de j do elemento em A
                    int n_hash_linha_b = b->funcaoHash(k_a); //calcula em qual posição do hash externo de B estão os elementos com linha = k_a
                    p_no_linha aux_linha_b = b->tabela_linhas[n_hash_linha_b]; //aux recebe a primeira linha do hash externo de B que caiu na posição calculada pela função de hash

                    while(aux_linha_b != nullptr && aux_linha_b->i != k_a ){ //anda com o ponteiro até que aponte para a linha que buscamos ou até que seja null (que significa que a linha k_a é uma linha nula em B)
                        aux_linha_b = aux_linha_b->prox;
                    }

                    if(aux_linha_b != nullptr){  //se o aux for diferente de null significa que a linha k_a em B não é nula, então é preciso multiplicar todos seus elementos pelo elemento A(i, k_a)
                        Hash_colunas *hash_colunas_b = aux_linha_b->valores; //ponteiro para a tabela hash interna (colunas) da linha k_a em B
                        for(int m = 0; m < hash_colunas_b->n_buckets; m++){ //percorre todas as buckets do hash interno 
                            p_no_coluna aux_valor_b = hash_colunas_b->tabela[m]; //recebe o primeiro elemento da bucket
                            while (aux_valor_b != nullptr) //percorre todos os elementos que caíram na mesma bucket 
                            { 
                                int produto = aux_valor_a->valor * aux_valor_b->valor; //multiplica o valor do elemento de A pelo valor do elemento de B
                                int valor_c = c->acharElemento(aux_valor_a->i, aux_valor_b->j); //acha o valor que tem em C na posição i,j, sendo i a linha de A e j a coluna de B
                                c->inserir(valor_c + produto, aux_valor_a->i, aux_valor_b->j); //soma o produto ao valor encontrado e atualiza o valor em C
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


struct MatrizComTransposta{ //struct que armazena a matriz original e a matriz transposta
    Matriz* normal;
    Matriz* transposta;

    MatrizComTransposta(){ //inicializa ambas com 11 buckets externas
        normal = new Matriz(11);
        transposta = new Matriz(11);
    }

    ~MatrizComTransposta() { //deletor
        delete normal;
        delete transposta;
    }

    void inserir(int valor, int i, int j){ //insere em uma na posição i,j e na segunda na posição j,i 
        normal->inserir(valor, i, j);
        transposta->inserir(valor, j, i);
    }

    int acharElemento(int i, int j){ 
        return normal->acharElemento(i, j); 
    }

    Matriz* getTransposta(){ //retorna a matriz transposta que estava sendo criada simultaneamente a normal
        return this->transposta;
    }

    void multiplicarEscalar(int escalar){ //multiplica ambas as matrizes por escalar
        normal->multiplicaEscalar(escalar);
        transposta->multiplicaEscalar(escalar);
    }
};

int main() {
    MatrizComTransposta m; // Cria o "pacote"

    cout << "### Teste de Armazenamento Duplo ###" << endl;

    // 1. Insere dados
    m.inserir(100, 5, 10); // A(5, 10) = 100
    m.inserir(50, 2, 8);  // A(2, 8) = 50

    // 2. Imprime a matriz normal
    cout << "--- Matriz Normal ---" << endl;
    // Deve mostrar: (5, 10)=100 e (2, 8)=50

    // 3. Pega a transposta (em O(1))
    Matriz* p_transposta = m.getTransposta();

    // 4. Imprime a matriz transposta
    cout << "--- Matriz Transposta (armazenada) ---" << endl;
    // Deve mostrar: (10, 5)=100 e (8, 2)=50

    // 5. Acessa elementos
    cout << "Acessando A(5, 10): " << m.acharElemento(5, 10) << endl; // Deve ser 100
    cout << "Acessando A_T(10, 5): " << p_transposta->acharElemento(10, 5) << endl; // Deve ser 100
    
    return 0;
}
