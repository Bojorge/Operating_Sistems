#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void applyBlurConvolution(unsigned char* imagePart, unsigned char* blurredImagePart, int width, int height, int channels, int kernelSize) {
    int halfKernel = kernelSize / 2;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            for (int c = 0; c < channels; ++c) {
                float sum = 0.0f;
                int count = 0;
                for (int ky = -halfKernel; ky <= halfKernel; ++ky) {
                    for (int kx = -halfKernel; kx <= halfKernel; ++kx) {
                        int nx = x + kx;
                        int ny = y + ky;
                        if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                            sum += imagePart[(ny * width + nx) * channels + c];
                            count++;
                        }
                    }
                }
                blurredImagePart[(y * width + x) * channels + c] = (unsigned char)(sum / count);
            }
        }
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int width, height, channels;
    unsigned char* inputImage = NULL;
    if (rank == 0) {
        inputImage = stbi_load("cato.png", &width, &height, &channels, 0);
        if (inputImage == NULL) {
            fprintf(stderr, "No se pudo cargar la imagen.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    MPI_Bcast(&width, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&height, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&channels, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int rowsPerProcess = height / size;
    int remainderRows = height % size;

    int localHeight = rowsPerProcess;
    if (rank < remainderRows) {
        localHeight++;
    }

    unsigned char* imagePart = (unsigned char*)malloc(localHeight * width * channels * sizeof(unsigned char));
    unsigned char* blurredImagePart = (unsigned char*)malloc(localHeight * width * channels * sizeof(unsigned char));

    if (rank == 0) {
        int offset = 0;
        for (int i = 0; i < size; ++i) {
            int rowsToSend = rowsPerProcess;
            if (i < remainderRows) {
                rowsToSend++;
            }
            if (i == 0) {
                memcpy(imagePart, inputImage + offset * width * channels, rowsToSend * width * channels);
            } else {
                MPI_Send(inputImage + offset * width * channels, rowsToSend * width * channels, MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD);
            }
            offset += rowsToSend;
        }
    } else {
        MPI_Recv(imagePart, localHeight * width * channels, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    applyBlurConvolution(imagePart, blurredImagePart, width, localHeight, channels, 25);

    int* recvCounts = NULL;
    int* displs = NULL;

    if (rank == 0) {
        recvCounts = (int*)malloc(size * sizeof(int));
        displs = (int*)malloc(size * sizeof(int));
        int offset = 0;
        for (int i = 0; i < size; ++i) {
            recvCounts[i] = rowsPerProcess * width * channels;
            if (i < remainderRows) {
                recvCounts[i] += width * channels;
            }
            displs[i] = offset;
            offset += recvCounts[i];
        }
    }

    unsigned char* resultImage = NULL;
    if (rank == 0) {
        resultImage = (unsigned char*)malloc(height * width * channels * sizeof(unsigned char));
    }

    MPI_Gatherv(blurredImagePart, localHeight * width * channels, MPI_UNSIGNED_CHAR,
                resultImage, recvCounts, displs, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        stbi_write_jpg("Outputs/gaussian_blur_filtered_image.jpg", width, height, channels, resultImage, 100);
        free(resultImage);
        free(recvCounts);
        free(displs);
    }

    free(imagePart);
    free(blurredImagePart);
    if (rank == 0) {
        stbi_image_free(inputImage);
    }

    MPI_Finalize();
    return 0;
}