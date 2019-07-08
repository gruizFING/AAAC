//Variables compartidas entre las rutinas de interrupcion
int empezo; //Indica si ya empezo la batalla
int pausado; //Indica si la batalla esta pausada

void interrupcion_keyboard()
{
	inicio:
	
		if (!empezo)
			empezo = 1; //Empieza la batalla
		else //Batalla empezada 
		{
			if (pausado)
				pausado = 0; //Saco la pausa
			else
			{
				pausado = 1; //Pongo pausa
				print("\n\n\t\tBATALLA EN PAUSA");
			}
		}
	
	__asm__ ("iret"); //Habilito las interrupciones nuevamente y retorno el control
	goto inicio;
}

