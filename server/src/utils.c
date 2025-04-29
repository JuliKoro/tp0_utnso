#include"utils.h"

t_log* logger;

int iniciar_servidor(void)
{
	// Quitar esta línea cuando hayamos terminado de implementar la funcion
	//assert(!"no implementado!");

	// @brief socket del servidor
	int socket_servidor;
	
	// addrinfo es utilizada para almacenar información sobre direcciones de red y parámetros relacionados con la conexión. 
	
	// @brief hints es una instancia de esta estructura que se utiliza para proporcionar sugerencias o restricciones al llamar a la función getaddrinfo
	struct addrinfo hints;

	// @brief puntero a una lista enlazada de addrinfo que cumplan con hints
	struct addrinfo *server_info;

	// Configurar hints
	memset(&hints, 0, sizeof(hints)); // pongo en 0 todos lo bytes de hints (memset llena un bloque de memoria con un valor específico)
	hints.ai_family = AF_INET; // familia de direcciones = IPv4
	hints.ai_socktype = SOCK_STREAM; // tipo de socket = TCP (conexión orientada)
	hints.ai_flags = AI_PASSIVE; // Banderas opcionales que modifican el comportamiento de getaddrinfo
								// para indicar que la dirección se va a usar para bind() (servidores).

	getaddrinfo(NULL, PUERTO, &hints, &server_info); // llena server_info con hints
	// NULL para que sea un puerto de escucha (en todas las interfaces de red disponibles)
	// Entonces, AI_PASSIVE vale 0.0.0.0 (direccion wildcard)
	/*
	Si el flag AI_PASSIVE está presente en hints.ai_flags y 
	el primer parámetro de getaddrinfo() es NULL, 
	las direcciones devueltas por getaddrinfo() son las adecuadas 
	para crear un socket de escucha.
	*/

	// Creamos el socket de escucha del servidor
	socket_servidor = socket(server_info->ai_family, // dominio
							server_info->ai_socktype, // tipo
							server_info->ai_protocol); // protocolo

	// Asociamos el socket a un puerto

	// setsocketpt: Antes, configuro funciones especiales en el socket:
	// socket_servidor: el fd del socket
	// SOL_SOCKET: nivel donde se aplica la opción (nivel de socket)
	// SO_REUSEPORT: Permite que varios sockets puedan enlazarse al mismo puerto
	// (Muy útil si cerraste el programa y querés volver a levantarlo rápido sin esperar que el sistema libere el puerto)
	// &(int){1}: valor que querés establecer (pne un 1/activar opcion)
	// sizeof(int): tamanio del valor pasado
	setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int));
	// setsockopt acá permite reusar el puerto inmediatamente después de cerrar el servidor (sin que tire error "Address already in use").

	// bind: Asocia el socket a una dirección IP y a un número de puerto
	// ai_addr: direccion del puerto
	// ai_adrlen: tamaño de la estructura de dirección
	bind(socket_servidor, server_info->ai_addr, server_info->ai_addrlen);
	// bind dice: "Este socket va a escuchar en esta IP y este puerto."

	// Escuchamos las conexiones entrantes
	// listen: Pone el socket en modo de escucha, esperando que los clientes se conecten.
	// socket_servidor: fd del socket YA bindeado
	// SOMAXCONN: nro máx de conexiones pendientes que puede mantener en cola (normalmente 128 o más)
	listen(socket_servidor, SOMAXCONN);
	// listen dice: "Estoy listo para aceptar conexiones de clientes."

	freeaddrinfo(server_info); // Libero la memoria reservada anteriormente por getaddrinfo
	log_trace(logger, "Listo para escuchar a mi cliente");

	return socket_servidor;
}

int esperar_cliente(int socket_servidor)
{
	// Quitar esta línea cuando hayamos terminado de implementar la funcion
	//assert(!"no implementado!");

	// Aceptamos un nuevo cliente
	// El servidor usa accept para esperar y aceptar una nueva conexión entrante de un cliente.
	// socket_servidor: socket YA bindeado y en modo escucha
	// NULL, NULL: Normalmente podrías pasar punteros para saber la dirección IP y el tamaño de quien se conecta, 
	// pero si no te interesa (por ahora), podés pasar NULL. 
	int socket_cliente = accept(socket_servidor, NULL, NULL);
	/*accept bloquea el servidor esperando un cliente que se quiera conectar.
	Cuando llega una conexión, devuelve un nuevo socket (socket_cliente) 
	que representa esa conexión específica.
	- socket_servidor sigue abierto para aceptar más clientes.
	- Cada cliente va a tener su propio socket_cliente.*/
	// accept es bloqueante: el servidor se queda esperando hasta que un cliente llegue.
	
	log_info(logger, "Se conecto un cliente!");

	return socket_cliente;
}

int handshake_servidor(int socket_cliente)
{
	size_t bytes;

	int32_t handshake;
	int32_t resultOk = 0;
	int32_t resultError = -1;
	
	// recibe el handshake del cliente
	/* el servidor espera recibir 4 bytes desde el cliente
	socket_cliente: el socket que representa al cliente (resultado de accept()).
	MSG_WAITALL: asegura que se reciban todos los bytes esperados (4 bytes en este caso) antes de continuar.
	El dato recibido se guarda en la variable handshake.
	*/
	bytes = recv(socket_cliente, &handshake, sizeof(int32_t), MSG_WAITALL);
	// lee el mensaje del cliente que, según el protocolo, debería ser 1.
	
	if (handshake == 1) { //Verifica si el handshake es válido
		//es correcto, se asume que el cliente está usando el protocolo esperado.
		// responde al cliente
		// Se envía el valor 0 al cliente como señal de "Handshake OK".
		bytes = send(socket_cliente, &resultOk, sizeof(int32_t), 0);
	} else { //Si fue incorrecto
		bytes = send(socket_cliente, &resultError, sizeof(int32_t), 0);
		//Se envía -1 como señal de "Handshake ERROR".
	}
}

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void* recibir_buffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void recibir_mensaje(int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "Me llego el mensaje %s", buffer);
	free(buffer);
}

t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}
