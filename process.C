// Definição de um processo

typedef struct {
    int PID;
    int arrivalTime;
    int CPUTime;
    int hasIO;
    int IORequestTime;
    int IOType;
    int IOIsRunning;
    int IOTimeLeft;
    int CPURunTime;
    int priority;
} process;

process initializeProcess(int PID, int config[]) {
    process p;
    p.PID = PID;
    p.arrivalTime = config[0];   // Tempo em que o processo chegou na fila de prontos
    p.CPUTime = config[1];       // Tempo necessário para processamento
    p.hasIO = config[2];         // 1 (Solicita I/O) ou 0 (não)
    p.IORequestTime = config[3]; // Tempo em que o processo solicitou operacao I/O 
    p.IOType = config[4];        // 1 (disco), 2(fita magnetica) ou 3(impressora)
    p.IOIsRunning=0;             // 0 (inativo) ou 1(ativo)
    p.CPURunTime=0;              // Tempo ja executado de CPU
    return p;
}
