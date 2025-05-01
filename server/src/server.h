/**
 * @file server.h
 * @author sisoputnfrba
 * @author JuliKoro
 * @date 21 Mar 2019
 * @brief "header file" (encabezado) del servidor
 *
 * Se ubican las declaraciones de funciones, constantes, structs, etc., que luego se usan en server.c.
 * @see https://docs.utnso.com.ar/primeros-pasos/tp0
 * @see https://docs.utnso.com.ar/guias/linux/sockets
 */

 /** Guardas de inclusion
 * Evitan que el archivo sea incluido más de una vez en el mismo archivo fuente (.c).
 * Si ya fue incluido, se omite; si no, se define CLIENT_H_ y se incluye el contenido.
 * Esto previene errores de compilación por definiciones múltiples.
 */
#ifndef SERVER_H_
#define SERVER_H_

// Librerias standard de C
#include <stdio.h> // Entrada/salida (por ejemplo, printf, perror, etc.).
#include <stdlib.h> // Utilidades como malloc, free, exit.
#include <string.h> // manipulación de cadenas de caracteres y memoria

// Librerías de la biblioteca Commons (de so-unix/utn)
#include <commons/log.h> // Para crear logs fácilmente (t_log* logger, log_info, etc.).

// Inclusión del archivo de utilidades
#include "utils.h"

/**
 * @brief 
 * @param 
 * @param 
 */
void iterator(char* value);

// Cierra las guards de inclusión
#endif /* SERVER_H_ */
