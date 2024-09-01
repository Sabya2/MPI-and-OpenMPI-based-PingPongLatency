// module load intel/2022.2 openmpi/intel/4.1.4 (with intel compiler)
// mpicc -o pingPongV3_openMPI_intel.exe pingPongV3.c
// module load gcc/9.3.0 openmpi/gcc.9/4.1.4  (with GNU compiler)
// mpicc -lm -o pingPongV3_openMPI_gnu.exe pingPongV3.c

// module load intel/2022.2 impi/2021.6   (intel compiler)
// mpiicc -o pingPongV3_intelMPI_intel.exe pingPongV3.c
// <./pingPongV3_intelMPI_intel.exe>

// module load gcc/9.3.0 impi/2021.6    (GNU compiler)
// mpigcc -lm -o pingPongV3_intelMPI_gnu.exe pingPongV3.c
// <./pingPongV3_intelMPI_gnu.exe>

#include <stdio.h>
#include <mpi.h>	
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

void parse_arguments(int argc, char **argv, int *enable_extra_output, int *key_arr, int *value_arr) {
    char *delimiter = "=";
    char *key, *value;
    int index = 1;
    *enable_extra_output = 0;
    printf("parse_arguments function\n -- checking Flags for data Transfer pattern\n -- creating the (base,power) pairs\n");
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {  // Check for flags
            if (strcmp(argv[i], "-A") == 0) {
                *enable_extra_output = 1;
                index = 2;} 
            else { printf("Invalid flag: %s\n", argv[i]);}
        }
        else {  // Process key-value pairs 
            key = strtok(argv[i], delimiter);
            value = strtok(NULL, delimiter);

            if (key && value) {
                printf(" ----- Data Packet for base:%s^ power:%s\n", key, value);
                value_arr[index] = atoi(value);
                key_arr[index] = atoi(key);
                index++; } 
            else { printf("Process %d: Invalid argument: %s\n", i, argv[i]);}
        }
    }
}

size_t *generateNumberArray(int base, int length){
    int i;
    size_t *numbers = (size_t*)malloc(length * sizeof(size_t));
    if (numbers == NULL) {
        fprintf(stderr, "Memory allocation failed - generateNumberArray.\n");
        exit(1);}
    for (i = 0; i < length; i++) { numbers[i] = pow(base, (i)); }
    // printf("Generated Number array -base_%d with %d elements\n", base, length);
    return numbers;
}

size_t *combineArrays(size_t *arr1, int size1, size_t *arr2, int size2, int *newSize) {
    *newSize = size1 + size2;
    size_t* combinedArray = (size_t*)malloc((size1 + size2) * sizeof(size_t));
    if (combinedArray == NULL) {
        fprintf(stderr, "Memory allocation failed - combineArrays .\n");
        exit(1);}
    int i;
    for (i = 0; i < size1; i++) { combinedArray[i] = arr1[i]; } //first array into combined array 
    for (i = 0; i < size2; i++) { combinedArray[size1 + i] = arr2[i]; } //second array into combined array   
    return combinedArray;
}

void selectionSort(size_t *combinedArray, int combined_size) {
    int i, j, minIndex;
    for (i =0; i < combined_size-1; i++) {
        minIndex = i;
        for (j = i+1; j < combined_size; j++) {
            if (combinedArray[j] < combinedArray[minIndex]) { minIndex = j;}
        }
        int temp = combinedArray[i];
        combinedArray[i] = combinedArray[minIndex];
        combinedArray[minIndex] = temp;
    }
}

void createCsvFile(int rows, int cols, double array[rows][cols], const char *filename, int id, char *transferCase ) {
    int i, j;
    char pattern[5];
    FILE *csvFile = fopen(filename, "w"); //pointer to csv file and file opening 
    if (csvFile == NULL) {
        printf("Failed to create CSV file\n");
        return; }
    // csv file row headers and the loops runs through rows --> columns 
    fprintf(csvFile, "ProcessID,Iterations,TransferPattern,BytesData(B),GigaBytesData(GB),StartTime,TransitTime,EndTime,SenderProcID\n");
    // fprintf(csvFile, "ProcessID,Iterations,TransferPattern,BytesData(B),SendTime,ReceiveTime,RoundTripTime\n");
        for (i = 0; i<rows; i++){
            fprintf(csvFile, "%d,%d,%s,", id, (i+1), transferCase);
            for (j=0; j<cols; j++) {
                // fprintf(csvFile, "%.15E", array[i][j]);
                fprintf(csvFile, "%.16e", array[i][j]);
                if (j < cols - 1) { fprintf(csvFile, ","); }
            } fprintf(csvFile, "\n"); // Add a newline character to end the row
        } fclose(csvFile);
    }

void InnerTransferLoop(int ierr, uint8_t *transfer_array, uint8_t *rcv_array, int procid, int len, int iteration, char *transferCase) {
    int row = iteration, col = 6, cases = 1;
    double arr_p0[row][col], arr_p1[row][col]; 
    double start_time, transit_time, end_time;

    for (int inner = 0; inner < iteration; inner++) {

        arr_p0[inner][0] = arr_p1[inner][0] = (double)len * sizeof(uint8_t);
        arr_p0[inner][1] = arr_p1[inner][1] = (double)len * sizeof(uint8_t) / (double)pow(1024, 3);
        
        if (transferCase == "B2A") { arr_p1[inner][5] = arr_p0[inner][5] = 1.0; }
        else if (transferCase == "A2B") { arr_p1[inner][5] = arr_p0[inner][5] = 0.0; }
        MPI_Barrier(MPI_COMM_WORLD);
        // printf("process - %d, transferCase - %s\n", procid, transferCase);
        
        if (procid == 1){ 
            MPI_Status status;

            if (transferCase == "B2A"){ // procid 1 is sender 
                // double sender = 1.0; 
                start_time = MPI_Wtime();
                ierr = MPI_Send(transfer_array, len, MPI_BYTE, 0, 4, MPI_COMM_WORLD);
                transit_time = MPI_Wtime();
                ierr = MPI_Recv(rcv_array, len, MPI_BYTE, 0, 3, MPI_COMM_WORLD, &status);
                end_time = MPI_Wtime();
    
                // double send_time = transit_time - start_time;
                // double receive_time = end_time - transit_time;
                // double round_trip_time = end_time - start_time;
            }
            else if (transferCase == "A2B"){ // procid 1 is receiver
                start_time = MPI_Wtime();
                ierr = MPI_Recv(rcv_array, len, MPI_BYTE, 0, 3, MPI_COMM_WORLD, &status);
                transit_time = MPI_Wtime();
                ierr = MPI_Send(rcv_array, len, MPI_BYTE, 0, 4, MPI_COMM_WORLD);
                end_time = MPI_Wtime();
            }
            arr_p1[inner][2] = start_time;
            arr_p1[inner][3] = transit_time;
            arr_p1[inner][4] = end_time;
        } 
        else if (procid == 0) { // Receive and transfer //pattern 10 10 
            MPI_Status status;
            if (transferCase == "B2A"){ // procid 0 is receiver 
                start_time = MPI_Wtime();
                ierr = MPI_Recv(rcv_array, len, MPI_BYTE, 1, 4, MPI_COMM_WORLD, &status);
                transit_time = MPI_Wtime();
                ierr = MPI_Send(rcv_array, len, MPI_BYTE, 1, 3, MPI_COMM_WORLD);
                end_time = MPI_Wtime();
                // arr_p0[inner][5] = 1.0;
            }
            else if (transferCase == "A2B"){ // procid 0 is sender
                start_time = MPI_Wtime();
                ierr = MPI_Send(rcv_array, len, MPI_BYTE, 1, 3, MPI_COMM_WORLD);
                transit_time = MPI_Wtime();
                ierr = MPI_Recv(rcv_array, len, MPI_BYTE, 1, 4, MPI_COMM_WORLD, &status);
                end_time = MPI_Wtime();
                // arr_p0[inner][5] = 0.0;
            }
            arr_p0[inner][2] = start_time;
            arr_p0[inner][3] = transit_time;
            arr_p0[inner][4] = end_time;
        }
        // printf("process - %d, transferCase - %s, start_time -> %E, end_time -> %E\n", procid, transferCase, start_time, end_time);
        MPI_Barrier(MPI_COMM_WORLD);
    }
    char filename0[100], filename1[100];  
    if (procid == 0){
        snprintf(filename0, sizeof(filename0), "sizeT_Packet%d_Node0_case%s.csv", len, transferCase );
        createCsvFile(row, col, arr_p0, filename0, procid, transferCase);
        printf(" ---- csv created - transferCase: %s, procID: %d, \n", transferCase, procid);
        }
    else if(procid == 1){
        snprintf(filename1, sizeof(filename1), "sizeT_Packet%d_Node1_case%s.csv", len, transferCase );
        createCsvFile(row, col, arr_p1, filename1, procid, transferCase);
        printf(" ---- csv created - transferCase: %s, procID: %d, \n", transferCase, procid);
        }  
    }

void InnerTransferLoopCases(int cases, int ierr, uint8_t *transfer_array, uint8_t *rcv_array, int procid, int len, int iteration) {
        if (cases == 0){ // default pattern of A2B when no flag is given
            InnerTransferLoop(ierr, transfer_array, rcv_array, procid, len, iteration, "A2B");
            printf("--------------- performed A2B \n\n");
            }
        else if (cases == 1) { // performs B2A when -A flag is given
            printf(" - case of asymmetrictity \n");
            InnerTransferLoop(ierr, transfer_array, rcv_array, procid, len, iteration, "A2B");
            // B2A_InnerTransferLoop(ierr, transfer_array, rcv_array, procid, len, iteration);
            printf(" ------ Completed - transfercase: A2B, procID: %d ------ \n", procid);
            // A2B_InnerTransferLoop(ierr, transfer_array, rcv_array, procid, len, iteration);
            InnerTransferLoop(ierr, transfer_array, rcv_array, procid, len, iteration, "B2A");
            printf(" ------ Completed - transfercase: B2A, procID: %d ------ \n", procid);
            }
    }

int main(int argc, char **argv) {

    if (argc < 2){ printf("Usage: %s and no argument given \n", argv[0]); return 0; }

    int ierr, procid, numprocs, outter, inner, i, j, enable_extra_output, iteration = 1000, k;
    size_t *combinedArray = NULL;
    int key_arr[argc - 1], value_arr[argc - 1], size, combined_size = 0;

    ierr = MPI_Init(&argc, &argv);
    ierr = MPI_Comm_rank(MPI_COMM_WORLD, &procid);
    ierr = MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    if (numprocs < 2){ // Num of processes must be greater than 1
        fprintf(stderr, "Num of processes must be greater than 1 for %s program\n", argv[0]);
        MPI_Abort(MPI_COMM_WORLD, 1);}

    parse_arguments(argc, argv, &enable_extra_output, key_arr, value_arr);
    if (enable_extra_output == 1 ) {i = 2;} // refers to perform transfer from both sides
    else if (enable_extra_output == 0) {i = 1;}	// refers transfer from one side only, the default transfer pattern
    for (i ; i < argc; i++) {
        int base = key_arr[i];
        int length = value_arr[i];
        size_t *arr = generateNumberArray(base, length);
        combinedArray = combineArrays(combinedArray, combined_size, arr, length, &combined_size);
        free(arr);
    }
    selectionSort(combinedArray, combined_size);    
    printf("procId: %d ----> Sequentially Executed Functions\n ------ generateNumberArray -> combineArrays -> selectionSort\n ------ The combined length of sorted array: %zu\n", procid, combined_size);
    
    // size_t *transfer_array = (size_t* ) malloc(combinedArray[combined_size-1] * sizeof(size_t)); // maximum space allocated
    // size_t *rcv_array = (size_t* ) malloc(combinedArray[combined_size-1] * sizeof(size_t));	 // maximum space allocated
    uint8_t* transfer_array = (uint8_t* ) malloc(combinedArray[combined_size-1] * sizeof(uint8_t)); // maximum space allocated
    uint8_t* rcv_array = (uint8_t* ) malloc(combinedArray[combined_size-1] * sizeof(uint8_t));	 // maximum space allocated
    
    for (outter = 0; outter < combined_size; outter++) { // Runs through the each element of the combined array
        MPI_Barrier(MPI_COMM_WORLD);
        int len = combinedArray[outter]; 
        for ( j = 0; j < len; j++) { transfer_array[j] = (j+1) * 5;}
        printf("\n\n ======================== outterLoop - %d ==========================\n", outter+1);
        InnerTransferLoopCases(enable_extra_output, ierr, transfer_array, rcv_array, procid, len, iteration);  
    }
    free(transfer_array), free(rcv_array);  // moved out of the outter loop
    // printf("outside outterLoop malloc arrays are free\n");     
    MPI_Finalize();
    free(combinedArray);
}
