#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

namespace SimpleTokenParser {
	struct RecognizeToken { // Token 儲存資料結構
		int position;
		std::string token;
	};

	enum AutometaMode { // 狀態機模式
		LETTER, // 字元狀態機
		NUMBER // 數值狀態機
	};

	std::string trim(const std::string& input) { // 去除字串頭尾空白
		std::string::size_type first = input.find_first_not_of(' ');
		if (first == std::string::npos) {
			return input;
		}
		std::string::size_type last = input.find_last_not_of(' ');
		if (last != std::string::npos) {
			return input.substr(first, last - first + 1);
		}
		return input.substr(first);
	}

	class TokenParser {
	private:
	protected:
		static const std::vector<std::string> keyword; // 關鍵字
		static const std::vector<char> logicalOperator; // 邏輯運算子
		static const std::vector<char> mathOperator; // 數學運算子
		static const std::vector<char> number; // 數值
		static const std::vector<char> symbol; // 符號

		// 用於儲存所有判斷結果
		std::vector<RecognizeToken> recognizeSymbol, recognizeMathOperator, recognizeLogicalOperator, recognizeKeyword, recognizeId, recognizeNumber, recognizeTwoTokenOperator, recognizeComment, error;

		std::vector<char> operatorList;
		char last = '\0', current = '\0';
		int position = -1;
		std::stringstream buffer;

		// 用於統計格式 (行數 空白 Tab)
		int blankCount = 0, lineCount = 1, tabCount = 0;

		bool twoTokenAutometa = false; // 2 Token 判斷狀態機
		AutometaMode mode = AutometaMode::LETTER; // 目前狀態機

		virtual void handleLetterBuffer(int position, std::string word) {
			word = trim(word);
			if (!word.empty()) {
				if (isKeyword(word)) { // 關鍵字
					recognizeKeyword.push_back(RecognizeToken{ position - (int)word.length(), word });
				}
				else if (isVariable(word)) {
					recognizeId.push_back(RecognizeToken{ position - (int)word.length(), word });
				}
				else {
					error.push_back(RecognizeToken{ position - (int)word.length(), word });
				}
			}
		}

		virtual void handleNumberBuffer(int position, std::string number) {
			number = trim(number);
			if (!number.empty()) {
				if (isNumber(number)) {
					recognizeNumber.push_back(RecognizeToken{ position - (int)number.length(), number });
				}
				else {
					error.push_back(RecognizeToken{ position - (int)number.length(), number });
				}
			}
		}
	public:
		TokenParser() {
			for (int i = 0; i < mathOperator.size(); ++i) {
				operatorList.push_back(mathOperator[i]);
			}
			for (int i = 0; i < logicalOperator.size(); ++i) {
				operatorList.push_back(logicalOperator[i]);
			}
		}

		static bool isKeyword(std::string& word) { // 判斷是否為關鍵字
			for (int i = 0; i < keyword.size(); ++i) {
				if (word == keyword[i]) {
					return true;
				}
			}
			return false;
		}

		static bool isMathOperator(char token) { // 判斷是否為數學運算子
			for (int i = 0; i < mathOperator.size(); ++i) {
				if (mathOperator[i] == token) {
					return true;
				}
			}
			return false;
		}

		static bool isLogicalOperator(char token) { // 判斷是否為邏輯運算子
			for (int i = 0; i < logicalOperator.size(); ++i) {
				if (logicalOperator[i] == token) {
					return true;
				}
			}
			return false;
		}

		static bool isVariable(std::string& word) { // 判斷是否為 ID (變數)
			if (word[0] != '_' && !isalpha(word[0])) {
				return false;
			}

			for (int i = 1; i < word.length(); ++i) {
				if (!isalpha(word[i]) && !isdigit(word[i]) && word[i] != '_') {
					return false;
				}
			}
			return true;
		}

		static bool isNumber(std::string& number) { // 判斷是否為整數或浮點數
			if (number[0] == '.' || !isdigit(number[0])) {
				return false;
			}

			bool hasDot = false, hasExponent = false;
			for (int i = 1; i < number.length(); ++i) {
				if (number[i] == '.') {
					if (hasDot) {
						return false;
					}
					if (i + 1 >= number.length() || !isdigit(number[i + 1])) {
						return false;
					}
					hasDot = true;
				}
				else if (number[i] == 'E') {
					if (hasExponent) {
						return false;
					}
					hasExponent = true;

					if (i + 2 >= number.length()) {
						return false;
					}
					char next = number[++i];
					if (next != '+' && next != '-') {
						return false;
					}

					char digit = number[++i];
					if (!isdigit(digit)) {
						return false;
					}
				}
				else if (!isdigit(number[i])) {
					return false;
				}
			}
			return true;
		}

		static bool isEndToken(char value) { // 判斷是否進行 Token 切割
			return value == ' ' || value == '\n' || value == ';' || value == ',' || value == ')' || value == ']' || value == '}' || value == EOF;
		}

		virtual void parse(std::istream& input) {
			while (!input.eof()) {
				if (current != '\0') {
					last = current;
				}

				current = input.get();
				++position;

				// 格式統計
				if (current == ' ') {
					++blankCount;
				}
				else if (current == '\n') {
					++lineCount;
				}
				else if (current == '\t') {
					++tabCount;
				}
				else if (current == '\r') { // 無用 跳過
					continue;
				}

				bool append = true; // 是否要接入到 ID 中
				for (int i = 0; i < symbol.size(); ++i) { // 符號判斷
					if (current == symbol[i]) {
						recognizeSymbol.push_back(RecognizeToken{ position, std::string(1, current) });
						append = false;
					}
				}

				if (buffer.str() == "" && isdigit(current)) { // 數值判斷 : 狀態機切換
					mode = AutometaMode::NUMBER;
				}

				if (mode == AutometaMode::LETTER) {
					// 運算子判斷
					bool operatorCheckState = false, operatorMatch = false;
					for (int i = 0; i < operatorList.size(); ++i) {
						if (!twoTokenAutometa) {
							if (current == operatorList[i]) {
								twoTokenAutometa = true;
								append = false;
								break;
							}
						}
						else {
							operatorCheckState = true;
							if (current == operatorList[i]) {
								if (current == '=') {
									std::stringstream ss;
									ss << last << "=";
									recognizeTwoTokenOperator.push_back(RecognizeToken{ position - 1, ss.str() });
									operatorMatch = true;
								}
								else if (current == '/' && last == '/') { // 一般註解
									int currentPosition = position - 1;
									std::stringstream comment;
									comment << "//";
									while (input.peek() != EOF) {
										char skip = input.get();
										++position;
										if (skip == '\n') {
											break;
										}
										comment << skip;
									}
									recognizeComment.push_back(RecognizeToken{ currentPosition, comment.str() });
									operatorMatch = true;
								}
								else if (current == '*' && last == '/') { // 星號註解
									int currentPosition = position - 1;
									std::stringstream comment;
									comment << "/*";
									while (input.peek() != EOF) {
										char skip = input.get();
										++position;
										if (skip == '/' && comment.str().back() == '*') {
											comment << skip;
											break;
										}
										comment << skip;
									}
									recognizeComment.push_back(RecognizeToken{ currentPosition, comment.str() });
									operatorMatch = true;
								}
								else if (current == last && (current == '+' || current == '-' || current == '<' || current == '>' || current == '&' || current == '|')) {
									// ++ / -- / << / >> / && / ||
									std::stringstream ss;
									ss << last << current;
									recognizeTwoTokenOperator.push_back(RecognizeToken{ position - 1, ss.str() });
									operatorMatch = true;
								}
								else if (current == '>' && last == '<') { // <> 不等於符號
									recognizeTwoTokenOperator.push_back(RecognizeToken{ position - 1, "<>" });
									operatorMatch = true;
								}
							}
						}
					}
					if (operatorCheckState && twoTokenAutometa) {
						if (!operatorMatch) {
							if (isMathOperator(last)) {
								recognizeMathOperator.push_back(RecognizeToken{ position - 1, std::string(1, last) });
							}
							else {
								recognizeLogicalOperator.push_back(RecognizeToken{ position - 1, std::string(1, last) });
							}
						}
						twoTokenAutometa = false;
						if (append) {
							append = !operatorMatch;
						}
					}

					if (!append || isEndToken(current)) { // 字母 (ID) 判斷
						// Word Token Accept
						handleLetterBuffer(position, buffer.str());
						buffer.str("");
					}
					else {
						buffer << current;
					}
				}
				else {
					if (isEndToken(current)) {
						// Number Token Accept
						handleNumberBuffer(position, buffer.str());
						buffer.str("");
						mode = AutometaMode::LETTER;
					}
					else {
						buffer << current;
					}
				}
			}
		}

		virtual std::vector<RecognizeToken>& getRecognizeSymbol() { // 符號
			return recognizeSymbol;
		}

		virtual std::vector<RecognizeToken>& getRecognizeMathOperator() { // 數學運算子
			return recognizeMathOperator;
		}

		virtual std::vector<RecognizeToken>& getRecognizeTwoTokenOperator() { // 2 Token 運算子
			return recognizeTwoTokenOperator;
		}

		virtual std::vector<RecognizeToken>& getRecognizeLogicalOperator() { // 邏輯運算子
			return recognizeLogicalOperator;
		}

		virtual std::vector<RecognizeToken>& getRecognizeKeyword() { // 關鍵字
			return recognizeKeyword;
		}

		virtual std::vector<RecognizeToken>& getRecognizeComment() { // 註解
			return recognizeComment;
		}

		virtual std::vector<RecognizeToken>& getRecognizeId() { // 變數
			return recognizeId;
		}

		virtual std::vector<RecognizeToken>& getRecognizeNumber() { // 整數與浮點數
			return recognizeNumber;
		}

		virtual std::vector<RecognizeToken>& getError() { // 辨識錯誤
			return error;
		}

		virtual int getBlankCount() const { // 空白數
			return blankCount;
		}

		virtual int getLineCount() const { // 行數
			return lineCount;
		}

		virtual int geTtabCount() const { // Tab 數
			return tabCount;
		}
	};

	const std::vector<std::string> TokenParser::keyword = {
		"auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else",
		"enum", "extern", "float", "for", "goto", "if", "int", "long", "register", "return",
		"short", "signed", "sizeof", "static", "struct", "switch", "typedef", "union", "unsigned", "void",
		"volatile", "while", "bool", "true", "false", "class", "template", "typename", "virtual"
	};

	const std::vector<char> TokenParser::logicalOperator = { '>', '<', '!', '|', '&', '^' };
	const std::vector<char> TokenParser::mathOperator = { '+', '-', '*', '/', '%', '=' };
	const std::vector<char> TokenParser::number = { '.', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
	const std::vector<char> TokenParser::symbol = { ',', ';', '\\', '(', ')', '{', '}', '[', ']', '\'', ':', '"', '~' };
}