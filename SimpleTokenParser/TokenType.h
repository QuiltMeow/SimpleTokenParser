#pragma once

namespace SimpleTokenParser {
	public enum TokenType {
		SYMBOL,
		MATH_OPERATOR,
		TWO_TOKEN_OPERATOR,
		LOGIC_OPERATOR,
		KEY_WORD,
		COMMENT,
		VARIABLE,
		NUMBER_TOKEN,
		ERROR_TOKEN
	};
}