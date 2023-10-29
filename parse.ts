import type { SyntaxToken } from './lex.ts';

export type Operator = '+' | '-' | '*' | '/' | '%' | ':';
export type NodeType =
    | 'program'
    | 'statement'
    | 'binary expression'
    | 'assignment expression'
    | 'identifier'
    | 'string literal'
    | 'number literal'
    | 'unexpected'
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
    operator: Operator;
}
export interface AssignmentExpressionNode extends Node {
    type: 'assignment expression';
    left: IdentifierNode;
    right: Node;
    operator: Operator;
}
export interface IdentifierNode extends Node {
    type: 'identifier';
    name: string;
}
export interface StringLiteralNode extends Node {
    type: 'string literal';
    value: string;
}
export interface NumberLiteralNode extends Node {
    type: 'number literal';
    value: number;
}
export interface UnexpectedNode extends Node {
    type: 'unexpected';
    token: SyntaxToken;
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

        switch (currentToken?.type) {
            case 'number': {
                return {
                    type: 'number literal',
                    value: parseFloat(currentToken.value),
                } as NumberLiteralNode;
            }
            case 'string': {
                return {
                    type: 'string literal',
                    value: currentToken.value,
                } as StringLiteralNode;
            }
            case 'identifier': {
                return {
                    type: 'identifier',
                    name: currentToken.value,
                } as IdentifierNode;
            }
            case ':': {
                const operator = currentToken.value as Operator;
                const lastNode = stack.pop();
                return {
                    type: 'assignment expression',
                    operator: operator,
                    left: lastNode,
                    right: parseToken(tokens, stack),
                } as AssignmentExpressionNode;
            }
            case '+':
            case '-':
            case '*':
            case '/':
            case '%': {
                const operator = currentToken.value as Operator;

                // check if previous node was binary
                // and compare their operator precedence
                const lastNode = stack.pop();
                const hasGreaterPrecedence =
                    lastNode &&
                    lastNode.type === 'binary expression' &&
                    getOperatorPrecedence(
                        (lastNode as BinaryExpressionNode).operator
                    ) < getOperatorPrecedence(operator as Operator);
                if (hasGreaterPrecedence) {
                    const newNode = (lastNode as BinaryExpressionNode).right;
                    (lastNode as BinaryExpressionNode).right = {
                        type: 'binary expression',
                        operator: operator,
                        left: newNode,
                        right: parseToken(tokens, stack),
                    } as BinaryExpressionNode;
                    return lastNode;
                } else {
                    return {
                        type: 'binary expression',
                        operator: operator,
                        left: lastNode,
                        right: parseToken(tokens, stack),
                    } as BinaryExpressionNode;
                }
            }
            case ';': {
                return stack.pop()!;
            }
            case '(': {
                let innerToken = tokens[0];
                while (innerToken.type !== ')') {
                    const node = parseToken(tokens, stack);
                    stack.push(node);
                    innerToken = tokens[0];
                }
                tokens.shift(); // eat right parenthesis
                return {
                    type: 'statement',
                    statement: stack.pop(),
                } as StatementNode;
            }
            case 'end of file': {
                return {
                    type: 'end of file',
                } as EndOfFileNode;
            }
            default: {
                return {
                    type: 'unexpected',
                    token: currentToken,
                } as UnexpectedNode;
            }
        }
    }

    return ast;
}

function getOperatorPrecedence(operator: Operator) {
    if (operator === '*' || operator === '/' || operator === '%') {
        return 1;
    } else {
        return 0;
    }
}

export function printTree(node: Node | undefined, indent = '', isLast = true) {
    const marker = isLast ? '`---' : '|---';

    let value;
    if (node?.type === 'number literal') {
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
