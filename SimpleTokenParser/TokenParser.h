#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

namespace SimpleTokenParser {
	struct RecognizeToken { // Token �x�s��Ƶ��c
		int position;
		std::string token;
	};

	enum AutometaMode { // ���A���Ҧ�
		LETTER, // �r�����A��
		NUMBER // �ƭȪ��A��
	};

	std::string trim(const std::string& input) { // �h���r���Y���ť�
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
		static const std::vector<std::string> keyword; // ����r
		static const std::vector<char> logicalOperator; // �޿�B��l
		static const std::vector<char> mathOperator; // �ƾǹB��l
		static const std::vector<char> number; // �ƭ�
		static const std::vector<char> symbol; // �Ÿ�

		// �Ω��x�s�Ҧ��P�_���G
		std::vector<RecognizeToken> recognizeSymbol, recognizeMathOperator, recognizeLogicalOperator, recognizeKeyword, recognizeId, recognizeNumber, recognizeTwoTokenOperator, recognizeComment, error;

		std::vector<char> operatorList;
		char last = '\0', current = '\0';
		int position = -1;
		std::stringstream buffer;

		// �Ω�έp�榡 (��� �ť� Tab)
		int blankCount = 0, lineCount = 1, tabCount = 0;

		bool twoTokenAutometa = false; // 2 Token �P�_���A��
		AutometaMode mode = AutometaMode::LETTER; // �ثe���A��

		virtual void handleLetterBuffer(int position, std::string word) {
			word = trim(word);
			if (!word.empty()) {
				if (isKeyword(word)) { // ����r
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

		static bool isKeyword(std::string& word) { // �P�_�O�_������r
			for (int i = 0; i < keyword.size(); ++i) {
				if (word == keyword[i]) {
					return true;
				}
			}
			return false;
		}

		static bool isMathOperator(char token) { // �P�_�O�_���ƾǹB��l
			for (int i = 0; i < mathOperator.size(); ++i) {
				if (mathOperator[i] == token) {
					return true;
				}
			}
			return false;
		}

		static bool isLogicalOperator(char token) { // �P�_�O�_���޿�B��l
			for (int i = 0; i < logicalOperator.size(); ++i) {
				if (logicalOperator[i] == token) {
					return true;
				}
			}
			return false;
		}

		static bool isVariable(std::string& word) { // �P�_�O�_�� ID (�ܼ�)
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

		static bool isNumber(std::string& number) { // �P�_�O�_����ƩίB�I��
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

		static bool isEndToken(char value) { // �P�_�O�_�i�� Token ����
			return value == ' ' || value == '\n' || value == ';' || value == ',' || value == ')' || value == ']' || value == '}' || value == EOF;
		}

		virtual void parse(std::istream& input) {
			while (!input.eof()) {
				if (current != '\0') {
					last = current;
				}

				current = input.get();
				++position;

				// �榡�έp
				if (current == ' ') {
					++blankCount;
				}
				else if (current == '\n') {
					++lineCount;
				}
				else if (current == '\t') {
					++tabCount;
				}
				else if (current == '\r') { // �L�� ���L
					continue;
				}

				bool append = true; // �O�_�n���J�� ID ��
				for (int i = 0; i < symbol.size(); ++i) { // �Ÿ��P�_
					if (current == symbol[i]) {
						recognizeSymbol.push_back(RecognizeToken{ position, std::string(1, current) });
						append = false;
					}
				}

				if (buffer.str() == "" && isdigit(current)) { // �ƭȧP�_ : ���A������
					mode = AutometaMode::NUMBER;
				}

				if (mode == AutometaMode::LETTER) {
					// �B��l�P�_
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
								else if (current == '/' && last == '/') { // �@�����
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
								else if (current == '*' && last == '/') { // �P������
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
								else if (current == '>' && last == '<') { // <> ������Ÿ�
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

					if (!append || isEndToken(current)) { // �r�� (ID) �P�_
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

		virtual std::vector<RecognizeToken>& getRecognizeSymbol() { // �Ÿ�
			return recognizeSymbol;
		}

		virtual std::vector<RecognizeToken>& getRecognizeMathOperator() { // �ƾǹB��l
			return recognizeMathOperator;
		}

		virtual std::vector<RecognizeToken>& getRecognizeTwoTokenOperator() { // 2 Token �B��l
			return recognizeTwoTokenOperator;
		}

		virtual std::vector<RecognizeToken>& getRecognizeLogicalOperator() { // �޿�B��l
			return recognizeLogicalOperator;
		}

		virtual std::vector<RecognizeToken>& getRecognizeKeyword() { // ����r
			return recognizeKeyword;
		}

		virtual std::vector<RecognizeToken>& getRecognizeComment() { // ����
			return recognizeComment;
		}

		virtual std::vector<RecognizeToken>& getRecognizeId() { // �ܼ�
			return recognizeId;
		}

		virtual std::vector<RecognizeToken>& getRecognizeNumber() { // ��ƻP�B�I��
			return recognizeNumber;
		}

		virtual std::vector<RecognizeToken>& getError() { // ���ѿ��~
			return error;
		}

		virtual int getBlankCount() const { // �ťռ�
			return blankCount;
		}

		virtual int getLineCount() const { // ���
			return lineCount;
		}

		virtual int geTtabCount() const { // Tab ��
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