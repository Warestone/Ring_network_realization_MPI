#include <stdio.h>
#include <iostream>
#include "mpi.h"
using namespace std;

bool Right_Side_Check = false;						// left or right side way

int Operations(int Sender, int Receiver, int Proc_N) // Compute operations for sides and return side + value of operations
{	
	int Right_side = 0, Left_side = 0;
	if (Sender > Receiver)							
	{
		Right_side = (Proc_N - Sender) + (Proc_N - (Proc_N - Receiver));
		Left_side = Sender - Receiver;
	}
	else
	{
		Right_side = Receiver - Sender;
		Left_side = (Sender--) + (Proc_N - Receiver);
	}
	if (Right_side < Left_side)
	{
		Right_Side_Check = true;
		return Right_side;
	}
	else
	{
		Right_Side_Check = false;
		return Left_side;
	}
}

void DIRECTION( int Operations, int Proc_N, int Sender, int Receiver, int* Massive_Proc)
{
	int s = 0;
	if (Right_Side_Check)
	{
		for (int i = Operations; i > 0; i--)
		{
			if (Sender == Proc_N)
			{
				Sender = 0;
			}
			Massive_Proc[s] = Sender;
			Sender++;
			s++;
		}
	}
	else
	{
		for (int i = Operations; i > 0; i--)
		{
			if (Sender == Proc_N)
			{
				Sender = 0;
			}
			Massive_Proc[s] = Sender;
			Sender--;
			s++;
		}
	}
}

int main(int argc, char* argv[])
{
	int Sender = atoi(argv[1]), Proc_N, Receiver = atoi(argv[2]), Current_Rank, Operations_Val = 0;
	double W_Time_Start, W_Time_End;
	int Message = 0;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &Proc_N);
	MPI_Comm_rank(MPI_COMM_WORLD, &Current_Rank);
	if (Proc_N < 2) // Protection conditions
	{
		if (Current_Rank == 0)
		{
			cout << "Too less threads for ring-network realization!\n\nMust be >= 2!" << endl;
		}
		MPI_Finalize();
		return 0;
	}
	if (Sender > Proc_N)
	{
		if (Current_Rank == 0)
		{
			cout << "Sender-thread must be less than total threads!" << endl;
		}
		MPI_Finalize();
		return 0;
	}
	if (Receiver > Proc_N)
	{
		if (Current_Rank == 0)
		{
			cout << "Receiver-thread must be less than total threads!" << endl;
		}
		MPI_Finalize();
		return 0;
	}
	if (Sender == Proc_N)
	{
		if (Current_Rank == 0)
		{
			cout << "Sender-thread dont be a receiver thread in same time!" << endl;
		}
		MPI_Finalize();
		return 0;
	}
	else // If all protect conditions are passed
	{
		if (Current_Rank == Sender) // just start messages
		{
			cout << "\n\n\t\tRing-network realization programm. Timofeev E.V. 381708-2" << endl;
			cout << "\n\tTotal threads = " << Proc_N << "\n\tSender-thread = " << Sender << "\n\tReceiver-thread = " << Receiver << "\n\n";					
		}
		Operations_Val = Operations(Sender, Receiver, Proc_N) + 1;												// value of operations
		int* Massive_Proc = new int[Operations_Val];
		DIRECTION(Operations_Val, Proc_N, Sender, Receiver, Massive_Proc);										// get a massive thread numbers (threads-senders/receivers)
		for (int i = 0; i < Operations_Val; i++)
		{
			if (Massive_Proc[i] == Current_Rank && Current_Rank == Sender)										// If Sender - only 1 send
			{
				Message = 200;
				MPI_Send(&Message, 1, MPI_INT, Massive_Proc[i + 1], 500, MPI_COMM_WORLD);
				cout << "\n\nThread = " << Current_Rank << "\nSending to " << Massive_Proc[i + 1];
				break;
			}
			if (Massive_Proc[i] == Current_Rank && Current_Rank == Receiver)									// If Receiver - only 1 receive
			{
				MPI_Recv(&Message, 1, MPI_INT, Massive_Proc[i - 1], MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				cout << "\n\nThread = " << Current_Rank << "\nReceiving from " << Massive_Proc[Operations_Val - 2] << "\nMessage: " << Message;
				break;
			}
			if (Massive_Proc[i] == Current_Rank && Current_Rank != Sender && Current_Rank != Receiver)			// All procces without sender & receiver - send & receive a message
			{
				MPI_Recv(&Message, 1, MPI_INT, Massive_Proc[i - 1], MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				MPI_Send(&Message, 1, MPI_INT, Massive_Proc[i + 1], 500, MPI_COMM_WORLD);
				cout << "\n\nThread = " << Current_Rank << "\nReceiving from " << Massive_Proc[i - 1] << "\nSending to " << Massive_Proc[i + 1];
				break;
			}								
		}
		MPI_Finalize();
		return 0;
	}
	
}
