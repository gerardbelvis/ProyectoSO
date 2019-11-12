#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <mysql/mysql.h>
#include <pthread.h>

//funcions consultes
void consulta_1(MYSQL *conn, char buff2[512])
{
	// Estructura especial para almacenar resultados de consultas 
	MYSQL_RES *resultado;
	MYSQL_ROW row;
	char consulta[512];
	char resbuff2[512];
	//Consideramos las muertes totales los asesinatos + las muertes del personaje
	strcpy(consulta, "SELECT partida.duracion FROM partida, relacionJugPar WHERE relacionJugPar.idpar = partida.id AND (relacionJugPar.muertes+relacionJugPar.asesinatos) = ( SELECT MAX(relacionJugPar.muertes+relacionJugPar.asesinatos) FROM relacionJugPar)");
	
	
	int err = mysql_query(conn, consulta);
	if (err!=0) {
		sprintf (resbuff2,"Error al consultar datos de la base %u %s\n",
				 mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	resultado = mysql_store_result (conn);
	
	row = mysql_fetch_row (resultado);
	
	if (row == NULL)
		sprintf (resbuff2,"No se han obtenido datos en la consulta\n");
	else
		sprintf(resbuff2, row[0]);
	
	strcpy(buff2, resbuff2);
}		


void consulta_2(MYSQL *conn, char buff2[512], char jugador[20], int partida)
{
	// Estructura especial para almacenar resultados de consultas 
	MYSQL_RES *resultado;
	MYSQL_ROW row;
	char resbuff2[512];
	char partidax[10];
	sprintf (partidax,"%d",partida); //Pasar de un entero a un string
	
	char consulta[500];
	
	
	strcpy(consulta, "SELECT relacionJugPar.muertes FROM relacionJugPar WHERE relacionJugPar.idjug = '");
	strcat(consulta, jugador);
	strcat(consulta, "' AND relacionJugPar.idpar = '");
	strcat(consulta, partidax);
	strcat(consulta, "'");
	
	int err = mysql_query(conn, consulta);
	if (err!=0) {
		sprintf (resbuff2,"Error al consultar datos de la base %u %s\n",
				 mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	resultado = mysql_store_result (conn);
	
	row = mysql_fetch_row (resultado);
	
	if (row == NULL)
		sprintf (resbuff2,"No se han obtenido datos en la consulta\n");
	else
		sprintf(resbuff2, row[0]);
	
	strcpy(buff2, resbuff2);
}	


void consulta_3(MYSQL *conn, char buff2[512], char personaje[20], char fecha[20])
{
	MYSQL_RES *resultado;
	MYSQL_ROW row;
	char resbuff2[512];

	int cont = 0;
	char consulta[500];
	
	
	//select relacionJugPar.personajes from partida, relacionJugPar where relacionJugPar.idpar = partida.id and partida.fecha = '12/02/2019';
	
	
	//primer necessito una taula on surti el nom de tots els personatges que es van jugar en la data demanada
	strcpy(consulta, "SELECT relacionJugPar.personajes FROM partida, relacionJugPar WHERE relacionJugPar.idpar = partida.id AND partida.fecha = '");
	strcat(consulta, fecha);
	strcat(consulta, "'");
	
	int err = mysql_query(conn, consulta);
	if (err!=0) {
		sprintf (resbuff2,"Error al consultar datos de la base %u %s\n",
				 mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	resultado = mysql_store_result (conn);
	
	row = mysql_fetch_row (resultado);
	
	if (row == NULL)
		sprintf (resbuff2,"No se han obtenido datos en la consulta\n");
	else{
		while (row !=NULL) {
			if(strcmp(personaje, row[0])==0)
				cont = cont + 1;
			row = mysql_fetch_row (resultado);
		}
		
		
	}
	sprintf(resbuff2,"%d", cont);
	strcpy(buff2, resbuff2);
	
}


int AnadirCuenta (MYSQL *conn, char usuario[20], char contrasena[20])
{
	//Incluiremos mecanismos de acceso excluyente para garantizar que no haya problemas a la hora de modificar la base de datos
	//DEVUELVE 0 SI OK, -1 SI NO SE HA PODIDO CREAR
	
	MYSQL_RES *resultado;
	MYSQL_ROW row;
	char consulta[512];
	int err;
	
	sprintf(consulta, "INSERT INTO jugador(id, contraseña) VALUES ('%s','%s');", usuario, contrasena);
	err = mysql_query (conn, consulta);
	
	if(err!=0)
		return -1; //HA HABIDO ERROR AL ANADIR LA CUENTA
	else 
		return 0;
}



typedef struct{
	char nombre[20];
	int socket;
}Conectado;

typedef struct{
	Conectado conectados[100];
	int num;
}ListaConectados;

ListaConectados lista; //Declarada aqui para poder ser usada en el main y en todas las funciones

int Pon(ListaConectados *lista, char nombre[20], int socket)
{
	if(lista->num == 100)
		return 0;
	else{
		lista->conectados[lista->num].socket = socket;
		strcpy(lista->conectados[lista->num].nombre, nombre);
		lista->num = lista->num +1;
		return 1;
	}
}


int DameSocket(ListaConectados *lista, char nombre[20])
{
	int encontrado = 0;
	int i =0;
	while ((i<lista->num) && (encontrado==0))
	{
		if (strcmp(lista->conectados[i].nombre, nombre)==0)
			encontrado =1;
		else
			i++;
	}
	if(encontrado==1)
		return lista->conectados[i].socket;
	else
		return -1;
}

int DamePos(ListaConectados *lista, char nombre[20])
{
	int encontrado = 0;
	int i = 0;
	while ((i<lista->num) && encontrado == 0)
	{
		if (strcmp(lista->conectados[i].nombre, nombre) == 0)
			encontrado=1;
		else 
			i =i+1;
	}
	if (encontrado ==1)
		return i;
	else 
		return -1;	
}

int EliminaConectado(ListaConectados *lista, char nombre[20])
{
	int pos = DamePos(lista, nombre);
	if (pos == -1)
		return -1;
	else
	{
		for (int i = pos; i < lista->num-1; i++)
		{
			lista->conectados[i].socket = lista->conectados[i+1].socket;
			strcpy(lista->conectados[i].nombre, lista->conectados[i+1].nombre);
		}
		lista->num --;
		return 0;
	}
}

void DameConectados(ListaConectados *lista, char conectados[100]) 
{
	//esta funcion nos dara todos los conectados de la siguiente manera: num,nombre, socket, nombre, socket.....
	//donde num es el total de personas conectadas 
	int i;
	sprintf(conectados,"%d", lista->num);
	
	for (i =0; i<lista->num; i++)
	{
		sprintf(conectados,"%s,%s,%d", conectados, lista->conectados[i].nombre, lista->conectados[i].socket);
	}
}


void *AtenderCliente(void *socket) //recibimos por referencia y no sabemos que es un int
{
	char personaje[20];
	char fecha[20];
	char jugador[20];
	int partida;
	char usuario[20];
	char contrasena[20];
	char consulta[512];
	
	//Connexio basedades (hacerlo en una fucion aparte)!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	MYSQL *conn;
	int err;
	
	//Creamos una conexion al servidor MYSQL 
	conn = mysql_init(NULL);
	
	if (conn==NULL) {
		printf ("Error al crear la conexiï¿ƒï¾³n: %u %s\n", 
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	//inicializar la conexion
	conn = mysql_real_connect (conn, "localhost","root", "mysql", "bbdd1", 0, NULL, 0);
	if (conn==NULL) {
		printf ("Error al inicializar la conexiï¿ƒï¾³n: %u %s\n", 
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	
	
	
	int sock_conn = *(int*) socket; //estiramos del puntero para que nos traiga el numero
	
	char buff[512]; //peticion
	char buff2[512]; //respuesta
	int ret;
	
	//Bucle de atención al cliente
	int terminar = 0;
	while (terminar ==0)
	{
		ret=read(sock_conn,buff, sizeof(buff));
		printf ("Recibido\n");
		
		
		// Tenemos que a?adirle la marca de fin de string 
		// para que no escriba lo que hay despues en el buffer
		buff[ret]='\0';
		
		printf ("Se ha conectado: %s\n",buff);
		
		char *p = strtok( buff, "/");
		int codigo =  atoi (p);
		
		if (codigo==0)
		{
			terminar = 1;
		}
		else if (codigo==3)	
		{
			p = strtok( NULL, "/");
			
			strcpy (personaje, p);
			p = strtok( NULL, "/");
			
			strcpy (fecha, p);
			printf ("Codigo: %d Personaje: %s Fecha: %s\n", codigo, personaje, fecha);
			consulta_3(conn, buff2, personaje, fecha);
		}
		
		
		else if (codigo==2)
		{
			p = strtok( NULL, "/");
			
			strcpy (jugador, p);
			char *p = strtok( NULL, "/");
			
			partida = atoi(p);
			printf ("Codigo: %d Jugador: %s Partida: %d\n", codigo, jugador, partida);
			consulta_2(conn, buff2, jugador, partida);
		}
		
		
		else if (codigo ==1) //durada partida amb mes morts		
		{
			consulta_1(conn, buff2);
			strcpy(consulta, buff2);
		}
		
		else if (codigo ==5) //registrar usuari
		{
			p = strtok( NULL, "/");
			
			strcpy (usuario, p);
			p = strtok( NULL, "/");
			strcpy (contrasena, p);
			int ok = AnadirCuenta(conn, usuario, contrasena);
			sprintf(buff2, "%d", ok);
		}
		
		else if (codigo ==6) //Añadir a lista de conectados
		{
			p = strtok(NULL, "/");
			strcpy(usuario, p);//usuario = Conectado
			int ponConectado = Pon(&lista, usuario, sock_conn);
			char vectorCon[100];
			DameConectados(&lista, vectorCon);
			sprintf(buff2, "%d,%s", ponConectado,vectorCon);
			int j;
			for(j = 0; j < lista.num ; j++)
				write(lista.conectados[j].socket,buff2,strlen(buff2));
		}
		
		if (codigo != 0 && codigo !=6)
		{
			write (sock_conn,buff2, strlen(buff2));
		}		
		
	}
	// Se acabo el servicio para este cliente
	close(sock_conn); 
}


int main(int argc, char *argv[])
{
	int sock_conn, sock_listen;
	struct sockaddr_in serv_adr;
	
		
	// INICIALITZACIONS
	// Obrim el socket
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("Error creant socket");
	// Fem el bind al port
	
	memset(&serv_adr, 0, sizeof(serv_adr));// inicialitza a zero serv_addr
	serv_adr.sin_family = AF_INET;
	
	// associa el socket a cualquiera de las IP de la m?quina. 
	//htonl formatea el numero que recibe al formato necesario
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	// escucharemos en el port 9070
	serv_adr.sin_port = htons(9070);
	if (bind(sock_listen, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) < 0)
		printf ("Error al bind");
	//La cola de peticiones pendientes no podra ser superior a 4
	if (listen(sock_listen, 2) < 0)
		printf("Error en el Listen");
	
	
	int i;
	int sockets[100];
	pthread_t thread[100]; //estructura de un vector de 100 threads
	
	//bukle infinito
	for(;;){
		printf ("Escuchando\n");
		
		sock_conn = accept(sock_listen, NULL, NULL);
		printf ("He recibido conexi?n\n");
		//sock_conn es el socket que usaremos para este cliente
		
		sockets[i] =sock_conn;
		
		//crear thread y decirle lo que tiene que hacer
		pthread_create (&thread[i], NULL, AtenderCliente, &sockets[i]); //pasamos por referencia el socket; &thread (pasado por ref) guardara un num que es un identificador de thread

	}

}
