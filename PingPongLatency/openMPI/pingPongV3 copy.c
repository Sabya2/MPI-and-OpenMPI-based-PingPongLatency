
// module load intel/2022.2 openmpi/intel/4.1.4 (with intel compiler)
// mpicc -o pingPongV3_openMPI_intel.exe pingPongV3.c
// <./pingPongV3.exe>

// module load gcc/9.3.0 openmpi/gcc.9/4.1.4  (with GNU compiler)
// mpicc -lm -o pingPongV3_openMPI_gnu.exe pingPongV3.c



#include <stdio.h>
#include <mpi.h>	
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

void parse_arguments(int argc, char **argv, int *enable_extra_output, int *key_arr, int *value_arr){
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
                index++;
            } 
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

    for (i = 0; i < length; i++) {
        numbers[i] = pow(base, (i)); }
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
    for (i = 0; i < size1; i++) { //first array into combined array
        combinedArray[i] = arr1[i];}
    for (i = 0; i < size2; i++) { //second array into combined array
        combinedArray[size1 + i] = arr2[i];}
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

void createCsvFile(int rows, int cols, double array[rows][cols], const char *filename, int id, int cases ) {
    int i, j;
    char pattern[5];
    FILE *csvFile = fopen(filename, "w"); //pointer to csv file and file opening 
    if (csvFile == NULL) {
        printf("Failed to create CSV file\n");
        return; }
    if (cases == 0) {strcpy(pattern, "A2B"); }
    else if (cases == 1) {strcpy(pattern, "B2A"); }
    // csv file row headers and the loops runs through rows --> columns 
    fprintf(csvFile, "ProcessID,Iterations,TransferPattern,BytesData(B),GigaBytesData(Gb),SendTime,ReceiveTime,RoundTripTime\n");
    // fprintf(csvFile, "ProcessID,Iterations,TransferPattern,BytesData(B),SendTime,ReceiveTime,RoundTripTime\n");
        for (i = 0; i<rows; i++){
            fprintf(csvFile, "%d,%d,%s,", id, (i+1), pattern);
            for (j=0; j<cols; j++) {
                // fprintf(csvFile, "%.15E", array[i][j]);
                fprintf(csvFile, "%E", array[i][j]);
                if (j < cols - 1) {
                    fprintf(csvFile, ",");
                }
            }
            fprintf(csvFile, "\n"); // Add a newline character to end the row
        }
    fclose(csvFile);
}

void A2B_InnerTransferLoop(int ierr, uint8_t *transfer_array, uint8_t *rcv_array, int procid, int len, int iteration){
    // int row = iteration, col = 4, cases = 0;
    int row = iteration, col = 5, cases = 0;
    double arr_p0[row][col], arr_p1[row][col]; 
    printf("-- A2B_InnerTransferLoop cases: %d, - procID: %d\n", cases, procid);
    // MPI_Barrier(MPI_COMM_WORLD);
    for (int inner = 0; inner < iteration; inner++) {
        MPI_Barrier(MPI_COMM_WORLD);
        if (procid == 0){//transfer and receive //pattern 10 10
            // printf(" ------ A2B -- procId: %d -> transfer and receive\n", procid);
            double starttime_send, transit_time, endtime_receive;
            MPI_Status status;
            starttime_send = MPI_Wtime();
            ierr = MPI_Send(transfer_array, len, MPI_BYTE, 1, 2, MPI_COMM_WORLD);
            transit_time = MPI_Wtime();
            ierr = MPI_Recv(rcv_array, len, MPI_BYTE, 1, 1, MPI_COMM_WORLD, &status);
            endtime_receive = MPI_Wtime();
            double send_time = transit_time - starttime_send;
            double receive_time = endtime_receive - transit_time;
            double round_trip_time = endtime_receive - starttime_send;
            arr_p0[inner][0] = (double)len * sizeof(uint8_t); // Transfer packet
            arr_p0[inner][1] = (double)len * sizeof(uint8_t) / pow(1024, 3); 
            arr_p0[inner][2] = send_time;
            arr_p0[inner][3] = receive_time;
            arr_p0[inner][4] = round_trip_time;
        } 
        else if (procid == 1) { // Receive and transfer //pattern 00 00 
            // printf(" ------ A2B -- procId: %d -> receive and transfer\n", procid);
            double starttime_receive, transit_time, endtime_send;
            MPI_Status status;
            starttime_receive = MPI_Wtime();
            ierr = MPI_Recv(rcv_array, len, MPI_BYTE, 0, 2, MPI_COMM_WORLD, &status);
            transit_time = MPI_Wtime();
            ierr = MPI_Send(rcv_array, len, MPI_BYTE, 0, 1, MPI_COMM_WORLD);
            endtime_send = MPI_Wtime();
            double receive_time = transit_time - starttime_receive;
            double send_time = endtime_send - transit_time;
            double round_trip_time = endtime_send - starttime_receive;
            arr_p1[inner][0] = (double)len * sizeof(uint8_t); // Transfer packet
            arr_p1[inner][1] = (double)len * sizeof(uint8_t) / (1024 * 1024 * 1024); 
            arr_p1[inner][2] = send_time;
            arr_p1[inner][3] = receive_time;
            arr_p1[inner][4] = round_trip_time;
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    char filename0[100], filename1[100];  
    if (procid == 0){
        snprintf(filename0, sizeof(filename0), "sizeT_Packet%d_Node0_case%d.csv", len, cases );
        createCsvFile(row, col, arr_p0, filename0, procid, cases);
        printf("csv created, with procId: %d\n", procid);
        }
    else if(procid == 1){
        snprintf(filename1, sizeof(filename1), "sizeT_Packet%d_Node1_case%d.csv", len, cases );
        createCsvFile(row, col, arr_p1, filename1, procid, cases);
        printf("csv created, with procId: %d\n", procid);
        }  
    
}

void B2A_InnerTransferLoop(int ierr, uint8_t *transfer_array, uint8_t *rcv_array, int procid, int len, int iteration) {
    // int row = iteration, col = 4, cases = 1;
    int row = iteration, col = 5, cases = 1;
    double arr_p0[row][col], arr_p1[row][col]; 
    // printf("-- B2A_InnerTransferLoop cases: %d, - procId: %d\n", cases, procid);
    // MPI_Barrier(MPI_COMM_WORLD);
    for (int inner = 0; inner < iteration; inner++) {
        MPI_Barrier(MPI_COMM_WORLD);
        if (procid == 1){ //transfer and receive //pattern 00 00
            // printf(" ------ B2A -- procId: %d -> transfer and receive\n", procid);
            double starttime_send, transit_time, endtime_receive;
            MPI_Status status;
            starttime_send = MPI_Wtime();
            // ierr = MPI_Send(transfer_array, len, MPI_INT, 0, 4, MPI_COMM_WORLD);
            ierr = MPI_Send(transfer_array, len, MPI_BYTE, 0, 4, MPI_COMM_WORLD);
            transit_time = MPI_Wtime();
            ierr = MPI_Recv(rcv_array, len, MPI_BYTE, 0, 3, MPI_COMM_WORLD, &status);
            endtime_receive = MPI_Wtime();
            double send_time = transit_time - starttime_send;
            double receive_time = endtime_receive - transit_time;
            double round_trip_time = endtime_receive - starttime_send;
            arr_p0[inner][0] = (double)len * sizeof(uint8_t); // Transfer packet
            arr_p0[inner][1] = (double)len * sizeof(uint8_t) / (double)pow(1024, 3);
            arr_p0[inner][2] = send_time;
            arr_p0[inner][3] = receive_time;
            arr_p0[inner][4] = round_trip_time;
        } 
        else if (procid == 0) { // Receive and transfer //pattern 10 10 
            // printf(" ------ B2A -- procId: %d -> receive and transfer\n", procid);
            double starttime_receive, transit_time, endtime_send;
            MPI_Status status;
            starttime_receive = MPI_Wtime();
            ierr = MPI_Recv(rcv_array, len, MPI_BYTE, 1, 4, MPI_COMM_WORLD, &status);
            transit_time = MPI_Wtime();
            ierr = MPI_Send(rcv_array, len, MPI_BYTE, 1, 3, MPI_COMM_WORLD);
            endtime_send = MPI_Wtime();
            double receive_time = transit_time - starttime_receive;
            double send_time = endtime_send - transit_time;
            double round_trip_time = endtime_send - starttime_receive;
            arr_p1[inner][0] = (double)len * sizeof(uint8_t); // Transfer packet
            arr_p1[inner][1] = (double)len * sizeof(uint8_t) / (double)(1024.0 * 1024.0 * 1024.0); 
            arr_p1[inner][2] = send_time;
            arr_p1[inner][3] = receive_time;
            arr_p1[inner][4] = round_trip_time;
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    char filename0[100], filename1[100];  
    if (procid == 0){
        snprintf(filename0, sizeof(filename0), "sizeT_Packet%d_Node0_case%d.csv", len, cases );
        createCsvFile(row, col, arr_p1, filename0, procid, cases);
        printf("csv created, with procId: %d\n", procid);
        }
    else if(procid == 1){
        snprintf(filename1, sizeof(filename1), "sizeT_Packet%d_Node1_case%d.csv", len, cases );
        createCsvFile(row, col, arr_p0, filename1, procid, cases);
        printf("csv created, with procId: %d\n", procid);
        }  
    
}

void InnerTransferLoopCases(int cases, int ierr, uint8_t *transfer_array, uint8_t *rcv_array, int procid, int len, int iteration) {
    // printf("InnerTransferLoopCases\n" );
        if (cases == 0){ // default pattern of A2B when no flag is given
            A2B_InnerTransferLoop(ierr, transfer_array, rcv_array, procid, len, iteration);
            printf("--------------- performed A2B ----------------\n----------------------------------------------\n");
            }
        else if (cases == 1) { // performs B2A when -A flag is given
            A2B_InnerTransferLoop(ierr, transfer_array, rcv_array, procid, len, iteration);
            // B2A_InnerTransferLoop(ierr, transfer_array, rcv_array, procid, len, iteration);
            printf("--------------- performed A2B - procID: %d ----------------\n----------------------------------------------\n", procid);
            // A2B_InnerTransferLoop(ierr, transfer_array, rcv_array, procid, len, iteration);
            B2A_InnerTransferLoop(ierr, transfer_array, rcv_array, procid, len, iteration);
            printf("--------------- performed B2A - procID: %d ----------------\n----------------------------------------------\n", procid);
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
    for (i ; i < argc; i++){
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
        printf("\n\n\n------------ outterLoop-%d, procID: %d ---------------------------\n", outter+1, procid);
        // printf("packet size %d*8->%d bytes\n",len, len * sizeof(size_t));   
        InnerTransferLoopCases(enable_extra_output, ierr, transfer_array, rcv_array, procid, len, iteration);  
    }
    free(transfer_array), free(rcv_array);  // moved out of the outter loop
    // printf("outside outterLoop malloc arrays are free\n");     
    MPI_Finalize();
    free(combinedArray);
}
