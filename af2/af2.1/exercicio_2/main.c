#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>

void processoFilho();
void processoNeto();
void processoFinalizado();

int main(int argc, char** argv) {

	// salva PID do processo de origem
	pid_t pai = getpid(); 
	
	//criação dos dois processos filhos
	pid_t f;
	for (int i = 0; i < 2; i++) {
		fflush(stdout);
		f = fork();
		if (f == 0) {
			processoFilho();
			//criação dos processos netos para cada filho
			pid_t n;
			for (int j = 0; j < 3; j++) {
				fflush(stdout);
				n = fork();
				if (n == 0) {
					processoNeto();
					break;
				} else if (n < 0) return 1; //checa se não foi criado com sucesso
			}
			break;
		} else if (f < 0) return 1; //checa se não foi criado com sucesso
		
	}

	//filhos esperam pelos netos, e origem espera pelos filhos
	while(wait(NULL) > 0);
	//executa diferentes mensagens dependendo se é processo origem
	if (getpid() == pai) {
		printf("Processo principal %d finalizado\n", getpid());
	} else {
		processoFinalizado();
	}
	return 0;
}

void processoFilho() {
	printf("Processo %d, filho de %d\n", getpid(), getppid());
}


void processoNeto() {
	processoFilho();
	sleep(5);
}

void processoFinalizado() {
	printf("Processo %d finalizado\n", getpid());
}
