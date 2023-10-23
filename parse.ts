import type { SyntaxToken } from './lex.ts';

type NodeType = 'program' | 'binary expression' | 'factor';
type Node = {
    type: NodeType;
    token: SyntaxToken | null;
    body: Node[];
};
interface ProgramNode extends Node {
    type: 'program';
    token: null;
}
interface BinaryExpressionNode extends Node {
    type: 'binary expression';
    body: [Node, Node];
}
interface FactorNode extends Node {
    type: 'factor';
    token: SyntaxToken;
}

export function parse(tokens: SyntaxToken[]) {
    let currentTokenPosition = 0;

    function getFactor(token: SyntaxToken) {
        return {
            type: 'factor',
            token,
        } as FactorNode;
    }

    const ast: ProgramNode = {
        type: 'program',
        token: null,
        body: [],
    };

    let token = tokens[currentTokenPosition];
    let left: Node = getFactor(token);
    token = tokens[++currentTokenPosition];
    while (
        token &&
        (token.type === 'plus' ||
            token.type === 'minus' ||
            token.type === 'star' ||
            token.type === 'forward slash')
    ) {
        const right = getFactor(tokens[++currentTokenPosition]);
        const expression = {
            type: 'binary expression',
            token,
            body: [left, right],
        } as BinaryExpressionNode;
        left = expression;
        token = tokens[++currentTokenPosition];
    }
    ast.body.push(left);

    return ast;
}

export function printTree(node: Node, indent = '', isLast = true) {
    const marker = isLast ? '`---' : '|---';

    console.log(
        `${indent}${marker}${node.type}${
            node?.token?.value ? ' ' + node?.token?.value : ''
        }`
    );

    indent += isLast ? '    ' : '|   ';
    for (let i = 0; i < node.body?.length; i++) {
        const child = node.body[i];
        printTree(child, indent, i === node.body?.length - 1);
    }
}
