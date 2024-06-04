#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <ctype.h>

#define AES_KEY_LENGTH 256
#define AES_BLOCK_SIZE 16
#define MAX_WORD_LENGTH 100
#define MAX_WORD_COUNT 1000

void handleErrors(void) {
    ERR_print_errors_fp(stderr);
    abort();
}

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

void count_words(char *text, char words[MAX_WORD_COUNT][MAX_WORD_LENGTH], int counts[MAX_WORD_COUNT], int *unique_word_count) {
    char *token = strtok(text, " ");
    *unique_word_count = 0;

    while (token != NULL) {
        int found = 0;
        for (int i = 0; i < *unique_word_count; i++) {
            if (strcmp(words[i], token) == 0) {
                counts[i]++;
                found = 1;
                break;
            }
        }
        if (!found) {
            strcpy(words[*unique_word_count], token);
            counts[*unique_word_count] = 1;
            (*unique_word_count)++;
        }
        token = strtok(NULL, " ");
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    unsigned char *plaintext = NULL;
    unsigned char *ciphertext = NULL;
    unsigned char *receivedCiphertext = NULL;
    unsigned char *decryptedtext = NULL;
    unsigned char key[AES_KEY_LENGTH / 8];
    unsigned char iv[AES_BLOCK_SIZE];
    int textLength = 0;
    int ciphertext_len;

    if (rank == 0) {
        // Servidor

        // Leer texto desde el archivo
        FILE *file = fopen("texto.txt", "r");
        if (file == NULL) {
            fprintf(stderr, "Error al abrir el archivo\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        fseek(file, 0, SEEK_END);
        textLength = ftell(file);
        fseek(file, 0, SEEK_SET);

        plaintext = (unsigned char*)malloc((textLength + 1) * sizeof(unsigned char));
        if (plaintext == NULL) {
            fprintf(stderr, "Error al asignar memoria\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        fread(plaintext, 1, textLength, file);
        plaintext[textLength] = '\0';
        fclose(file);

        // Generar clave y IV aleatorios
        if (!RAND_bytes(key, sizeof(key)) || !RAND_bytes(iv, sizeof(iv))) {
            fprintf(stderr, "Error generando clave o IV\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // Cifrar el texto
        ciphertext = (unsigned char*)malloc((textLength + AES_BLOCK_SIZE) * sizeof(unsigned char));
        ciphertext_len = encrypt(plaintext, textLength, key, iv, ciphertext);

        // Enviar la longitud del texto cifrado a todos los procesos
        MPI_Bcast(&ciphertext_len, 1, MPI_INT, 0, MPI_COMM_WORLD);

        // Enviar el texto cifrado
        MPI_Bcast(ciphertext, ciphertext_len, MPI_CHAR, 0, MPI_COMM_WORLD);

        // Enviar la clave y el IV
        MPI_Bcast(key, sizeof(key), MPI_CHAR, 0, MPI_COMM_WORLD);
        MPI_Bcast(iv, sizeof(iv), MPI_CHAR, 0, MPI_COMM_WORLD);

        // Recibir y procesar resultados de los clientes
        char words[MAX_WORD_COUNT][MAX_WORD_LENGTH];
        int counts[MAX_WORD_COUNT] = {0};
        int unique_word_count = 0;

        for (int i = 1; i < size; i++) {
            char received_word[MAX_WORD_LENGTH];
            int received_count;
            MPI_Recv(received_word, MAX_WORD_LENGTH, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&received_count, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            int found = 0;
            for (int j = 0; j < unique_word_count; j++) {
                if (strcmp(words[j], received_word) == 0) {
                    counts[j] += received_count;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                strcpy(words[unique_word_count], received_word);
                counts[unique_word_count] = received_count;
                unique_word_count++;
            }
        }

        // Encontrar la palabra que más se repite
        int max_count = 0;
        char most_repeated_word[MAX_WORD_LENGTH];
        for (int i = 0; i < unique_word_count; i++) {
            if (counts[i] > max_count) {
                max_count = counts[i];
                strcpy(most_repeated_word, words[i]);
            }
        }

        printf("La palabra que más se repite es: %s (repetida %d veces)\n", most_repeated_word, max_count);

        free(plaintext);
        free(ciphertext);
    } else {
        // Cliente

        // Recibir la longitud del texto cifrado
        MPI_Bcast(&ciphertext_len, 1, MPI_INT, 0, MPI_COMM_WORLD);

        // Asignar memoria para recibir el texto cifrado
        receivedCiphertext = (unsigned char*)malloc(ciphertext_len * sizeof(unsigned char));
        if (receivedCiphertext == NULL) {
            fprintf(stderr, "Error al asignar memoria en el proceso %d\n", rank);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // Recibir el texto cifrado
        MPI_Bcast(receivedCiphertext, ciphertext_len, MPI_CHAR, 0, MPI_COMM_WORLD);

        // Recibir la clave y el IV
        MPI_Bcast(key, sizeof(key), MPI_CHAR, 0, MPI_COMM_WORLD);
        MPI_Bcast(iv, sizeof(iv), MPI_CHAR, 0, MPI_COMM_WORLD);

        // Asignar memoria para el texto descifrado
        decryptedtext = (unsigned char*)malloc((ciphertext_len + 1) * sizeof(unsigned char));
        if (decryptedtext == NULL) {
            fprintf(stderr, "Error al asignar memoria en el proceso %d\n", rank);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // Descifrar el texto
        int decryptedtext_len = decrypt(receivedCiphertext, ciphertext_len, key, iv, decryptedtext);
        decryptedtext[decryptedtext_len] = '\0';

        // Contar palabras
        char words[MAX_WORD_COUNT][MAX_WORD_LENGTH];
        int counts[MAX_WORD_COUNT] = {0};
        int unique_word_count = 0;

        count_words((char*)decryptedtext, words, counts, &unique_word_count);

        // Encontrar la palabra que más se repite
        int max_count = 0;
        char most_repeated_word[MAX_WORD_LENGTH];
        for (int i = 0; i < unique_word_count; i++) {
            if (counts[i] > max_count) {
                max_count = counts[i];
                strcpy(most_repeated_word, words[i]);
            }
        }

        // Enviar la palabra más repetida y su conteo al servidor
        MPI_Send(most_repeated_word, MAX_WORD_LENGTH, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&max_count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

        free(receivedCiphertext);
        free(decryptedtext);
    }

    MPI_Finalize();
    return 0;
}

