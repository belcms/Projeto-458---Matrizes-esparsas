# Implementação e Análise de Estruturas para Matrizes Esparsas

Este repositório contém a implementação de duas estruturas de dados distintas projetadas para o armazenamento e manipulação eficiente de **Matrizes Esparsas** em C++. O projeto foca na otimização de memória e tempo de processamento, comparando uma abordagem baseada em **Tabelas Hash** com uma baseada em **Árvores Balanceadas**.

## 📖 Introdução

Uma matriz esparsa é, por definição, uma matriz em que a maioria dos elementos é igual a zero.

Mais formalmente, uma matriz $A \in \mathbb{R}^{n \times m}$ é considerada esparsa se o número de elementos não nulos $k$ satisfaz a seguinte condição:

$$k \ll n \times m$$

Isto é, o número de elementos não nulos ($k$) é muito menor que o número total de posições possíveis ($n \times m$).

Matrizes esparsas aparecem frequentemente nos mais diversos campos da computação e matemática (como processamento de imagens, aprendizado de máquina e simulações físicas), por vezes representando processos reais que dependem de eficiência e otimização.

Este projeto visa, por meio da ótica experimental e teórica, implementar e comparar o uso de duas estruturas distintas que promovem o uso mais eficiente de memória ($O(k)$) e poder computacional em comparação à representação densa tradicional.

---

## 🏗️ Estruturas Implementadas

### Estrutura 1: Hash de Hashes (Hash Table)
Focada no acesso ultra-rápido e inserção em tempo constante esperado.

* **Arquitetura:** Utiliza uma abordagem hierárquica. Um **Hash Externo** mapeia os índices das linhas ($i$) para buckets. Cada nó de linha contém um **Hash Interno** que mapeia os índices das colunas ($j$) para os valores.
* **Gestão de Memória:** Implementa *Lazy Allocation* (alocação sob demanda) para criar linhas apenas quando necessário e *Garbage Collection* automática (linhas vazias são desalocadas imediatamente).
* **Transposta:** Utiliza um padrão *Proxy* (Flag). A operação é $O(1)$ pois cria uma visão lógica que inverte os índices de acesso, sem mover dados fisicamente.
* **Dinâmica:** Possui *Rehashing* automático (crescimento e encolhimento) baseado no Fator de Carga para garantir o acesso $O(1)$.

### Estrutura 2: Árvore Rubro-Negra (Balanced Tree)
Focada em operações ordenadas e robustez no pior caso.

* **Arquitetura:** Utiliza árvores binárias de busca balanceadas (Rubro-Negras) para armazenar os elementos não nulos.
* **Armazenamento Duplo:** Mantém simultaneamente duas árvores em memória: uma ordenada por $(i, j)$ e outra espelhada por $(j, i)$.
* **Transposta:** Graças à estratégia de armazenamento duplo, a operação de transposta é $O(1)$. Ela é realizada através da troca de ponteiros das raízes, retornando um novo objeto que inverte a árvore normal pela transposta já pré-calculada.

---

## 📊 Complexidade Assintótica

Comparativo das complexidades teóricas obtidas para cada operação, onde $k$ é o número de elementos não nulos:

| Operação | Estrutura 1 (Hash) | Estrutura 2 (Árvore) | Descrição |
| :--- | :--- | :--- | :--- |
| **Memória** | $O(k)$ | $O(k)$ | Espaço proporcional aos não nulos |
| **Acesso** $A[i,j]$ | $O(1)^*$ | $O(\log k)$ | Ler um valor |
| **Inserção** | $O(1)^*$ | $O(\log k)$ | Inserir ou atualizar valor |
| **Transposta** $A^T$ | $O(1)$ | $O(1)$ | Retornar a matriz transposta |
| **Soma** $A+B$ | $O(k_A + k_B)$ | $O((k_A + k_B) \log k_C)$ | Somar duas matrizes |
| **Mult. Escalar** | $O(k)$ | $O(k)$ | Multiplicar todos por $\alpha$ |
| **Multiplicação** | $O(k_A \cdot d_B)$ | - | Produto matricial $A \times B$ |

*\* Complexidade esperada/amortizada.*

---

## 🚀 Como Executar

### Pré-requisitos
* Compilador C++ moderno (g++, clang, etc.)

### Compilação
Compile os arquivos separadamente conforme a estrutura desejada:

**Para a Estrutura Hash:**
```bash
g++ main_hash.cpp -o matriz_hash
./matriz_hash
