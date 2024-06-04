#include <algorithm>
#include <cmath>
#include <cstring>
#include <numeric>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <filesystem>
#include "lib.h"

namespace fs = std::filesystem;


float calculateNormalizedMedian(unsigned int histogramValues[], int size) {
    int totalCount = 0;
    for (int i = 0; i < size; i++) {
        totalCount += histogramValues[i];
    }

    int middle = totalCount / 2;
    int cumulativeCount = 0;
    float normalizedMedian = 0.0;

    for (int i = 0; i < size; i++) {
        cumulativeCount += histogramValues[i];
        if (cumulativeCount >= middle) {
            normalizedMedian = i + (middle - (cumulativeCount - histogramValues[i])) / (float)histogramValues[i];
            break;
        }
    }

    return normalizedMedian;
}

double isHistogramEqualized(unsigned int histogram[], int size) {
    int maxFrequency = 0;
    int expectedFrequency;

    // Calculate the maximum frequency in the histogram
    for (int i = 0; i < size; i++) {
        if (histogram[i] > maxFrequency) {
            maxFrequency = histogram[i];
        }
    }

    // Calculate the expected frequency if the histogram is perfectly equalized
    expectedFrequency = maxFrequency / size;

    // Calculate the difference between actual and expected frequencies
    int diff = 0;
    for (int i = 0; i < size; i++) {
        diff += abs((int)histogram[i] - expectedFrequency);
    }

    // Set a threshold for equality based on the histogram length
    int threshold = 0.1 * size;

    // If the difference is below the threshold, consider the histogram equalized
    return diff;
}

unsigned int underexposed(unsigned int histogram[], int size) {
    return std::accumulate(histogram, histogram + size / 2, 0u);
}

unsigned int overexposed(unsigned int histogram[], int size) {
    return std::accumulate(histogram + size / 2, histogram + size, 0u);
}

unsigned int highlights(unsigned int histogram[], int size) {
    return std::accumulate(histogram + int(size * 0.75), histogram + size, 0u);
}

#define prnt(format, ...) printf(format, __VA_ARGS__ );fprintf(html, format, __VA_ARGS__ );

int main() {
    char filename[64];
    sprintf(filename, "all_hist.csv");
    FILE* of = fopen(filename, "w");
    FILE* html = fopen("result.html", "wt");
    fprintf(
        html,
        "<html lang='en'> "
        "<head> "
        "<meta charset='UTF-8'> <meta name='viewport' content='width=device-width, initial-scale=1.0'>"
        " <title>TLVIEW</title>"
        "<link rel='stylesheet' href='style.css'>"
        "</head> "
        "<body>");


    for (const auto& entry : fs::directory_iterator("tl")) {
        if (!entry.is_regular_file() || strcasecmp(entry.path().extension().string().c_str(), ".jpg") != 0) continue;
        auto filename = entry.path().filename().c_str();


        read_jpeg_file(entry.path().c_str());
        fprintf(html, "<div id='root' >\n<img class='img' src='%s'></img>\n",
                entry.path().c_str());

        auto u = underexposed(histogram, std::size(histogram));
        auto o = overexposed(histogram, std::size(histogram));
        auto h = highlights(histogram, std::size(histogram));
        uint overexposed = 100 * o / (u + o);
        uint underexposed = 100 * u / (u + o);
        uint hi = 100 * h / (u + o);
        auto max = *std::max_element(histogram, histogram + 256);

        if (hi > 30 && underexposed <= 51 || (histogram[255] == max && underexposed < 70))
            fprintf(html, "<div class='histogram over'  >\n");
        else
            fprintf(html, "<div class='histogram'  >\n");
        for (auto h : histogram) {
            fprintf(html, "<div class='bar' style='height: %d%%' ></div>\n", 100 * h / (max));
        }
        fprintf(html, "</div>\n");


        float median = calculateNormalizedMedian(histogram, 256);
        double diff = isHistogramEqualized(histogram, 256);
        int threshold = 0.1 * 256;
        prnt("%s: ev=%.02f,  mean = %.02f,lum(mean) = %.02f, median = %.02f, lum(med)=%.02f", entry.path().c_str(), luminance,
             mean,
             lum(mean),
             median, lum(median));
        prnt(", 1/2.2 = %.02f", std::pow(mean/255, 1/2.2));
        prnt(", 2.2 = %.02f", std::pow(mean/255, 2.2));
        prnt(", diff = %.02f %d ", diff/256, threshold);
        prnt(",  %f ", (diff - threshold));


        prnt(",  under=%d%% over = %d%%", underexposed, overexposed);
        prnt(",  clipped=%f", clipped);
        prnt(",  highlights=%d%%", 100*h / (u+o));

        fprintf(html, "</div>\n");


        printf("\n");
        // printf("%d %s: %f, %f, mean = %f\n", i, filename, luminance, clipped, mean);

        fprintf(of, "%s, ", filename);

        for (int h = 0; h < 256; h++) {
            fprintf(of, "%d", histogram[h]);
            fprintf(of, ", ");
        }
        fprintf(of, "\n");
    }
    fprintf(html, "</body></html>\n");


    fclose(of);
    fclose(html);
}
