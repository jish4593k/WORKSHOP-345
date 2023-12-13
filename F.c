#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void applyCircularMask(unsigned char* img_data, int width, int height, int channels, int blur_radius) {
    int offset = blur_radius * 2;

    unsigned char* mask_data = (unsigned char*)malloc(width * height);

    for (int i = 0; i < width * height; ++i) {
        mask_data[i] = 0;
    }

    for (int y = offset; y < height - offset; ++y) {
        for (int x = offset; x < width - offset; ++x) {
            if ((x - width / 2) * (x - width / 2) + (y - height / 2) * (y - height / 2) < (width / 2 - offset) * (width / 2 - offset)) {
                mask_data[y * width + x] = 255;
            }
        }
    }

    stbi_image_free(img_data);

    // Applying Gaussian blur (you might need a dedicated library for this in C)
    // ...

    unsigned char* result_data = (unsigned char*)malloc(width * height * channels);
    
    // Apply the circular mask
    for (int i = 0; i < width * height; ++i) {
        for (int j = 0; j < channels; ++j) {
            result_data[i * channels + j] = img_data[i * channels + j] * mask_data[i] / 255;
        }
    }

    // Save or display the result_data as needed
    // ...

    free(mask_data);
    free(result_data);
}

size_t write_data(void* ptr, size_t size, size_t nmemb, FILE* stream) {
    return fwrite(ptr, size, nmemb, stream);
}

int downloadImage(const char* url, const char* filename) {
    CURL* curl;
    FILE* fp;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();

    if (curl) {
        fp = fopen(filename, "wb");
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            return 1;
        }

        curl_easy_cleanup(curl);
        fclose(fp);
    }

    curl_global_cleanup();
    return 0;
}

int main() {
    const char* imageUrl = "URL_TO_YOUR_IMAGE";
    const char* outputFilename = "downloaded_image.jpg";

    if (downloadImage(imageUrl, outputFilename) == 0) {
        int width, height, channels;
        unsigned char* img_data = stbi_load(outputFilename, &width, &height, &channels, 0);

        if (img_data) {
            applyCircularMask(img_data, width, height, channels, 1);
        } else {
            fprintf(stderr, "Error loading image.\n");
        }
    } else {
        fprintf(stderr, "Error downloading image.\n");
    }

    return 0;
}
