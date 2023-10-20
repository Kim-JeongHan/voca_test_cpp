﻿#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>
#include <utility>
#include <regex>







class TestVoca
{
private:
    std::string filename_;
    std::vector<std::pair<std::string, std::string>> words_meanings_;
    std::vector<int> indices_;
    int score_ = 0;

    std::regex pattern_{ "\\s+" };

    void ReadFile_()
    {
        std::ifstream file(filename_+ ".csv");
        if (!file.is_open()) {
            std::cout << "File not found.\n";
            return;
        }
        std::string line;
        while (std::getline(file, line)) {
            size_t pos = line.find(",");
            if (pos != std::string::npos) {
                words_meanings_.emplace_back(line.substr(0, pos), line.substr(pos + 1));
            }
        }
        
        file.close();
    }

    void shuffle_(const long int words_size)
    {
        indices_.resize(words_size);
        std::iota(indices_.begin(), indices_.end(), 0);
        std::random_device rd;
        std::default_random_engine eng(rd());
        std::shuffle(indices_.begin(), indices_.end(), eng);
    }

    void save_(const std::vector<std::pair<std::string, std::string>> &  words)
    {
        std::ofstream output_file(filename_+ "_wrong.csv");
        for (int i = 0; i < words.size(); ++i) {
            output_file << words[i].first << ", " << words[i].second << std::endl;
        }
        output_file.close();
    }

    bool ContainsComma_(const std::string& str) {
        std::regex pattern(",");
        return std::regex_search(str, pattern);
    }

    std::vector<std::string> SplitAndSort_(const std::string& input) {
        std::vector<std::string> result;
        std::regex pattern(",");
        std::sregex_token_iterator iter(input.begin(), input.end(), pattern, -1);
        std::sregex_token_iterator end;
        for (; iter != end; ++iter) {
            std::string token = *iter;
            //token.erase(std::remove_if(token.begin(), token.end(), ::isspace), token.end());
            result.push_back(token);
        }
        std::sort(result.begin(), result.end());
        return result;
    }

    bool AreVectorsEqual_(
        const std::vector<std::string>& vec1, 
        const std::vector<std::string>& vec2) 
    {
        if (vec1.size() != vec2.size()) {
            return false;
        }
        for (std::size_t i = 0; i < vec1.size(); ++i) {
            if (vec1[i] != vec2[i]) {
                return false;
            }
        }
        return true;
    }

public:
    TestVoca(const std::string& filename)
    : filename_(filename)
    {
        ReadFile_();
        runTest();
    }
    void runTest()
    {
        shuffle_(words_meanings_.size());
        std::vector<std::pair<std::string, std::string>> wrong_words, final_wrong_words;
        Test(words_meanings_, wrong_words);
        printScore(words_meanings_.size());

        shuffle_(wrong_words.size());
        Test(wrong_words, final_wrong_words);
        printScore(wrong_words.size());
        printWrongVoca(wrong_words);
        save_(final_wrong_words);

    }
    void Test(std::vector<std::pair<std::string, std::string>> & words_meanings,
              std::vector<std::pair<std::string, std::string>> & wrong_words)
    {
        for (int i : indices_) {
            std::cout << "What is the meaning of " << words_meanings[i].first << "? ";
            std::string answer;
            std::getline(std::cin, answer);
            // remove all whitespaces
            answer = std::regex_replace(answer, pattern_, "");

            // remove leading and trailing whitespaces
            if (words_meanings[i].second.front() == '\"') {
                words_meanings[i].second.erase(words_meanings[i].second.begin());
            }
            // remove leading and trailing whitespaces
            if (words_meanings[i].second.back() == '\"') {
                words_meanings[i].second.pop_back();
            }
            std::string second_result = std::regex_replace(words_meanings[i].second, pattern_, "");

            if (ContainsComma_(second_result)) {
                std::vector<std::string> answers;
                std::vector<std::string> responses;
                answers = SplitAndSort_(answer);
                responses = SplitAndSort_(second_result);

                if (!AreVectorsEqual_(answers, responses)) {
                    wrong_words.push_back(words_meanings[i]);
                    std::cout << "Incorrect. The correct answer is: " << words_meanings[i].second << "\n";
                }
            
                else {
                    score_++;
                }
            }
            else {
                if (answer != second_result) {
                    wrong_words.push_back(words_meanings[i]);
                    std::cout << "Incorrect. The correct answer is: " << words_meanings[i].second << "\n";
                }
                else {
                    score_++;
                }
            }
        
        }
    }

    void printScore(const long int total_score)
    {
        std::cout << "Score: " << score_ << " / " << total_score << std::endl;
    }

    void printWrongVoca(std::vector<std::pair<std::string, std::string>> & wrong_words)
    {
        std::cout << "The following words were answered incorrectly: \n";
        for (const auto& word : wrong_words) {
            std::cout << word.first << ":" << word.second << "\n";
        }
    }

};  



int main(int argc, char* argv[])
{
    std::string filename;
    filename = argv[1];
    filename = "./words/"+ filename;
 
    TestVoca test(filename);


    return 0;
}