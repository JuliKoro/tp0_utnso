/**
 * @file utils.h
 * @author sisoputnfrba
 * @author JuliKoro
 * @date 21 Mar 2019
 * @brief "header file" (encabezado) de las utilities del client
 *
 * Se ubican las declaraciones de funciones, constantes, structs, etc., que luego se usan en utils.c.
 * @see https://docs.utnso.com.ar/primeros-pasos/tp0
 * @see https://docs.utnso.com.ar/guias/linux/sockets
 */

 /** Guardas de inclusion
 * Evitan que el archivo sea incluido más de una vez en el mismo archivo fuente (.c).
 * Si ya fue incluido, se omite; si no, se define CLIENT_H_ y se incluye el contenido.
 * Esto previene errores de compilación por definiciones múltiples.
 */
#ifndef UTILS_H_
#define UTILS_H_

// Librerias standard de C
#include<stdio.h> // Entrada/salida (por ejemplo, printf, perror, etc.).
#include<stdlib.h> // Utilidades como malloc, free, exit.
#include<string.h> // manipulación de cadenas de caracteres y memoria

// Librerias standard de POSIX/Linux
#include<signal.h> // Permite manejar señales del sistema (como SIGINT, SIGTERM, etc.)
#include<unistd.h> // Contiene funciones POSIX básicas (read, write, close, etc.) Syscalls al SO
#include<sys/socket.h> // Proporciona la interfaz principal para trabajar con sockets (socket, bind, listen, etc.) (struct sockaddr)
#include<netdb.h> // Para trabajar con resolución de nombres de host y puertos (getaddrinfo(), freeaddrinfo()) (struct addrinfo)

// Librerías de la biblioteca Commons (de so-unix/utn)
#include<commons/log.h> // Para crear logs fácilmente (t_log* logger, log_info, etc.).

/**
 * @brief Define los tipos de operación que pueden ser enviados a través del socket
 * 
 * Permite saber qué tipo de contenido está llegando en una conexión, y así tomar decisiones distintas según el valor.
 */
typedef enum
{
	MENSAJE, /**< mensajes simples (string) [por defecto es 0]*/
	PAQUETE /**< otro tipo de contenido más complejo [por defecto 1]*/
} op_code;

/**
 * @brief Representa un bloque de datos en memoria
 */
typedef struct
{
	int size; /**< tamaño del contenido (en bytes) */
	void* stream; /**< un puntero genérico (void*) a la zona de memoria donde está el contenido (el mensaje real, datos binarios, etc.) */
} t_buffer;

/**
 * @brief Representa un paquete de datos completo que se va a enviar o recibir
 * 
 * Es el "envoltorio" completo para cualquier tipo de mensaje o instrucción que quieras enviar por socket
 */
typedef struct
{
	op_code codigo_operacion; /**< indica qué tipo de mensaje es (usando el enum op_code) */
	t_buffer* buffer; /**< contiene el contenido real del paquete (y su tamaño) */
} t_paquete;

/**
 * @brief Crea nu socket de conexion cliente y lo conecta a un server
 * @param ip IP del server ("000.0.0.0") [char*]
 * @param puerto Numero de puerto del server ("4444") [char*]
 */
int crear_conexion(char* ip, char* puerto);

int handshake_cliente(int socket_cliente);

/**
 * @brief Envia un mensaje string al servidor por socket
 * @param mensaje l string char* que vamos a enviar
 * @param socket_cliente fd del socket a través del cual lo vamos a enviar.
 * 
 * Envia un mensaje por socket a un servidor, empaquetando los datos con una estructura más robusta y organizada.
 */
void enviar_mensaje(char* mensaje, int socket_cliente);

/**
 * @brief Crea el paquete que vamos a enviar
 * @return paquete: devuelve el puntero al paquete creado, ya listo para usarse
 * @note Función de inicialización que se encarga de crear un nuevo t_paquete, configurarlo y dejarlo listo para ser usado 
 */
t_paquete* crear_paquete(void);

/**
 * @brief Inicializar el campo buffer dentro de un t_paquete
 * @param paquete t_paquete* ya creado
 */
void crear_buffer(t_paquete* paquete);

/**
 * @brief Dado un stream y su tamaño, lo agrega al paquete.
 * @param paquete (t_paquete*) paquete de datos, ya creado
 * @param valor (void*) datos que querramos agregar
 * @param tamanio (int) tamanio del valor
 * 
 * Permite agregar varios datos al mismo paquete, uno tras otro, con su tamaño incluido.
 */
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);

/**
 * @brief Toma un t_paquete (estructura compleja) y lo convierte en un bloque de memoria contiguo (void*) listo para ser enviado por un socket con send()
 * @param paquete t_paquete* que quiero serializar
 * @param bytes tamanio en bytes del paquete
 * @return void* magic: puntero a una estructura de este tipo
 * @note
 * | 4 bytes (codigo_operacion) | 4 bytes (size) | size bytes (stream) |
 */
void* serializar_paquete(t_paquete* paquete, int bytes);

/**
 * @brief Dada una conexión y un paquete, lo envía a través de ella.
 * @param paquete (t_paquete*) paquete ya creado y listo
 * @param socket_cliente (int) fd del socket de conexion
 * 
 * Se encarga de enviar un paquete estructurado a través de un socket ya conectado.
 */
void enviar_paquete(t_paquete* paquete, int socket_cliente);

/**
 * @brief Termina la conexión y libera los recursos que se usaron para gestionar la misma.
 * @param socket_cliente fd del socket utilizado para la conexion
 * @note Es un wrapper para close()
 */
void liberar_conexion(int socket_cliente);

/**
 * @brief Elimina la memoria usada por el paquete.
 * @param paquete (t_paquete*) paquete ya enviado que no sirve mas.
 * @note Escencial para evitar memory leaks
 * 
 * Se encarga de liberar toda la memoria reservada para un paquete (t_paquete), incluyendo su contenido.
 */
void eliminar_paquete(t_paquete* paquete);

// Cierra las guards de inclusión
#endif /* UTILS_H_ */
