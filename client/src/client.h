/**
 * @file client.h
 * @author sisoputnfrba
 * @author JuliKoro
 * @date 21 Mar 2019
 * @brief "header file" (encabezado) del cliente
 *
 * Se ubican las declaraciones de funciones, constantes, structs, etc., que luego se usan en client.c.
 * @see https://docs.utnso.com.ar/primeros-pasos/tp0
 * @see https://docs.utnso.com.ar/guias/linux/sockets
 */

/** Guardas de inclusion
 * Evitan que el archivo sea incluido más de una vez en el mismo archivo fuente (.c).
 * Si ya fue incluido, se omite; si no, se define CLIENT_H_ y se incluye el contenido.
 * Esto previene errores de compilación por definiciones múltiples.
 */
#ifndef CLIENT_H_
#define CLIENT_H_

// Librerias standard de C
#include<stdio.h> // Entrada/salida (por ejemplo, printf, perror, etc.).
#include<stdlib.h> // Utilidades como malloc, free, exit.

// Librerías de la biblioteca Commons (de so-unix/utn)
#include<commons/log.h> // Para crear logs fácilmente (t_log* logger, log_info, etc.).
#include<commons/string.h> // Funciones adicionales para manejo de strings.
#include<commons/config.h> //  Permite leer archivos .config, accediendo a claves y valores.

// Leer entradas desde la consola
#include<readline/readline.h> 

// Inclusión del archivo de utilidades
#include "utils.h"

/**
 * @brief Crea un archivo logger, listo para utilizar
 * @return t_log* nuevo_logger: puntero al archivo cliente.log
*/
t_log* iniciar_logger(void);

/**
 * @brief carga un archivo de configuración al inicio del programa, y obtener acceso fácil a sus valores clave.
 * @return t_config* nuevo_config:  puntero a una estructura config, que maneja las claves y valores del archivo
 */
t_config* iniciar_config(void);

/**
 * @brief Lee desde la consola y lo loggea
 * @param logger puntero t_log* al logger creado previamente
 * 
 * Lee líneas de texto que el usuario escribe en la consola (una por una) y las muestra en el log, hasta que el usuario ingresa una línea vacía (solo presiona Enter).
 */
void leer_consola(t_log*);

/**
 * @brief 
 * @param 
 */
void paquete(int);

/**
 * @brief Cierra y libera to las estructuras de memoria utilizadas
 * @param conexion (int) fd del socket de conexion
 * @param logger uilizado para logger mensajes
 * @param onfig archivo de configs
 */
void terminar_programa(int, t_log*, t_config*);

// Cierra las guards de inclusión
#endif /* CLIENT_H_ */
