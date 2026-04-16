#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

void processoFilho();
void processoPai(pid_t pid);

int main(int arc, char **argv) { 
	
	// salva PID do processo de origem
	pid_t pai = getpid();

	//criação dos dois processos filhos
	pid_t f;
	for (int i = 0; i < 2; i++) {
		fflush(stdout);
		f = fork();
		if (f == 0) {
			processoFilho();
			break;
		} else if (f > 0){
			processoPai(f);
		} else return 1; //checa se não foi criado com sucesso
	}
	//espera por ambos os filhos para finalizar processo origem
	while(wait(NULL) > 0);
	//executa mensagem dependendo se é processo origem
	if (getpid() == pai) {
		printf("Processo pai finalizado!\n");
	}
	return 0;
}


void processoFilho() {
	printf("Processo filho %d criado\n", getpid());
}

void processoPai(pid_t pid) {
	printf("Processo pai criou %d\n", pid);
}
