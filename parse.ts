import type { Token } from './lex.ts';
export function parse(tokens: Token[]) {
    let current = 0;
    function walk() {
        let token = tokens[current];

        if (token.type === 'number') {
            current++;
            return {
                type: 'NumberLiteral',
                value: token.value,
            };
        }
        if (token.type === 'parenthesis' && token.value === '(') {
            token = tokens[++current];
            const expression: any = {
                type: 'CallExpression',
                name: token.value,
                params: [],
            };
            token = tokens[++current];
            while (token.value !== ')') {
                expression.params.push(walk());
                token = tokens[current];
            }
            current++;
            return expression;
        }
    }
    const ast = {
        type: 'Program',
        body: [walk()],
    };
    return ast;
}
