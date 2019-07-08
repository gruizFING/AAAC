//#include "dwarf_vs_nop.players.h"
#include "dwarf_vs_imp.players.h"

#define MAX_TURNOS 512 //Maximos de turnos por batalla

//Variables compartidas entre las rutinas de interrupcion
int empezo; //Indica si ya empezo la batalla
int pausado; //Indica si la batalla esta pausada

//Genera un numero positivo y aleatorio a partir de una semilla y lo devuelve
unsigned int aleatorio(unsigned int semilla);

void printTurno(unsigned int turn);

void printPosicion(unsigned int pos);

void printOperando(char* modo, unsigned short op);

void printEntero(unsigned int ent);

void interrupcion_timer()
{
	//Variables locales
	int clear = 0;
	unsigned int tics;
	unsigned int sem;
	unsigned short pc1, pc2;
	unsigned int turno;
	unsigned int inst;
	int i;
	
	unsigned char opc;  //Opcode
	unsigned char ma;   //Adressing mode for A
	unsigned char mb;   //Adressing mode for B
	unsigned short A;	
	unsigned short B;
	unsigned short dirA, dirB;
	unsigned int opa; //Operand A
	unsigned int opb; //Operand B
	
	char *modoA;
	char *modoB;
	
	short pointer;
	
	unsigned int core[4096];
	char gano1, gano2;
	
	empezo = 0;
	pausado = 0;
	
	inicio:
		
		if (empezo)
		{
			if (!clear) //Inicializar todo
			{
				//Inicializo el CORE
				for (i = 0; i < 4096; i++)
					core[i] = 0;
		
				sem = tics;
				sem = aleatorio(sem);
				pc1 = sem % 4096;
				sem = aleatorio(sem);
				pc2 = sem % 4096;
				
				for (i = pc1; i < PLAYER_LENGTH_X + pc1; i++)
					core[i] = player_x[i - pc1];
				pc1 += PLAYER_PC_OFFSET_X;
				
				for (i = pc2; i < PLAYER_LENGTH_Y + pc2; i++)
					core[i] = player_y[i - pc2];
				pc2 += PLAYER_PC_OFFSET_y;
				
				gano1 = 0;
				gano2 = 0;
				tics = 8;
				turno = 0;
				clear = 1;
			}

			if (!pausado)
			{
				tics++;
				if (tics == 9) //Medio segundo = 1 turno
				{
					tics = 0;
					turno++;
					
					clrscr();
					print("\t\t*** CORE ***\n\t\t*** WARS***\n\n\n");
					printTurno(turno);
					
					//Juega el 1
					print("\tJUGADOR 1 --> ");
					printPosicion(pc1);
					
					inst = core[pc1];
					
					//Decodifico la instruccion
					opc = inst >> 28;
					ma = (inst >> 26) & 3;
					mb = (inst >> 24) & 3;
					A = (inst >> 12) & 0xFFF;
					B = inst & 0xFFF;
					
					switch (ma)
					{
						case(0): //Inmediato '#'
						{	
							opa = A;
							*modoA = '#';
						}
						case(1): //Relativo
						{
							dirA = (pc1 + A) % 4096;
							opa = core[dirA];
							modoA = NULL;
						}
						case(3): //Indirecto '@'
						{
							pointer = (pc1 + opa) % 4096;
							dirA = (pointer + core[pointer]) % 4096;
							opa = core[dirA];
							*modoA = '@';
						}
					}
					
					switch (mb)
					{
						case(0): //Inmediato '#'
						{	
							opb = B;
							*modoB = '#';
						}
						case(1): //Relativo
						{
							dirB = (pc1 + B) % 4096;
							opb = core[dirB];
							modoB = NULL;
						}
						case(3): //Indirecto '@'
						{
							pointer = (pc1 + opb) % 4096;
							dirB = (pointer + core[pointer]) % 4096;
							opb = core[dirB];
							*modoB = '@';
						}
					}

					pc1 = (pc1 + 1) % 4096;
					
					//Ejecuto la instruccion
					switch (opc)
					{
						case (0): //DAT, el gladiador 1 pierde
						{
							gano2 = 1;
							print("DAT   ");
							printOperando(modoB, B);
						}
						case (1): //MOV, copia lo de A a B
						{	
							core[dirB] = opa;
							print("MOV ");
							printOperando(modoA, A);
							printOperando(modoB, B);
						}
						case (2): //ADD, suma lo de A a B
						{
							core[dirB] = opa + opb;
							print("ADD ");
							printOperando(modoA, A);
							printOperando(modoB, B);
						}
						case (3): //SUB, resta lo de A a B
						{
							core[dirB] = opb - opa;
							print("SUB ");
							printOperando(modoA, A);
							printOperando(modoB, B);
						}
						case (4): //JMP, salta a la direccion indicada por A
						{
							pc1 = opa;
							print("JMP ");
							printOperando(modoA, A);
						}
						case (5): //JMZ, salta a la direccion indicada por A si el operandoB es 0
						{
							if (opb == 0)
								pc1 = opa;
							print("JMZ ");
							printOperando(modoA, A);
							printOperando(modoB, B);
						}
						case (6): //DJZ, decrementa en 1 el operandoB y si queda en 0 se salta a la direccion indicada por A
						{
							opb--;
							core[dirB] = opb;
							if (opb == 0)
								pc1 = opa;
							print("DJZ ");
							printOperando(modoA, A);
							printOperando(modoB, B);
						}
						case (7): //CMP, compara opa y opb, si son distintos salta la siguiente instruccion
						{
							if (opa != opb)
								pc1 = (pc1 + 1) % 4096;
							print("CMP ");
							printOperando(modoA, A);
							printOperando(modoB, B);
						}
					}
						
					//Juega el 2
					print("\n\tJUGADOR 2 --> ");
					printPosicion(pc2);
					
					inst = core[pc2];
					
					//Decodifico la instruccion
					opc = inst >> 28;
					ma = (inst >> 26) & 3;
					mb = (inst >> 24) & 3;
					A = (inst >> 12) & 0xFFF;
					B = inst & 0xFFF;
					
					switch (ma)
					{
						case(0): //Inmediato '#'
						{	
							opa = A;
							*modoA = '#';
						}
						case(1): //Relativo
						{
							dirA = (pc2 + A) % 4096;
							opa = core[dirA];
							modoA = NULL;
						}
						case(3): //Indirecto '@'
						{
							pointer = (pc2 + opa) % 4096;
							dirA = (pointer + core[pointer]) % 4096;
							opa = core[dirA];
							*modoA = '@';
						}
					}
					
					switch (mb)
					{
						case(0): //Inmediato '#'
						{	
							opb = B;
							*modoB = '#';
						}
						case(1): //Relativo
						{
							dirB = (pc2 + B) % 4096;
							opb = core[dirB];
							modoB = NULL;
						}
						case(3): //Indirecto '@'
						{
							pointer = (pc2 + opb) % 4096;
							dirB = (pointer + core[pointer]) % 4096;
							opb = core[dirB];
							*modoB = '@';
						}
					}

					pc2 = (pc2 + 1) % 4096;
					
					//Ejecuto la instruccion
					switch (opc)
					{
						case (0): //DAT, el gladiador 2 pierde
						{
							gano1 = 1;
							print("DAT ");
							printOperando(modoB, B);
						}
						case (1): //MOV, copia lo de A a B
						{	
							core[dirB] = opa;
							print("MOV ");
							printOperando(modoA, A);
							printOperando(modoB, B);
						}
						case (2): //ADD, suma lo de A a B
						{
							core[dirB] = opa + opb;
							print("ADD ");
							printOperando(modoA, A);
							printOperando(modoB, B);
						}
						case (3): //SUB, resta lo de A a B
						{
							core[dirB] = opb - opa;
							print("SUB ");
							printOperando(modoA, A);
							printOperando(modoB, B);
						}
						case (4): //JMP, salta a la direccion indicada por A
						{
							pc2 = opa;
							print("JMP ");
							printOperando(modoA, A);
						}
						case (5): //JMZ, salta a la direccion indicada por A si el operandoB es 0
						{
							if (opb == 0)
								pc2 = opa;
							print("JMZ ");
							printOperando(modoA, A);
							printOperando(modoB, B);
						}
						case (6): //DJZ, decrementa en 1 el operandoB y si queda en 0 se salta a la direccion indicada por A
						{
							opb--;
							core[dirB] = opb;
							if (opb == 0)
								pc2 = opa;
							print("DJZ ");
							printOperando(modoA, A);
							printOperando(modoB, B);
						}
						case (7): //CMP, compara opa y opb, si son distintos salta la siguiente instruccion
						{
							if (opa != opb)
								pc2 = (pc2 + 1) % 4096;
							print("CMP ");
							printOperando(modoA, A);
							printOperando(modoB, B);
						}
					}
					
					//Me fijo si alguno gano, y si se llego al limite de turnos
					if (gano1 && !gano2)
					{
						print("\n\t\tEL JUGADOR 1 ES EL GANADOR!!\n\n");
						print("\tPresione cualquier tecla para empezar una nueva batalla");
						empezo = 0;
						clear = 0;
					}
					else if (gano2 && !gano1)
					{
						print("\n\t\tEL JUGADOR 2 ES EL GANADOR!!\n\n");
						print("\tPresione cualquier tecla para empezar una nueva batalla");
						empezo = 0;
						clear = 0;
					}
					else if ((gano1 && gano2) || turno = MAX_TURNOS) //Los dos perdieron en el mismo turno o se llego al limite de turnos
					{
						print("\n\t\tES UN EMPATE!!\n\n");
						print("\tPresione cualquier tecla para empezar una nueva batalla");
						empezo = 0;
						clear = 0;
					}
				}
			}
		}
		
	__asm__ ("iret"); //Habilito las interrupciones nuevamente y retorno el control
	goto inicio;
}


unsigned int aleatorio(unsigned int semilla)
{
	semilla ^= semilla << 1;
	semilla ^= semilla >> 3;
	semilla ^= semilla << 10;
	return semilla;
}

void printTurno(unsigned int turn)
{
	print("\tTURNO = ");
	printEntero(turn);
}

void printPosicion(unsigned int pos)
{
	print("POS = ");
	printEntero(pos);
}

void printOperando(char* modo, unsigned short op)
{
	if (modo != NULL) //Inmediato o Indirecto
		print(modo);
	if ((op & 0x0800) == 0x0800) //Tiene el bit 12 prendido, es negativo
	{	
		print("-");
		op = (~op + 1) & 0xFFF; //Complemento a dos, los primeros 12 bits
	}
	printEntero(op);
}

void printEntero(unsigned int ent)
{
	char digitos[5] = {0, 0, 0, 0, 0};
	if (ent < 10) //1 solo digito
		digitos[0] = '0' + ent;
	else if (ent < 100) //2 digitos
	{
		digitos[0] = '0' + (ent / 10);
		digitos[1] = '0' + (ent % 10);
	}
	else if (ent < 1000) //3 digitos
	{
		digitos[0] = '0' + (ent / 100);
		digitos[1] = '0' + (ent / 10) % 10;
		digitos[2] = '0' + (ent % 10);
	}
	else //4 digitos
	{
		digitos[0] = '0' + (ent / 1000);
		digitos[1] = '0' + (ent / 100) % 10;
		digitos[2] = '0' + (ent / 10) % 10;
		digitos[3] = '0' + (ent % 10);
	}
	print(digitos);
}


