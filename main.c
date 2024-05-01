/*

This file is part of Prime numbers calculation using MPICH

Copyright 2024 Joaquín Cuéllar <joaquin.cuellar(at)uco(dot)es>

Prime numbers calculation using MPICH is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Prime numbers calculation using MPICH is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void share_Prime_Numbers(int * scatteredSizes, int * scatteredStrides, int size, long calcNumPrimeNumbers)
{
    if (size > 0 && calcNumPrimeNumbers > 0)
    {
        int added = 0;
        for (int i = 0; i < size; i++)
        {
            int addedToThis = 0;
            if ((calcNumPrimeNumbers - added) % size != 0)
                addedToThis = 1;
            scatteredSizes[i] = calcNumPrimeNumbers / size + addedToThis;
            if (i == 0)
                scatteredStrides[i] = 0;
            else
                scatteredStrides[i] = scatteredSizes[i - 1] + scatteredStrides[i - 1];
            added += addedToThis;
        }
    }
    return;
}

int is_Prime(long number, long *primesVector, int primesVectorSize)
{
    int isPrime = 0;
    for (int i = 0; i < primesVectorSize; i++)
    {
        if (number % primesVector[i] == 0)
            isPrime = 1;
    }
    return isPrime;
}

int main(int argc, char ** argv)
{

    
    if (argc != 2) 
    {
        fprintf(stderr, "Debes indicar el número de números primos a calcular: prime_numbers <cantidad de números primos a calcular>\n");
        exit(1);
    }

    int size, myRank;
    int numPrimeNumbers = atoi(argv[1]);
    if (numPrimeNumbers <= 5) 
    {
        fprintf(stderr, "Debes indicar una cantidad de números primos a calcular MAYOR de 5!\n");
        exit(1);
    }
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank( MPI_COMM_WORLD, &myRank);
    long *calculatedPrimeNumbers;
    int scatteredSizes[size];
    int scatteredStrides[size];

    if (myRank == 0)
    {
        calculatedPrimeNumbers = malloc(sizeof(long)*numPrimeNumbers);
        calculatedPrimeNumbers[0] = 2;
        calculatedPrimeNumbers[1] = 3;
        calculatedPrimeNumbers[2] = 5;
        calculatedPrimeNumbers[3] = 7;
        calculatedPrimeNumbers[4] = 11;
    }
    long calcNumPrimeNumbers = 5;
    long counter = 12;
    long * myPrimes;
    while (calcNumPrimeNumbers < numPrimeNumbers)
    {
        share_Prime_Numbers(&scatteredSizes[0], &scatteredStrides[0], size, calcNumPrimeNumbers);
        int localNumberOfPrimes;
        MPI_Scatter(scatteredSizes, 1, MPI_INT, &localNumberOfPrimes, 1, MPI_INT, 0, MPI_COMM_WORLD);
        myPrimes = malloc(sizeof(long)*localNumberOfPrimes);
        MPI_Scatterv(calculatedPrimeNumbers, scatteredSizes, scatteredStrides, MPI_LONG, myPrimes, localNumberOfPrimes, MPI_LONG, 
                                                              0, MPI_COMM_WORLD);
        MPI_Bcast( &counter, 1, MPI_LONG, 0, MPI_COMM_WORLD);
        int isPrime = is_Prime(counter, &myPrimes[0], localNumberOfPrimes);
        int reducedIsPrime;
        MPI_Reduce(&isPrime, &reducedIsPrime, 1, MPI_INT, MPI_SUM, 0,
           MPI_COMM_WORLD);
        free(myPrimes);
        if (myRank == 0)
        {
            printf("checking number %ld... ", counter);
            if (reducedIsPrime > 0)
            {
                calculatedPrimeNumbers[calcNumPrimeNumbers] = counter;
                calcNumPrimeNumbers++;
                printf("IT'S PRIME!\n");
            }
            else
                printf("IT IS NOT PRIME!\n");
        }
        counter++;
    }
    if (myRank == 0)
    {
        printf("the prime numbers calculated are: ");
        for (long j = 0; j++; j < calcNumPrimeNumbers)
            printf("%ld, ", calculatedPrimeNumbers[j]);
        free(calculatedPrimeNumbers);
    }
    MPI_Finalize();

    
    return 0;
}