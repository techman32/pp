#include "bitmap_image.hpp"
#include <windows.h>
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <stdio.h>
#include <malloc.h>
#include <fstream>
#include <mutex>

constexpr int BLUR_WIDTH = 5;

std::mutex fileMutex;
std::chrono::high_resolution_clock::time_point programStartTime;

struct Data
{
    bitmap_image* oldImage;
    bitmap_image* newImage;
    int startRow;
    int endRow;
    int threadIndex;
};

void blur(bitmap_image* oldImage, bitmap_image* newImage, int startRow, int endRow, int threadIndex)
{
    for (int i = startRow; i < endRow; i++)
    {
        for (int p = 0; p < oldImage->width(); p += BLUR_WIDTH)
        {
            auto start = std::chrono::high_resolution_clock::now();

            int sumRed = 0;
            int sumGreen = 0;
            int sumBlue = 0;

            for (int j = p; j < p + BLUR_WIDTH; j++)
            {
                rgb_t pixel;
                oldImage->get_pixel(j, i, pixel);
                sumRed += pixel.red;
                sumGreen += pixel.green;
                sumBlue += pixel.blue;
            }

            rgb_t blurred;
            blurred.red = sumRed / BLUR_WIDTH;
            blurred.green = sumGreen / BLUR_WIDTH;
            blurred.blue = sumBlue / BLUR_WIDTH;

            for (int j = p; j < p + BLUR_WIDTH; j++)
            {
                newImage->set_pixel(j, i, blurred);
            }

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - programStartTime).count();

            std::lock_guard<std::mutex> lock(fileMutex);
            std::ofstream outfile("times.txt", std::ios_base::app);
            if (outfile.is_open())
            {
                outfile << "Thread " << threadIndex << ": Pixel operation duration: " << duration << " ms\n";
                outfile.close();
            }
        }
    }
}

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
    struct Data* tData = (struct Data*)lpParam;
    blur(tData->oldImage, tData->newImage, tData->startRow, tData->endRow, tData->threadIndex);
    ExitThread(0);
}


int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "Russian");

    if (argc != 5)
    {
        std::cerr << "Usage: " << argv[0] << " <input file> <output file> <number of cores> <number of threads>\n";
        return 1;
    }

    std::string inputFile = argv[1];
    std::string outputFile = argv[2];
    int numCores = std::stoi(argv[3]);
    int numThreads = std::stoi(argv[4]);

    programStartTime = std::chrono::high_resolution_clock::now();

    bitmap_image image(inputFile);
    bitmap_image newImage(inputFile);

    if (!image)
    {
        std::cerr << "Error: Could not open " << inputFile << std::endl;
        return 1;
    }

    HANDLE* handles = new HANDLE[numThreads];

    int rows_per_thread = image.height() / numThreads;

    newImage.setwidth_height(image.width() / BLUR_WIDTH * BLUR_WIDTH, image.height());

    for (int a = 0; a < 1; a++)
    {
        for (int i = 0; i < (numThreads); i++)
        {
            int start_row = i * rows_per_thread;
            int end_row = (i == numThreads - 1) ? image.height() : (i + 1) * rows_per_thread;

            Data* tData = new Data();

            tData->oldImage = &image;
            tData->newImage = &newImage;
            tData->startRow = start_row;
            tData->endRow = end_row;
            tData->threadIndex = i;

            handles[i] = CreateThread(NULL, 0, &ThreadProc, (LPVOID)tData, CREATE_SUSPENDED, NULL);

            if (handles[i] == NULL) continue;

            int priority;
            if (i == 0) {
                priority = THREAD_PRIORITY_ABOVE_NORMAL;
            }
            else {
                priority = THREAD_PRIORITY_NORMAL;
            }
            SetThreadPriority(handles[i], priority);

            //DWORD_PTR affinity_mask = (DWORD_PTR)1 << (i % numCores);
            //SetThreadAffinityMask(handles[i], affinity_mask);
        }

        for (int i = 0; i < numThreads; i++)
        {
            if (handles[i] == NULL)
            {
                std::cerr << "Error when creating a thread " << i << std::endl;
                return 1;
            }
            ResumeThread(handles[i]);
        }

        WaitForMultipleObjects(numThreads, handles, TRUE, INFINITE);
    }
    newImage.save_image(outputFile);

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - programStartTime).count();

    std::cout << "Time: " << duration << " ms" << std::endl;
    return 0;
}