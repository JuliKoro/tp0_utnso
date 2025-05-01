/**
 * @file utils.c
 * @author sisoputnfrba
 * @author JuliKoro
 * @date 21 Mar 2019
 * @brief codigo fuente de las "utilities" para el cliente
 *
 * Se usa para agrupar funciones genéricas o reutilizables, como:
 * - Envío y recepción segura de datos
 * - Funciones de impresión o logging
 * - Validación de argumento
 * - Serialización/deserialización
 * - Funciones de handshake
 * - Manejo de errores
 * @see https://docs.utnso.com.ar/primeros-pasos/tp0
 * @see https://docs.utnso.com.ar/guias/linux/sockets
 */

#include "utils.h"

void* serializar_paquete(t_paquete* paquete, int bytes)
{
	// Reservo un bloque de memoria del tamaño total calculado (bytes), para guardar todo el contenido serializado.
	void * magic = malloc(bytes);
	// contador que indica cuántos bytes ya hemos copiado, para ir armando el bloque paso a paso.
	int desplazamiento = 0;

	// Copiar el código de operación
	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);

	// Copiar el tamaño del buffer
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);

	// Copiar el contenido real
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}

int crear_conexion(char *ip, char* puerto)
{
	// addrinfo es utilizada para almacenar información sobre direcciones de red y parámetros relacionados con la conexión. 
	
	// hints es una instancia de esta estructura que se utiliza para proporcionar sugerencias o restricciones al llamar a la función getaddrinfo
	struct addrinfo hints;

	struct addrinfo *server_info; // puntero a una lista enlazada de addrinfo que cumplan con hints

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
	// Se crea una estructura dinamica en memoria que representa un mensaje para enviar.
	t_paquete* paquete = malloc(sizeof(t_paquete));

	// Se setea y configura el paquete a enviar
	paquete->codigo_operacion = MENSAJE; // tipo de paquete: MESNAJE ( indica que el contenido es un mensaje de texto)
	paquete->buffer = malloc(sizeof(t_buffer)); // Se crea un buffer dentro del paquete (t_buffer es otra estructura que almacena el tamaño y los datos reales) 
	paquete->buffer->size = strlen(mensaje) + 1; // Se calcula el tamaño del mensaje, incluyendo el \0 de fin de cadena
	paquete->buffer->stream = malloc(paquete->buffer->size); // Se reserva memoria para el contenido
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size); // Se copia el mensaje dentro del buffer (stream) del paquete.

	// Se calcula el tamaño total del paquete serializado:
	int bytes = paquete->buffer->size + 2*sizeof(int); // los bytes del msj + (1 int para buffer->size + 1 int cod_op)

	// Se convierte el t_paquete en una secuencia lineal de bytes (en formato raw) lista para enviarse por socket.
	void* a_enviar = serializar_paquete(paquete, bytes);

	// Se envía el bloque completo de datos al socket.
	send(socket_cliente, a_enviar, bytes, 0); // lo que el servidor recibe y debe deserializar

	// Se libera la memoria asignada
	free(a_enviar); // el paquete serializado
	eliminar_paquete(paquete); // el paquete en si y sus componentes 
}

void crear_buffer(t_paquete* paquete)
{
	paquete->buffer = malloc(sizeof(t_buffer)); // Reserva memoria dinámica para un t_buffer, para guardar datos a enviar
	paquete->buffer->size = 0; // Inicializa el tamaño del buffer en 0 porque todavía no se ha agregado ningún dato
	paquete->buffer->stream = NULL; // Inicializa el puntero a NULL, lo cual indica que aún no hay contenido cargado
}

t_paquete* crear_paquete(void)
{
	t_paquete* paquete = malloc(sizeof(t_paquete)); // Reserva memoria en el heap para un nuevo paquete.
	paquete->codigo_operacion = PAQUETE; //  le dice a quien reciba este paquete que es un "paquete"
	crear_buffer(paquete); // inicializar el buffer del paquete con tamaño 0 y sin datos aún
	return paquete; // Devuelve el puntero al paquete creado, ya listo para usarse
}

void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio)
{
	//Expandir el stream
	// realloc amplía el bloque de memoria existente para que quepa el nuevo dato
	// Suma sizeof(int) porque además del contenido, guarda también su tamaño.
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	// Copia el tamaño del nuevo dato al final del stream
	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	
	// Copia el contenid real, los datos en sí (lo que querés enviar) justo después del tamaño
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	// Actualizar el tamaño total del buffer
	paquete->buffer->size += tamanio + sizeof(int); //Suma el nuevo tamaño agregado al total del paquete
}

void enviar_paquete(t_paquete* paquete, int socket_cliente)
{
	/** Calcular la cantidad de bytes a enviar
	 * El msj serializado contiene: 
	 * sizeof(int) para codigo_operacion
	 * sizeof(int) para buffer->size
	 * buffer->size bytes de datos reales (stream)
	 * TOTAL = 2 * sizeof(int) + buffer->size
	 */
	int bytes = paquete->buffer->size + 2*sizeof(int);

	/** Serializar el paquete
	 * Convierte la estructura t_paquete (que tiene campos dispersos en memoria) 
	 * a un bloque contiguo (void*) apto para enviar por red.
	 */
	void* a_enviar = serializar_paquete(paquete, bytes);

	// Enviar los datos por el socket
	send(socket_cliente, a_enviar, bytes, 0);

	// Se libera el bloque temporal a_enviar porque ya fue enviado y no se necesita más
	free(a_enviar);
}

// Toda la memoria reservada con malloc debe ser liberada manualmente.
void eliminar_paquete(t_paquete* paquete)
{
	// Libera la memoria asignada al campo stream, que contiene los datos enviados o recibidos.
	free(paquete->buffer->stream);
	// Libera la memoria del buffer completo (t_buffer)
	free(paquete->buffer);
	// Libera el propio t_paquete, que es la estructura principal
	free(paquete);
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente); // Cierra el fd del socket
	/* close():
	Libera todos los recursos del sistema asociados a ese socket.
	Hace que el número de socket (socket_cliente) ya no pueda usarse para enviar o recibir datos.
	El sistema operativo puede cerrar la conexión TCP asociada (con el proceso correspondiente de FIN/ACK, etc., según el protocolo).
	*/
}
