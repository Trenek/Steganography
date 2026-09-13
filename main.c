#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int min(int a, int b) {
    return (a < b) ? a : b;
}

int hide(const char* baitName, const char* secretName, const char* resultName, int bit) {
    int w1 = 0;
    int h1 = 0;
    int c1 = 0;
    int w2 = 0;
    int h2 = 0;
    int c2 = 0;

    unsigned char* bait = stbi_load(baitName, &w1, &h1, &c1, 3);
    unsigned char* secret = stbi_load(secretName, &w2, &h2, &c2, 3);

    unsigned char (*baitID)[w1][c1] = (void*)bait;
    unsigned char (*secretID)[w2][c2] = (void*)secret;

    int minW = min(w1, w2);
    int minH = min(h1, h2);
    int minC = min(c1, c2);

    for (int i = 0; i < minH; i += 1) {
        for (int j = 0; j < minW; j += 1) {
            for (int k = 0; k < minC; k += 1) {
                baitID[i][j][k] = 
                    ((-1U << bit) & baitID[i][j][k]) |
                    (secretID[i][j][k] >> (8 - bit));
            }
        }
    }

    int success = stbi_write_png(resultName, w1, h1, 3, bait, w1 * 3);

    stbi_image_free(bait);
    stbi_image_free(secret);

    return success;
}

int uhide(const char* baitName, const char* resultName, int bit) {
    int w1 = 0;
    int h1 = 0;
    int c1 = 0;

    unsigned char* bait = stbi_load(baitName, &w1, &h1, &c1, 3);

    unsigned char (*baitID)[w1][3] = (void*)bait;

    for (int i = 0; i < h1; i += 1) {
        for (int j = 0; j < w1; j += 1) {
            for (int k = 0; k < 3; k += 1) {
                baitID[i][j][k] <<= 8 - bit;
            }
        }
    }

    int success = stbi_write_png(resultName, w1, h1, 3, bait, w1 * 3);

    stbi_image_free(bait);

    return success;
}

double entropy(double p) {
    return p * log2(p);
}

uint8_t getChangedBits(const char* fileName) {
    uint8_t result = 0;

    int w = 0;
    int h = 0;
    int c = 0;

    unsigned char* img = stbi_load(fileName, &w, &h, &c, 3);
    
    if (img == NULL) {
        fprintf(stderr, "Error: File %s Does not exist\n", fileName);
        return 0;
    }

    long long pixelQuantity = 3 * w * h;
    long long bitCounts[8][2] = {};
    double bitEntropy[8] = {};

    for (long long i = 0; i < pixelQuantity; i += 1) {
        for (int b = 0; b < 8; b += 1) {
            bitCounts[b][(img[i] >> b) & 1] += 1;
        }
    }

    stbi_image_free(img);

    for (int b = 0; b < 8; b += 1) {
        for (int val = 0; val < 2; val += 1) {
            bitEntropy[b] -= entropy((double)bitCounts[b][val] / pixelQuantity);
        }
    }

    for (int b = 0; b < 7; b += 1) {
        double diff = bitEntropy[b] - bitEntropy[b + 1];

        printf("%lf\n", diff);
        if (diff < -0.02 || (diff > 0.08 && b < 5)) {
            result = b + 1;
        }
    }

    return result;
}


void print_usage(const char* progName) {
    printf("Usage:\n");
    printf("  %s encode <bit> <bait file> <secret file> <result file>\n", progName);
    printf("  %s decode <coded_image> <decoded_image>\n", progName);
}

void readCommand(int argc, char *argv[]) {
    const char* progName = argv[0];

    if (argc < 2) {
        print_usage(progName);
        return;
    }

    if (strcmp(argv[1], "encode") == 0) {
        if (argc != 6) {
            fprintf(stderr, "Wrong Usage\n");
            print_usage(progName);
            return;
        }

        int bit = atoi(argv[2]);
        const char* bait = argv[3];
        const char* secret = argv[4];
        const char* result = argv[5];

        if (bit < 0 || bit > 8) {
            fprintf(stderr, "Error: Number of bits have to be between 0 and 8\n");
            return;
        }

        hide(bait, secret, result, bit);
    }
    else if (strcmp(argv[1], "decode") == 0) {
        if (argc != 4) {
            fprintf(stderr, "Błąd: Niepoprawna składnia dla 'decode'.\n");
            print_usage(progName);
            return;
        }

        const char* coded = argv[2];

        uint8_t changedBits = getChangedBits(coded);

        if (changedBits == 0) {
            printf("No steganography detected\n");
            return;
        }
        else {
            printf("Steganography detected at bit: %d\n", changedBits);
        }

        uhide(coded, argv[3], changedBits);
    }
    else {
        fprintf(stderr, "Błąd: Nieznana komenda '%s'.\n", argv[1]);
        print_usage(progName);
    }
}

int main(int argc, char *argv[]) {
    // int status = hide("obrazki/3.png", "obrazki/1.jpeg", "obrazki/coded.png", 6);
    // int changedBits = 0;
    //
    // printf("%d\n", changedBits = getChangedBits("obrazki/coded.png"));
    //
    // uhide("obrazki/coded.png", "obrazki/decoded.png", changedBits);

    readCommand(argc, argv);

    return 0;
}
