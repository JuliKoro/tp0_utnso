#include "utils.h"


void* serializar_paquete(t_paquete* paquete, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}

int crear_conexion(char *ip, char* puerto)
{
	// addrinfo es utilizada para almacenar información sobre direcciones de red y parámetros relacionados con la conexión. 
	
	// @brief hints es una instancia de esta estructura que se utiliza para proporcionar sugerencias o restricciones al llamar a la función getaddrinfo
	struct addrinfo hints;

	struct addrinfo *server_info; // @brief puntero a una lista enlazada de addrinfo que cumplan con hints

	// Configurar hints
	memset(&hints, 0, sizeof(hints)); // pongo en 0 todos lo bytes de hints (memset llena un bloque de memoria con un valor específico)
	hints.ai_family = AF_INET; // familia de direcciones = IPv4
	hints.ai_socktype = SOCK_STREAM; // tipo de socket = TCP (conexión orientada)

	// Resuelve la IP y el puerto en un addrinfo (server_info) para crear o conectar un socket o enlazarse (bindear) a un IP 
	getaddrinfo(ip, puerto, &hints, &server_info); // llena server_info con hint
	// Podria hacer:
	// err = getaddrinfo(...) para tener un placeholder
	// Si err == 0 -> salio todo bien y server_info contiene una lista de posibles direcciones listas para usar
	// Si err != 0 -> algo falló (host inválido, puerto inválido, etc.)

	// Ahora vamos a crear el socket.
	// crea un socket y te devuelve un fd (un int >= 0) que se usa para operar ese socket.
	// si algo sale mal devuelve -1
	int socket_cliente = socket(server_info->ai_family, // dominio
								server_info->ai_socktype, // tipo
								server_info->ai_protocol); // protocolo

	// Ahora que tenemos el socket, vamos a conectarlo
	// connect: Le dice al socket del cliente que se conecte a una dirección de servidor.
	// socket_cliente: socket previamente creado con socket()
	// ai_addr: Dirección del servidor a la que querés conectarte (IP + puerto)
	// ai_addrlen: Tamaño de la dirección.
	connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);
	/* connect intenta establecer una conexión TCP con el servidor en la IP y puerto indicados.
	- Si todo va bien, connect devuelve 0.
	- Si falla, devuelve -1 y errno indica qué salió mal (por ejemplo, servidor no encontrado, conexión rechazada, etc).*/
	// connect es bloqueante por defecto: el programa se queda esperando hasta que la conexión se logra o falla.

	// Libero la memoria reservada anteriormente por getaddrinfo
	freeaddrinfo(server_info);

	return socket_cliente; // devuelvo el socket conectado
}

int handshake_cliente(int socket_cliente)
{
	size_t bytes;

	int32_t handshake = 1; // Se define un valor fijo (1) que el cliente usará para iniciar el handshake
	int32_t result; // int32_t: entero signado de 32 bits

	/* send: envia una estructura al servidor
	socket_cliente: es el socket ya conectado al servidor.
	&handshak: Se envía el entero handshake al servidor.
	sizeof(int32_t): se asegura de que se envíen exactamente 4 bytes.
	0: sin flags especiales. */
	bytes = send(socket_cliente, &handshake, sizeof(int32_t), 0);
	// Esto manda un mensaje inicial al servidor, como diciendo: "Quiero comunicarme".
	
	/* recv: espera una respuesta del servidor y la guarda
	&result: variable donde se guarda la respuesta
	MSG_WAITALL: flag que fuerza a que se reciban exactamente esos sizeof(int32_t) bytes antes de continuar (importante para datos binarios).
	*/
	bytes = recv(socket_cliente, &result, sizeof(int32_t), MSG_WAITALL);
	// Está esperando que el servidor responda con un código de estado.

	if (result == 0) {
    	return 0; // Handshake OK
	} else {
    	return -1;// Handshake ERROR
	}
}

void enviar_mensaje(char* mensaje, int socket_cliente)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}

void crear_buffer(t_paquete* paquete)
{
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

t_paquete* crear_paquete(void)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = PAQUETE;
	crear_buffer(paquete);
	return paquete;
}

void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}

void enviar_paquete(t_paquete* paquete, int socket_cliente)
{
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}

void eliminar_paquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}
