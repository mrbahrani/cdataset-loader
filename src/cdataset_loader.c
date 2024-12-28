#include "cdataset_loader.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <curl/curl.h>

#ifdef _WIN32
#include <direct.h>
#define mkdir(path, mode) _mkdir(path)
#endif


typedef struct {
    FILE *file;
    const char *filepath;
} FileData;

// Callback function for writing downloaded data to a file
size_t write_file_callback(void *ptr, size_t size, size_t nmemb, void *stream) {
    FileData *file_data = (FileData *)stream;
    size_t written = fwrite(ptr, size, nmemb, file_data->file);
    return written;
}


int _create_directory(const char *path) {
    // Attempt to create the directory
    if (mkdir(path, 0755) == 0) {
        printf("Directory '%s' created successfully.\n", path);
        return 1;
    } else {
        perror("Error creating directory");
        return 0;
    }
}

int _directory_exists(const char* dir) {
    struct stat info;

    // Check if the direcotry exists and is a directory
    if (stat(dir, &info) != 0) {
        // Path does not exist
        return 0;
    } else if (info.st_mode & S_IFDIR) {
        // Path exists and is a directory
        return 1;
    } else {
        // Path exists but is not a directory
        printf("Error: Path exists but it is not a direcotry");
        return 2;
    }
}

int _file_exists(const char *path) {
    struct stat buffer;
    return (stat(path, &buffer) == 0);
}

int _check_files_exists(const char* dir) {
    const char *mnist_files[] = {
        "train-images-idx3-ubyte",
        "train-labels-idx1-ubyte",
        "t10k-images-idx3-ubyte",
        "t10k-labels-idx1-ubyte"
    };

    char full_path[1024];
    for (int i = 0; i < 4; i++) {
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, mnist_files[i]);
        if (!_file_exists(full_path)) {
            printf("File missing: %s\n", full_path);
            return 0; // Return 0 if any file is missing
        }
    }

    return 1;
}

// Function to download a single file
int download_file(const char *url, const char *output_path) {
    CURL *curl;
    CURLcode res;

    // Open the file for writing
    FILE *file = fopen(output_path, "wb");
    if (!file) {
        perror("Error opening file for writing");
        return 0;
    }

    // Initialize libcurl
    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Error initializing libcurl\n");
        fclose(file);
        return 0;
    }

    // Set curl options
    FileData file_data = {file, output_path};
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_file_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file_data);

    // Perform the download
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "Error downloading %s: %s\n", url, curl_easy_strerror(res));
        fclose(file);
        curl_easy_cleanup(curl);
        return 0;
    }

    // Clean up
    fclose(file);
    curl_easy_cleanup(curl);
    return 1;
}

int _mnist_download(const char* dir) {
    const char *base_url = "http://yann.lecun.com/exdb/mnist";
    const char *mnist_files[] = {
        "train-images-idx3-ubyte.gz",
        "train-labels-idx1-ubyte.gz",
        "t10k-images-idx3-ubyte.gz",
        "t10k-labels-idx1-ubyte.gz"
    };

    char url[256];
    char output_path[1024];

    for (int i = 0; i < 4; i++) {
        snprintf(url, sizeof(url), "%s/%s", base_url, mnist_files[i]);
        snprintf(output_path, sizeof(output_path), "%s/%s", dir, mnist_files[i]);

        printf("Downloading %s...\n", url);
        if (!_download_file(url, output_path)) {
            fprintf(stderr, "Failed to download %s\n", url);
            return 0;
        }
    }

    printf("All MNIST files downloaded successfully.\n");
    return 1;
}

int _load_mnist_pictures(const char* dir, float* data, size_t size, int is_train) {
    return 0;
}

int _load_mnist_labels(const char* dir, int* label, size_t size, int is_train) {
    return 0;
}

int load_mnist(const char* directory, float* X_train, int* y_train, size_t train_size, float* X_test, int* y_test, size_t test_size) {
    int must_download = 0;
    int is_dir = _directory_exists(directory);
    if (is_dir==1) {
        if (!_check_files_exists(directory)) {
            must_download = 1;
        }
    }
    else if (is_dir==2) {
        printf("Error: The path '%s' exists. But it is not directory\n",directory);
    }
    else {
        printf("Error: The directory doesn't exist\nAttempting to create it...\n");
        _create_directory(directory);
        must_download = 1;
    }
    
    if (must_download) {
        _mnist_download(directory);
    }

    _load_mnist_pictures(directory, X_train, train_size, 1);
    _load_mnist_labels(directory, y_train, train_size, 1);
    _load_mnist_pictures(directory, X_test, test_size, 0);
    _load_mnist_labels(directory, y_test, test_size, 0);

    return 0;
}