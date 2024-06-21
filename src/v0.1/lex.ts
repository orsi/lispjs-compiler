type Symbol =
    | '('
    | ')'
    | '{'
    | '}'
    | '+'
    | '-'
    | '*'
    | '/'
    | '%'
    | '>'
    | '<'
    | ':'
    | ';'
    | '"';
type Literal = 'number' | 'string' | 'identifier';
const KEYWORDS = ['if', 'else'] as const;
type Keyword = (typeof KEYWORDS)[number];
type Something = 'end of file' | 'unknown';
export type SyntaxToken = {
    type: Symbol | Literal | Something | Keyword;
    value: string;
    column: number;
    line: number;
};

const isAlpha = (input: string) => /[a-z/]/i.test(input);
const isNumber = (input: string) => /\d/.test(input);

export function lex(input: string) {
    const tokens: SyntaxToken[] = [];
    let currentPosition = 0;
    let currentRow = 1;
    let currentColumn = 1;
    while (currentPosition <= input.length) {
        let character = input[currentPosition];

        switch (character) {
            case ' ': {
                currentPosition++;
                currentColumn++;
                continue;
            }
            case '\n': {
                currentPosition++;
                currentRow++;
                currentColumn = 1;
                continue;
            }
            case '(': {
                tokens.push({
                    type: '(',
                    value: character,
                    column: currentColumn,
                    line: currentRow,
                });
                currentPosition++;
                currentColumn++;
                continue;
            }
            case ')': {
                tokens.push({
                    type: ')',
                    value: character,
                    column: currentColumn,
                    line: currentRow,
                });
                currentPosition++;
                currentColumn++;
                continue;
            }
            case '{': {
                tokens.push({
                    type: '{',
                    value: character,
                    column: currentColumn,
                    line: currentRow,
                });
                currentPosition++;
                currentColumn++;
                continue;
            }
            case '}': {
                tokens.push({
                    type: '}',
                    value: character,
                    column: currentColumn,
                    line: currentRow,
                });
                currentPosition++;
                currentColumn++;
                continue;
            }
            case '"': {
                let value = character;
                character = input[++currentPosition];
                while (character !== '"') {
                    value += character;
                    character = input[++currentPosition];
                }
                value += character;
                character = input[++currentPosition];
                tokens.push({
                    type: 'string',
                    value,
                    column: currentColumn,
                    line: currentRow,
                });
                currentColumn += value.length;
                continue;
            }
            case ':': {
                tokens.push({
                    type: ':',
                    value: character,
                    column: currentColumn,
                    line: currentRow,
                });
                currentPosition++;
                currentColumn++;
                continue;
            }
            case ';': {
                tokens.push({
                    type: ';',
                    value: character,
                    column: currentColumn,
                    line: currentRow,
                });
                currentPosition++;
                currentColumn++;
                continue;
            }
            case '+': {
                tokens.push({
                    type: '+',
                    value: '+',
                    column: currentColumn,
                    line: currentRow,
                });
                currentPosition++;
                currentColumn++;
                continue;
            }
            case '-': {
                tokens.push({
                    type: '-',
                    value: '-',
                    column: currentColumn,
                    line: currentRow,
                });
                currentPosition++;
                currentColumn++;
                continue;
            }
            case '*': {
                tokens.push({
                    type: '*',
                    value: '*',
                    column: currentColumn,
                    line: currentRow,
                });
                currentPosition++;
                currentColumn++;
                continue;
            }
            case '/': {
                tokens.push({
                    type: '/',
                    value: character,
                    column: currentColumn,
                    line: currentRow,
                });
                currentPosition++;
                currentColumn++;
                continue;
            }
            case '%': {
                tokens.push({
                    type: '%',
                    value: character,
                    column: currentColumn,
                    line: currentRow,
                });
                currentPosition++;
                currentColumn++;
                continue;
            }
            case '<': {
                tokens.push({
                    type: '<',
                    value: character,
                    column: currentColumn,
                    line: currentRow,
                });
                currentPosition++;
                currentColumn++;
                continue;
            }
            case '>': {
                tokens.push({
                    type: '>',
                    value: character,
                    column: currentColumn,
                    line: currentRow,
                });
                currentPosition++;
                currentColumn++;
                continue;
            }
            case undefined: {
                tokens.push({
                    type: 'end of file',
                    value: '',
                    column: currentColumn,
                    line: currentRow,
                });
                currentPosition++;
                currentColumn++;
                continue;
            }
            default: {
                if (isAlpha(character)) {
                    let value = '';
                    while (isAlpha(character)) {
                        value += character;
                        character = input[++currentPosition];
                    }

                    switch (value) {
                        case 'if': {
                            tokens.push({
                                type: 'if',
                                value,
                                column: currentColumn,
                                line: currentRow,
                            });
                            currentColumn += value.length;
                            continue;
                        }
                        case 'else': {
                            tokens.push({
                                type: 'else',
                                value,
                                column: currentColumn,
                                line: currentRow,
                            });
                            currentColumn += value.length;
                            continue;
                        }
                        default: {
                            tokens.push({
                                type: 'identifier',
                                value,
                                column: currentColumn,
                                line: currentRow,
                            });
                            currentColumn += value.length;
                            continue;
                        }
                    }
                } else if (isNumber(character)) {
                    let value = '';
                    while (isNumber(character)) {
                        value += character;
                        character = input[++currentPosition];
                    }
                    tokens.push({
                        type: 'number',
                        value,
                        column: currentColumn,
                        line: currentRow,
                    });
                    currentColumn += value.length;
                    continue;
                } else {
                    tokens.push({
                        type: 'unknown',
                        value: character,
                        column: currentColumn,
                        line: currentRow,
                    });
                    currentPosition++;
                    currentColumn++;
                    continue;
                }
            }
        }
    }
    return tokens;
}
