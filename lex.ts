type TokenType =
    | 'left parenthesis'
    | 'right parenthesis'
    | 'plus'
    | 'minus'
    | 'star'
    | 'forward slash'
    | 'modulo'
    | 'end of file'
    | 'unknown'
    | 'number'
    | 'identifier';
export type SyntaxToken = {
    type: TokenType;
    value: string;
};

const IS_LETTER = /[a-z/]/i;
const IS_WHITESPACE = /\s/;
const IS_NUMBER = /\d/;

export function lex(input: string) {
    const tokens: SyntaxToken[] = [];
    let currentPosition = 0;
    while (currentPosition <= input.length) {
        let character = input[currentPosition];

        if (!character) {
            tokens.push({
                type: 'end of file',
                value: '',
            });
            currentPosition++;
        } else if (character === '(') {
            tokens.push({
                type: 'left parenthesis',
                value: '(',
            });
            currentPosition++;
        } else if (character === ')') {
            tokens.push({
                type: 'right parenthesis',
                value: ')',
            });
            currentPosition++;
        } else if (character === '+') {
            tokens.push({
                type: 'plus',
                value: '+',
            });
            currentPosition++;
        } else if (character === '-') {
            tokens.push({
                type: 'minus',
                value: '-',
            });
            currentPosition++;
        } else if (character === '*') {
            tokens.push({
                type: 'star',
                value: '*',
            });
            currentPosition++;
        } else if (character === '/') {
            tokens.push({
                type: 'forward slash',
                value: '/',
            });
            currentPosition++;
        } else if (character === '%') {
            tokens.push({
                type: 'modulo',
                value: '%',
            });
            currentPosition++;
        } else if (IS_LETTER.test(character)) {
            let value = '';
            while (IS_LETTER.test(character)) {
                value += character;
                character = input[++currentPosition];
            }
            tokens.push({
                type: 'identifier',
                value,
            });
        } else if (IS_WHITESPACE.test(character)) {
            currentPosition++;
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
        }
    }
    return tokens;
}
