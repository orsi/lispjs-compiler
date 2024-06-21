import {
    AssignmentExpressionNode,
    BinaryExpressionNode,
    IdentifierNode,
    Node,
    NumberLiteralNode,
    ProgramNode,
    StatementNode,
    StringLiteralNode,
} from './parse.ts';

const identifiers: Record<string, unknown> = {};

export function evaluate(node: ProgramNode): any[] {
    const results: any[] = [];
    for (const body of node.body) {
        results.push(compute(body));
    }
    return results;
}

function compute(node: Node): any {
    if (node.type === 'statement') {
        return compute((node as StatementNode).statement);
    } else if (node.type === 'binary expression') {
        const expression = node as BinaryExpressionNode;
        const left = compute(expression.left);
        const right = compute(expression.right);

        if (expression.operator === '+') {
            return left + right;
        } else if (expression.operator === '-') {
            return left - right;
        } else if (expression.operator === '*') {
            return left * right;
        } else if (expression.operator === '/') {
            return left / right;
        } else if (expression.operator === '%') {
            return left % right;
        } else {
            throw new Error(
                `Unexpected binary operator ${expression.operator}`
            );
        }
    } else if (node.type === 'assignment expression') {
        const expression = node as AssignmentExpressionNode;
        const left = expression.left;
        const right = compute(expression.right);

        return (identifiers[left.name] = right);
    } else if (node.type === 'identifier') {
        return identifiers[(node as IdentifierNode).name];
    } else if (node.type === 'number literal') {
        return (node as NumberLiteralNode).value;
    } else if (node.type === 'string literal') {
        return (node as StringLiteralNode).value;
    } else if (node.type === 'unexpected') {
        return;
    } else if (node.type === 'end of file') {
        return;
    } else {
        throw new Error('whomp');
    }
}
