type TokenType =
    | 'left parenthesis'
    | 'right parenthesis'
    | 'plus'
    | 'minus'
    | 'star'
    | 'forward slash'
    | 'modulo'
    | 'colon'
    | 'semicolon'
    | 'double quote'
    | 'end of file'
    | 'unknown'
    | 'number'
    | 'string'
    | 'identifier';
export type SyntaxToken = {
    type: TokenType;
    value: string;
};

const IS_LETTER = /[a-z/]/i;
const IS_NUMBER = /\d/;

export function lex(input: string) {
    const tokens: SyntaxToken[] = [];
    let currentPosition = 0;
    while (currentPosition <= input.length) {
        let character = input[currentPosition];

        switch (character) {
            case ' ':
            case '\n': {
                currentPosition++;
                break;
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
                });
                break;
            }
            case ':': {
                tokens.push({
                    type: 'colon',
                    value: ':',
                });
                currentPosition++;
                break;
            }
            case ';': {
                tokens.push({
                    type: 'semicolon',
                    value: ';',
                });
                currentPosition++;
                break;
            }
            case '(': {
                tokens.push({
                    type: 'left parenthesis',
                    value: '(',
                });
                currentPosition++;
                break;
            }
            case ')': {
                tokens.push({
                    type: 'right parenthesis',
                    value: ')',
                });
                currentPosition++;
                break;
            }
            case '+': {
                tokens.push({
                    type: 'plus',
                    value: '+',
                });
                currentPosition++;
                break;
            }
            case '-': {
                tokens.push({
                    type: 'minus',
                    value: '-',
                });
                currentPosition++;
                break;
            }
            case '*': {
                tokens.push({
                    type: 'star',
                    value: '*',
                });
                currentPosition++;
                break;
            }
            case '/': {
                tokens.push({
                    type: 'forward slash',
                    value: '/',
                });
                currentPosition++;
                break;
            }
            case '%': {
                tokens.push({
                    type: 'modulo',
                    value: '%',
                });
                currentPosition++;
                break;
            }
            case undefined: {
                tokens.push({
                    type: 'end of file',
                    value: '',
                });
                currentPosition++;
                break;
            }
            default: {
                if (IS_LETTER.test(character)) {
                    let value = '';
                    while (IS_LETTER.test(character)) {
                        value += character;
                        character = input[++currentPosition];
                    }
                    tokens.push({
                        type: 'identifier',
                        value,
                    });
                } else if (IS_NUMBER.test(character)) {
                    let value = '';
                    while (IS_NUMBER.test(character)) {
                        value += character;
                        character = input[++currentPosition];
                    }
                    tokens.push({
                        type: 'number',
                        value,
                    });
                } else {
                    tokens.push({
                        type: 'unknown',
                        value: character,
                    });
                    currentPosition++;
                }
            }
        }
    }
    return tokens;
}
