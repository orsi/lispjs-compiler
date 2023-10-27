import {
    BinaryExpressionNode,
    Node,
    NumberLiteralNode,
    ProgramNode,
    StatementNode,
    StringLiteralNode,
} from './parse.ts';

export function evaluate(node: Node): any {
    if (node.type === 'program') {
        for (const body of (node as ProgramNode).body) {
            return evaluate(body);
        }
    } else if (node.type === 'end of file') {
        return;
    } else if (node.type === 'binary expression') {
        const expression = node as BinaryExpressionNode;
        const left = evaluate(expression.left);
        const right = evaluate(expression.right);

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
    } else if (node.type === 'number') {
        return (node as NumberLiteralNode).value;
    } else if (node.type === 'statement') {
        return evaluate((node as StatementNode).statement);
    } else if (node.type === 'string') {
        return (node as StringLiteralNode).value;
    } else if (node.type === 'unknown') {
        return;
    } else {
        throw new Error('whomp');
    }
}
