/**
 * @file utils.h
 * @author sisoputnfrba
 * @author JuliKoro
 * @date 21 Mar 2019
 * @brief "header file" (encabezado) de las utilities del servidor
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
#include<assert.h> // Permite verificar que se cumplan condiciones críticas durante la ejecución.

// Librerias standard de POSIX/Linux
#include<sys/socket.h> // Proporciona la interfaz principal para trabajar con sockets (socket, bind, listen, etc.) (struct sockaddr)
#include<unistd.h> // Contiene funciones POSIX básicas (read, write, close, etc.) Syscalls al SO
#include<netdb.h> // Para trabajar con resolución de nombres de host y puertos (getaddrinfo(), freeaddrinfo()) (struct addrinfo)

// Librerías de la biblioteca Commons (de so-unix/utn)
#include<commons/log.h> // Para crear logs fácilmente (t_log* logger, log_info, etc.).
#include<commons/collections/list.h>

/* define un nombre simbólico (macro) llamado PUERTO,
 que será reemplazado por el valor "4444" en tiempo de compilación.*/
#define PUERTO "4444"

/**
 * @brief Define los tipos de operación que pueden ser enviados a través del socket
 * 
 * Permite saber qué tipo de contenido está llegando en una conexión, y así tomar decisiones distintas según el valor.
 */
typedef enum
{
	MENSAJE,
	PAQUETE
}op_code;

// Declaracion de variable global
extern t_log* logger;

/**
 * @brief Recibir datos dinámicos desde un socket
 * @param size (int) tamaño del buffer que va a llegar
 * @param socket_cliente (int) fd del socket ya conectado y verificado handshake
 * @return buffer (void*) retorna el buffer con los datos enviados y su tamanio (como variable generica)
 */
void* recibir_buffer(int*, int);

/**
 * @brief Crea un socket de escucha, lo configura, lo bindea a un IP y puerto, y queda en escucha
 * @return socket_servidor (un fd)
 */
int iniciar_servidor(void);

/**
 * @brief Acepta un nuevo cliente y lo conecta al servidor
 * @param socket_servidor fd (int) del socket del server ya bindeado y en escucha
 * @return socket_cliente: fd (int) del socket a conectado
 * @note Es bloqueante, por lo que se queda esperando hasta que acepte a algun cliente
 */
int esperar_cliente(int);

/**
 * @brief Recibir un paquete compuesto por múltiples elementos (strings o bloques de datos) desde un socket, y almacenarlos en una lista (t_list*)
 * @param socket_cliente (int) fd del socket
 * @pre @p socket_cliente conectado, bindeado y aceptado
 * @return @p valores (t_list*) lista de elementos recibidos
 */
t_list* recibir_paquete(int);

/**
 * @brief Recibir un mensaje enviado desde un cliente por un socket
 * @param socket_cliente (int) fd del socket listo para comunicar
 * @pre @p socket_cliente conectado, bindeado y handshake OK
 */
void recibir_mensaje(int);

/**
 * @brief Recibir el código de operación enviado por un cliente a través de un socket
 * @param socket_cliente (int) fd del socket ya conectado
 * @return cod_op (int) si la operacion fue exiotsa o -1 si algo fallo
 * @note Sirve como un handshacke para la comunicacion
 */
int recibir_operacion(int);

// Cierra las guards de inclusión
#endif /* UTILS_H_ */
