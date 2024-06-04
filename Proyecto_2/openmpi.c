#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <openssl/evp.h>
#include <openssl/err.h>

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
int countWordsAndFrequencies(const char* text, int length, WordCount** wordCounts) {
    char word[100];
    int wordLen = 0;
    int wordCountSize = 0;
    int capacity = 10;
    *wordCounts = (WordCount*)malloc(capacity * sizeof(WordCount));

    for (int i = 0; i < length; ++i) {
        if (isalpha(text[i])) {
            if (wordLen < 99) {
                word[wordLen++] = tolower(text[i]);
            }
        } else {
            if (wordLen > 0) {
                word[wordLen] = '\0';
                int found = 0;
                for (int j = 0; j < wordCountSize; ++j) {
                    if (strcmp((*wordCounts)[j].word, word) == 0) {
                        (*wordCounts)[j].count++;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    if (wordCountSize >= capacity) {
                        capacity *= 2;
                        *wordCounts = (WordCount*)realloc(*wordCounts, capacity * sizeof(WordCount));
                    }
                    strcpy((*wordCounts)[wordCountSize].word, word);
                    (*wordCounts)[wordCountSize].count = 1;
                    wordCountSize++;
                }
                wordLen = 0;
            }
        }
    }

    // Contar la última palabra si el texto no termina con un delimitador
    if (wordLen > 0) {
        word[wordLen] = '\0';
        int found = 0;
        for (int j = 0; j < wordCountSize; ++j) {
            if (strcmp((*wordCounts)[j].word, word) == 0) {
                (*wordCounts)[j].count++;
                found = 1;
                break;
            }
        }
        if (!found) {
            if (wordCountSize >= capacity) {
                capacity *= 2;
                *wordCounts = (WordCount*)realloc(*wordCounts, capacity * sizeof(WordCount));
            }
            strcpy((*wordCounts)[wordCountSize].word, word);
            (*wordCounts)[wordCountSize].count = 1;
            wordCountSize++;
        }
    }

    return wordCountSize;
}

// Función para encontrar la palabra más repetida en una lista de WordCount
void findMostFrequentWord(WordCount* wordCounts, int size, char* mostFrequentWord, int* maxCount) {
    *maxCount = 0;
    for (int i = 0; i < size; ++i) {
        if (wordCounts[i].count > *maxCount) {
            *maxCount = wordCounts[i].count;
            strcpy(mostFrequentWord, wordCounts[i].word);
        }
    }
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
        FILE* file = fopen("texto.txt", "r");
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

    int chunkSize = fileSize / size;
    int remainder = fileSize % size;

    int localSize = chunkSize;
    if (rank < remainder) {
        localSize++;
    }

    char* textPart = (char*)malloc((localSize + 1) * sizeof(char));
    unsigned char* encryptedTextPart = (unsigned char*)malloc((localSize + EVP_MAX_BLOCK_LENGTH) * sizeof(unsigned char)); // buffer para texto cifrado
    unsigned char* decryptedTextPart = (unsigned char*)malloc((localSize + EVP_MAX_BLOCK_LENGTH) * sizeof(unsigned char)); // buffer para texto descifrado

    if (rank == 0) {
        int offset = 0;
        for (int i = 0; i < size; ++i) {
            int sendSize = chunkSize;
            if (i < remainder) {
                sendSize++;
            }
            if (i == 0) {
                memcpy(textPart, inputText, sendSize);
                textPart[sendSize] = '\0';
            } else {
                // Encriptar la porción de texto antes de enviar
                int ciphertext_len = encrypt((unsigned char*)(inputText + offset), sendSize, key, iv, encryptedTextPart);
                MPI_Send(&ciphertext_len, 1, MPI_INT, i, 0, MPI_COMM_WORLD); // Enviar longitud del texto cifrado
                MPI_Send(encryptedTextPart, ciphertext_len, MPI_BYTE, i, 1, MPI_COMM_WORLD); // Enviar texto cifrado
            }
            offset += sendSize;
        }
    } else {
        int ciphertext_len;
        MPI_Recv(&ciphertext_len, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // Recibir longitud del texto cifrado
        MPI_Recv(encryptedTextPart, ciphertext_len, MPI_BYTE, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // Recibir texto cifrado
        // Desencriptar la porción de texto recibida
        int plaintext_len = decrypt(encryptedTextPart, ciphertext_len, key, iv, decryptedTextPart);
        decryptedTextPart[plaintext_len] = '\0';
        strcpy(textPart, (char*)decryptedTextPart);
    }

    WordCount* localWordCounts = NULL;
    int localWordCountSize = countWordsAndFrequencies(textPart, strlen(textPart), &localWordCounts);

    char localMostFrequentWord[100];
    int localMaxCount = 0;
    findMostFrequentWord(localWordCounts, localWordCountSize, localMostFrequentWord, &localMaxCount);

    char globalMostFrequentWord[100];
    int globalMaxCount = 0;

    typedef struct {
        char word[100];
        int count;
    } GlobalWordCount;

    GlobalWordCount localResult;
    strcpy(localResult.word, localMostFrequentWord);
    localResult.count = localMaxCount;

    GlobalWordCount* globalResults = NULL;
    if (rank == 0) {
        globalResults = (GlobalWordCount*)malloc(size * sizeof(GlobalWordCount));
    }

    MPI_Gather(&localResult, sizeof(GlobalWordCount), MPI_BYTE, globalResults, sizeof(GlobalWordCount), MPI_BYTE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        for (int i = 0; i < size; ++i) {
            if (globalResults[i].count > globalMaxCount) {
                globalMaxCount = globalResults[i].count;
                strcpy(globalMostFrequentWord, globalResults[i].word);
            }
        }
        printf("La palabra más frecuente es: %s\n", globalMostFrequentWord);
        free(globalResults);
        free(inputText);
    }

    free(textPart);
    free(encryptedTextPart);
    free(decryptedTextPart);
    free(localWordCounts);
    MPI_Finalize();
    return 0;
}
