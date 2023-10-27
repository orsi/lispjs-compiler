import type { SyntaxToken } from './lex.ts';

export type NodeType =
    | 'program'
    | 'statement'
    | 'binary expression'
    | 'string'
    | 'number'
    | 'unknown'
    | 'end of file';
export type Node = {
    type: NodeType;
};
export interface ProgramNode extends Node {
    type: 'program';
    body: Node[];
}
export interface StatementNode extends Node {
    type: 'statement';
    statement: Node;
}
export interface BinaryExpressionNode extends Node {
    type: 'binary expression';
    left: Node;
    right: Node;
    operator: string;
}
export interface StringLiteralNode extends Node {
    type: 'string';
    value: string;
}
export interface NumberLiteralNode extends Node {
    type: 'number';
    value: number;
}
export interface UnknownNode extends Node {
    type: 'unknown';
    value: string;
}
export interface EndOfFileNode extends Node {
    type: 'end of file';
}

export function parse(tokens: SyntaxToken[]) {
    const ast: ProgramNode = {
        type: 'program',
        body: [],
    };

    while (tokens.length > 0) {
        ast.body.push(parseToken(tokens, ast.body));
    }

    function parseToken(tokens: SyntaxToken[], stack: Node[]): Node {
        const currentToken = tokens.shift();
        if (!currentToken) {
            // we did something wrong here didn't we?
            throw new Error('whoops');
        } else if (currentToken.type === 'end of file') {
            return {
                type: 'end of file',
            } as EndOfFileNode;
        } else if (currentToken.type === 'number') {
            return {
                type: 'number',
                value: parseFloat(currentToken.value),
            } as NumberLiteralNode;
        } else if (
            currentToken.type === 'plus' ||
            currentToken.type === 'minus' ||
            currentToken.type === 'star' ||
            currentToken.type === 'forward slash'
        ) {
            return {
                type: 'binary expression',
                operator: currentToken.value,
                left: stack.pop()!,
                right: parseToken(tokens, stack),
            } as BinaryExpressionNode;
        } else if (currentToken.type === 'left parenthesis') {
            let innerToken = tokens[0];
            while (innerToken.type !== 'right parenthesis') {
                const node = parseToken(tokens, stack);
                stack.push(node);
                innerToken = tokens[0];
            }
            tokens.shift(); // eat right parenthesis
            return {
                type: 'statement',
                statement: stack.pop(),
            } as StatementNode;
        } else {
            return {
                type: 'unknown',
                value: currentToken.value,
            } as UnknownNode;
        }
    }

    return ast;
}

export function printTree(node: Node | undefined, indent = '', isLast = true) {
    const marker = isLast ? '`---' : '|---';

    let value;
    if (node?.type === 'number') {
        value = (node as NumberLiteralNode).value;
    } else if (node?.type === 'binary expression') {
        value = (node as BinaryExpressionNode).operator;
    }

    console.log(`${indent}${marker}${node?.type}: ${value}`);

    indent += isLast ? '    ' : '|   ';

    if (node?.type === 'binary expression') {
        const left = (node as BinaryExpressionNode).left;
        const right = (node as BinaryExpressionNode).right;
        printTree(left, indent, left?.type !== 'binary expression');
        printTree(right, indent, right?.type !== 'binary expression');
    }
}
