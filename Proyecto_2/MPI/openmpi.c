#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include "Driver/driver_lib.h"

// Estructura para almacenar palabras y sus frecuencias
typedef struct {
    char word[100];
    int count;
} WordCount;

void handleErrors(void) {
    ERR_print_errors_fp(stderr);
    abort();
}

// Función para convertir una cadena a minúsculas
void toLowerCase(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

// Función para contar palabras en una porción de texto y almacenar sus frecuencias
int countWordsAndFrequencies(const char* text, int length, WordCount** wordCounts, int* wordCountSize) {
    char word[100];
    int wordLen = 0;
    int capacity = 10;
    *wordCounts = (WordCount*)malloc(capacity * sizeof(WordCount));
    *wordCountSize = 0;

    for (int i = 0; i < length; ++i) {
        if (isalpha(text[i])) {
            if (wordLen < 99) {
                word[wordLen++] = tolower(text[i]);
            }
        } else {
            if (wordLen > 0) {
                word[wordLen] = '\0';
                int found = 0;
                for (int j = 0; j < *wordCountSize; ++j) {
                    if (strcmp((*wordCounts)[j].word, word) == 0) {
                        (*wordCounts)[j].count++;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    if (*wordCountSize >= capacity) {
                        capacity *= 2;
                        *wordCounts = (WordCount*)realloc(*wordCounts, capacity * sizeof(WordCount));
                    }
                    strcpy((*wordCounts)[*wordCountSize].word, word);
                    (*wordCounts)[*wordCountSize].count = 1;
                    (*wordCountSize)++;
                }
                wordLen = 0;
            }
        }
    }

    // Contar la última palabra si el texto no termina con un delimitador
    if (wordLen > 0) {
        word[wordLen] = '\0';
        int found = 0;
        for (int j = 0; j < *wordCountSize; ++j) {
            if (strcmp((*wordCounts)[j].word, word) == 0) {
                (*wordCounts)[j].count++;
                found = 1;
                break;
            }
        }
        if (!found) {
            if (*wordCountSize >= capacity) {
                capacity *= 2;
                *wordCounts = (WordCount*)realloc(*wordCounts, capacity * sizeof(WordCount));
            }
            strcpy((*wordCounts)[*wordCountSize].word, word);
            (*wordCounts)[*wordCountSize].count = 1;
            (*wordCountSize)++;
        }
    }
}

int compareWordCount(const void* a, const void* b) {
    return ((WordCount*)b)->count - ((WordCount*)a)->count;
}

// Función para encriptar datos
int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;

    if (!(ctx = EVP_CIPHER_CTX_new())) handleErrors();
    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) handleErrors();
    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len)) handleErrors();
    ciphertext_len = len;
    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) handleErrors();
    ciphertext_len += len;
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

// Función para desencriptar datos
int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;

    if (!(ctx = EVP_CIPHER_CTX_new())) handleErrors();
    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) handleErrors();
    if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len)) handleErrors();
    plaintext_len = len;
    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) handleErrors();
    plaintext_len += len;
    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}

void divideText(char* inputText, long fileSize, int rank, int size, char** textPart, int* localSize) {
    int chunkSize = fileSize / size;
    int remainder = fileSize % size;

    int start = rank * chunkSize + (rank < remainder ? rank : remainder);
    int end = start + chunkSize + (rank < remainder ? 1 : 0);

    // Ajustar el punto de inicio y fin para evitar partir palabras
    if (rank != 0) {
        while (start < fileSize && inputText[start] != ' ') start++;
        if (start < fileSize) start++; // Incluir el espacio en el inicio de la siguiente porción
    }
    if (rank != size - 1) {
        while (end < fileSize && inputText[end] != ' ') end++;
    }

    *localSize = end - start;
    *textPart = (char*)malloc((*localSize + 1) * sizeof(char));
    strncpy(*textPart, inputText + start, *localSize);
    (*textPart)[*localSize] = '\0';
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    unsigned char key[32] = "01234567890123456789012345678901"; // Key de 256 bits
    unsigned char iv[16] = "0123456789012345"; // IV de 128 bits

    char* inputText = NULL;
    long fileSize = 0;

    if (rank == 0) {
        FILE* file = fopen("mobydick.txt", "r");
        if (file == NULL) {
            fprintf(stderr, "No se pudo abrir el archivo.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        fseek(file, 0, SEEK_END);
        fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);

        inputText = (char*)malloc(fileSize + 1);
        fread(inputText, 1, fileSize, file);
        inputText[fileSize] = '\0';
        fclose(file);
    }

    MPI_Bcast(&fileSize, 1, MPI_LONG, 0, MPI_COMM_WORLD);

    char* textPart = NULL;
    int localSize = 0;

    if (rank == 0) {
        for (int i = 0; i < size; ++i) {
            char* part;
            int partSize;
            divideText(inputText, fileSize, i, size, &part, &partSize);
            if (i == 0) {
                textPart = part;
                localSize = partSize;
            } else {
                // Encriptar la porción de texto antes de enviar
                unsigned char* encryptedTextPart = (unsigned char*)malloc((partSize + EVP_MAX_BLOCK_LENGTH) * sizeof(unsigned char));
                int ciphertext_len = encrypt((unsigned char*)part, partSize, key, iv, encryptedTextPart);
                MPI_Send(&ciphertext_len, 1, MPI_INT, i, 0, MPI_COMM_WORLD); // Enviar longitud del texto cifrado
                MPI_Send(encryptedTextPart, ciphertext_len, MPI_BYTE, i, 1, MPI_COMM_WORLD); // Enviar texto cifrado
                free(part);
                free(encryptedTextPart);
            }
        }
        free(inputText);
    } else {
        int ciphertext_len;
        unsigned char* encryptedTextPart = (unsigned char*)malloc((fileSize / size + EVP_MAX_BLOCK_LENGTH) * sizeof(unsigned char));
        MPI_Recv(&ciphertext_len, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // Recibir longitud del texto cifrado
        MPI_Recv(encryptedTextPart, ciphertext_len, MPI_BYTE, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // Recibir texto cifrado
        // Desencriptar la porción de texto recibida
        unsigned char* decryptedTextPart = (unsigned char*)malloc((ciphertext_len + EVP_MAX_BLOCK_LENGTH) * sizeof(unsigned char));
        int plaintext_len = decrypt(encryptedTextPart, ciphertext_len, key, iv, decryptedTextPart);
        decryptedTextPart[plaintext_len] = '\0';
        textPart = (char*)malloc((plaintext_len + 1) * sizeof(char));
        strcpy(textPart, (char*)decryptedTextPart);
        localSize = plaintext_len;
        free(encryptedTextPart);
        free(decryptedTextPart);
    }

    WordCount* localWordCounts = NULL;
    int localWordCountSize;
    countWordsAndFrequencies(textPart, localSize, &localWordCounts, &localWordCountSize);

    // Recolectar los tamaños de los conteos de palabras de cada proceso
    int* recvCounts = NULL;
    int* displs = NULL;
    int localWordCountBytes = localWordCountSize * sizeof(WordCount);
    
    if (rank == 0) {
        recvCounts = (int*)malloc(size * sizeof(int));
        displs = (int*)malloc(size * sizeof(int));
    }

    MPI_Gather(&localWordCountBytes, 1, MPI_INT, recvCounts, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        displs[0] = 0;
        for (int i = 1; i < size; ++i) {
            displs[i] = displs[i-1] + recvCounts[i-1];
        }
    }

    // Recolectar los conteos de palabras de cada proceso
    WordCount* totalWordCounts = NULL;
    int totalWordCountBytes = 0;
    
    if (rank == 0) {
        totalWordCountBytes = displs[size-1] + recvCounts[size-1];
        totalWordCounts = (WordCount*)malloc(totalWordCountBytes);
    }

    MPI_Gatherv(localWordCounts, localWordCountBytes, MPI_BYTE, totalWordCounts, recvCounts, displs, MPI_BYTE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        int combinedWordCountSize = recvCounts[0] / sizeof(WordCount);

        for (int i = 1; i < size; ++i) {
            WordCount* nodeWordCounts = (WordCount*)((char*)totalWordCounts + displs[i]);
            int nodeWordCountSize = recvCounts[i] / sizeof(WordCount);
            for (int j = 0; j < nodeWordCountSize; ++j) {
                int found = 0;
                for (int k = 0; k < combinedWordCountSize; ++k) {
                    if (strcmp(totalWordCounts[k].word, nodeWordCounts[j].word) == 0) {
                        totalWordCounts[k].count += nodeWordCounts[j].count;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    totalWordCounts[combinedWordCountSize] = nodeWordCounts[j];
                    combinedWordCountSize++;
                }
            }
        }

        // Encontrar la palabra más frecuente
        qsort(totalWordCounts, combinedWordCountSize, sizeof(WordCount), compareWordCount);

        printf("\nLa palabra más frecuente es: \"%s\" y la cantidad de veces que se repite es: %d\n", totalWordCounts[0].word, totalWordCounts[0].count);
        printf("\nLa segunda palabra más frecuente es: \"%s\" y la cantidad de veces que se repite es: %d\n", totalWordCounts[1].word, totalWordCounts[1].count);

        int fd = open_serial_port("/dev/ttyACM0");
        int bytes_written = send_word(fd, ("%s\n", totalWordCounts[0].word));

        close_serial_port(fd);
        free(totalWordCounts);
        free(recvCounts);
        free(displs);
    }

    free(textPart);
    free(localWordCounts);
    MPI_Finalize();
    return 0;
}

