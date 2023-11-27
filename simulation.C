#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include "queue.C"
#include "process.C"


// Constantes definidas pelo grupo
#define NUMBER_OF_PROCESSES 2
#define DISC_TIME 2
#define MAGNETIC_TAPE_TIME 3
#define PRINTER_TIME 4




// Define filas a serem utilizadas
queue process_high_priority_queue;
queue process_low_priority_queue;
queue IO_disc_queue;
queue IO_magnetic_tape_queue;
queue IO_printer_queue;

// Array dos processos
process processArray[NUMBER_OF_PROCESSES]; 

// Variavel com o atual numero de processos e I/Os concluidos
int completed = 0;                              

/*// Le e armazena dados de processos e I/O dos arquivos de configuracao
int* getConfig(int confignum, int datasize){
    FILE* file;

    // Abre o file para leitura
    file = fopen("pconfig", "r");

    if (file == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo pconfig.\n");
        return NULL;
    }

    // Move para a linha correspondente a confignum
    for (int i = 1; i < confignum; i++) {
        if (fscanf(file, "%*[^\n]") == EOF) {
            fprintf(stderr, "Linha especificada não encontrada no arquivo.\n");
            fclose(file);
            return NULL;
        }
        fgetc(file); // Move para a próxima linha
    }

    // Aloca um array para armazenar os valores
    int* valores = (int*)malloc(datasize * sizeof(int));

    // Lê os valores da linha especificada
    for (int i = 0; i < datasize; i++) {
        if (fscanf(file, "%d", &valores[i]) != 1) {
            fprintf(stderr, "Erro ao ler os valores do arquivo.\n");
            free(valores);  // Libera a memória alocada antes de retornar
            fclose(file);
            return NULL;
        }
    }

    fclose(file);

    return valores;
}
*/
// Armazena processos da simulacao em um array
void initializeAllProcesses(){
    //for(int i=0; i<NUMBER_OF_PROCESSES; i++){
    //   processArray[i] = initializeProcess(i, getConfig(i, 3));
    //}
     int p1[] = {0, 3, 1, 2, 3};
     int p2[] = {1, 2, 0, 0, 0};
     processArray[0] = initializeProcess(0, p1);
     processArray[1] = initializeProcess(1, p2);
}

int checkNewIO(int PID){
    if(processArray[PID].hasIO == 1){
        if(processArray[PID].IORequestTime == processArray[PID].CPURunTime){
            int type = processArray[PID].IOType;
            printf("Processo %d requisitou IO\n", PID+1);
            switch(type){
                case 1:
                    push(&IO_disc_queue, PID+1);
                    printf("    adicionado na fila de IO: Disco\n");
                    return 1;
                case 2:
                    push(&IO_magnetic_tape_queue, PID+1);
                    printf("    adicionado na fila de IO: Fita Magnetica\n");
                    return 1;
                case 3:
                    push(&IO_printer_queue, PID+1);
                    printf("    adicionado na fila de IO: Impressora\n");
                    return 1;
            }
        }
    }
    return 0;
}

void checkNewProcesses(int clock){
    for(int i=0; i<NUMBER_OF_PROCESSES; i++){
        if(processArray[i].arrivalTime == clock){
            int PID = processArray[i].PID;
            if(checkNewIO(PID)==1){
                return;
            }
            printf("Processo %d chegou\n", PID+1);
            push(&process_high_priority_queue, PID);
            printf("    adicionado na fila de alta prioridade\n");
            processArray[i].priority=1;
        }
    }
}

int checkIORequest(int PID){
    if(processArray[PID].hasIO == 1){
        if(processArray[PID].IORequestTime == processArray[PID].CPURunTime){
            int type = processArray[PID].IOType;
            printf("Processo %d requisitou IO\n", PID+1);
            switch(type){
                case 1:
                    if(processArray[PID].priority==1){
                        push(&IO_disc_queue, pop(&process_high_priority_queue));
                        printf("    adicionado na fila de IO: Disco\n");
                        return 1;
                    }
                    push(&IO_disc_queue, pop(&process_low_priority_queue));
                    printf("    adicionado na fila de IO: Disco\n");
                    return 1;
                case 2:
                    if(processArray[PID].priority==1){
                        push(&IO_magnetic_tape_queue, pop(&process_high_priority_queue));
                        printf("    adicionado na fila de IO: Fita Magnetica\n");
                        return 1;
                    }
                    push(&IO_magnetic_tape_queue, pop(&process_low_priority_queue));
                    printf("    adicionado na fila de IO: Fita Magnetica\n");
                    return 1;
                case 3:
                    if(processArray[PID].priority==1){
                        push(&IO_printer_queue, pop(&process_high_priority_queue));
                        printf("    adicionado na fila de IO: Impressora\n");
                        return 1;
                    }
                    push(&IO_printer_queue, pop(&process_low_priority_queue));
                    printf("    adicionado na fila de IO: Impressora\n");
                    return 1;
            }
        }
    }
    return 0;
}

void runProcess(queue q){
    int PID = pop(&q);
    printf("Executando processo %d\n", PID+1);
    int newTime = processArray[PID].CPURunTime + 1;
    int targetTime = processArray[PID].CPUTime;
    if(newTime==targetTime){
        printf("    processo finalizado\n");
        completed++;
        return;
    }
    processArray[PID].CPURunTime = newTime;
    if(checkIORequest(PID)==1){
        return;
    }
    push(&process_low_priority_queue, PID);
    printf("    processo %d movido para fila de baixa prioridade\n", PID+1);
    processArray[PID].priority=0;
    return;
}

void runIO(queue q, int type){
    int PID = pop(&q);
    printf("Executando IO %d\n", PID+1);
    processArray[PID].IOIsRunning = 1;
    switch(type){
        case 1:
            processArray[PID].IOTimeLeft = DISC_TIME - 1;
        case 2:
            processArray[PID].IOTimeLeft = MAGNETIC_TAPE_TIME - 1;
        case 3:
            processArray[PID].IOTimeLeft = PRINTER_TIME - 1;
    }
}

void schedule(int clock){
    // Verifica se alguma operacao de IO esta sendo executada
    for(int i=0; i<NUMBER_OF_PROCESSES; i++){
        if(processArray[i].IOIsRunning == 1){
            int PID = processArray[i].PID;
            printf("Executando IO %d\n", PID+1);
            int newTime = processArray[i].IOTimeLeft - 1;
            processArray[i].IOTimeLeft = newTime; 
            if(processArray[i].IOTimeLeft == 0){
                int type = processArray[i].IOType;
                printf("    IO do processo %d terminou\n", PID+1);
                processArray[i].IOIsRunning = 0;
                processArray[i].hasIO = 0;
                switch(type){
                    case 1:
                        push(&process_low_priority_queue, pop(&IO_disc_queue));
                        printf("        processo %d movido para fila de baixa prioridade\n", PID+1);
                        processArray[PID].priority=0;
                    case 2:
                        push(&process_high_priority_queue, pop(&IO_magnetic_tape_queue));
                        printf("        processo %d movido para fila de alta prioridade\n", PID+1);
                        processArray[PID].priority=1;
                    case 3:
                        push(&process_high_priority_queue, pop(&IO_printer_queue));
                        printf("        processo %d movido para fila de alta prioridade\n", PID+1);
                        processArray[PID].priority=1;
                return;
                }
            }
        printf("IO do processo %d continuou sua execucao\n", PID+1);
        return;
        }
    }

    // Prioriza processo/IO
    if((isEmpty(&IO_printer_queue))){
        if(isEmpty(&IO_magnetic_tape_queue)){
            if(isEmpty(&IO_disc_queue)){
                if(isEmpty(&process_high_priority_queue)){
                        runProcess(process_low_priority_queue);
                        return;
                }
                runProcess(process_high_priority_queue);
                return;
            }
            int type = 1;
            runIO(IO_disc_queue ,type);
            return;
        }   
        int type = 2;
        runIO(IO_magnetic_tape_queue ,type);
        return;
    }
    int type = 3;
    runIO(IO_printer_queue ,type);
    return;

}


int main (){
    int clock = 0;                                     // Inicializa clock

    // Inicializa filas
    initQueue(&process_high_priority_queue);
    initQueue(&process_low_priority_queue);
    initQueue(&IO_disc_queue);
    initQueue(&IO_magnetic_tape_queue);
    initQueue(&IO_printer_queue);

    initializeAllProcesses();                          // Inicializa processos

    // Inicia simulacao de processameto
    while(completed != NUMBER_OF_PROCESSES){
        printf("ROUND %d ------------------------------------\n", clock);
        // Busca por novos processos
        checkNewProcesses(clock);
        // Busca por novos eventos de I/O
        checkNewIO(clock);
        // Decide qual processo será priorizado
        schedule(clock);
        clock++;
    }

    return 0;
}
