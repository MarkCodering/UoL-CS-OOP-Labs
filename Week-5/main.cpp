#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <unordered_map>

class IrisRecord {
public:
    double sepalLength;
    double sepalWidth;
    double petalLength;
    double petalWidth;
    std::string species;

    IrisRecord(double sl, double sw, double pl, double pw, const std::string& sp)
        : sepalLength(sl),
          sepalWidth(sw),
          petalLength(pl),
          petalWidth(pw),
          species(sp) {}
};

// Simple CSV splitter assuming no quoted fields and no commas inside fields
std::vector<std::string> splitCSVLine(const std::string& line) {
    std::vector<std::string> result;
    std::stringstream ss(line);
    std::string item;

    while (std::getline(ss, item, ',')) {
        result.push_back(item);
    }
    return result;
}

// Parse one line into an IrisRecord, throwing on problems
IrisRecord parseIrisLine(const std::string& line) {
    std::vector<std::string> tokens = splitCSVLine(line);

    // Expecting 5 columns: sl, sw, pl, pw, species
    if (tokens.size() != 5) {
        throw std::runtime_error("Invalid number of fields: expected 5, got " +
                                 std::to_string(tokens.size()));
    }

    try {
        double sl = std::stod(tokens[0]);
        double sw = std::stod(tokens[1]);
        double pl = std::stod(tokens[2]);
        double pw = std::stod(tokens[3]);
        std::string species = tokens[4];

        return IrisRecord(sl, sw, pl, pw, species);
    } catch (const std::invalid_argument& e) {
        throw std::runtime_error("Conversion error (invalid_argument): " +
                                 std::string(e.what()));
    } catch (const std::out_of_range& e) {
        throw std::runtime_error("Conversion error (out_of_range): " +
                                 std::string(e.what()));
    }
}

int main() {
    std::string filename = "iris.csv";
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file '" << filename << "'\n";
        return 1;
    }

    std::vector<IrisRecord> records;
    std::string line;

    std::size_t totalLines = 0;
    std::size_t validLines = 0;
    std::size_t invalidLines = 0;

    // Skip header line (assuming the first line contains column names)
    if (std::getline(file, line)) {
        // Optional: you could check that it contains "sepal_length" etc.
    }

    // For simple statistics
    double sumSepalLength = 0.0;
    double sumSepalWidth  = 0.0;
    double sumPetalLength = 0.0;
    double sumPetalWidth  = 0.0;

    std::unordered_map<std::string, std::size_t> speciesCount;

    while (std::getline(file, line)) {
        ++totalLines;

        // Ignore empty lines
        if (line.empty()) {
            continue;
        }

        try {
            IrisRecord rec = parseIrisLine(line);
            records.push_back(rec);
            ++validLines;

            sumSepalLength += rec.sepalLength;
            sumSepalWidth  += rec.sepalWidth;
            sumPetalLength += rec.petalLength;
            sumPetalWidth  += rec.petalWidth;
            speciesCount[rec.species]++;

        } catch (const std::exception& e) {
            ++invalidLines;
            std::cerr << "Warning: failed to parse line " << totalLines
                      << " -> " << e.what() << "\n";
            // We continue instead of crashing
        }
    }

    file.close();

    std::cout << "Finished reading file: " << filename << "\n";
    std::cout << "Total data lines (excluding header): " << totalLines << "\n";
    std::cout << "Valid lines: " << validLines << "\n";
    std::cout << "Invalid lines: " << invalidLines << "\n\n";

    if (validLines > 0) {
        std::cout << "Summary statistics (based on valid lines):\n";
        std::cout << "Average sepal length: "
                  << (sumSepalLength / validLines) << "\n";
        std::cout << "Average sepal width:  "
                  << (sumSepalWidth  / validLines) << "\n";
        std::cout << "Average petal length: "
                  << (sumPetalLength / validLines) << "\n";
        std::cout << "Average petal width:  "
                  << (sumPetalWidth  / validLines) << "\n\n";

        std::cout << "Counts by species:\n";
        for (const auto& pair : speciesCount) {
            std::cout << "  " << pair.first << ": " << pair.second << "\n";
        }
    } else {
        std::cout << "No valid lines were parsed.\n";
    }

    return 0;
}
