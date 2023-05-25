
# Memória compartilhada, threads e IPC em C


Neste programa, é feita a utilização do [Fork](https://br.ccm.net/faq/10841-o-que-faz-um-fork)  para geração de um processo pai e três processos filhos, em que dois filhos consomem o valor de uma variável de memória compartilhada, iterando 30 vezes e decrementando o seu valor, com a utlização de operações de leitura para uma variável local e escrita na variável compartilhada. O outro processo filho, gera duas threads, que também tem o mesmo comportamento dos outros processos fihos. Após o fim dos processos, é enviada uma mensagem para o processo pai, que aguarda de forma bloqueante, o termino dos processos, para que a execução do programa possa ser finalizada.

## Aprendizados

Neste projeto, da disciplina de Sistemas Operacionais do CEFET/RJ, Pude exercitar alguns pontos muito importantes da programação. São eles:
- Exclusão mútua para acesso a região crítica
- Manipulação de variáveis compartilhadas 
- Utilização de semáforos POSIX
- Utilização de Threads
- Envio e recebimento de mensagens entre diferentes processos
- Concorrencia entre processos
