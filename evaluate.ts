import { BinaryExpressionNode, Node, NumberLiteralNode } from './parse.ts';

export function evaluate(node: Node): any {
    if (node.type === 'number') {
        return (node as NumberLiteralNode).value;
    }

    if (node.type === 'binary expression') {
        const expression = node as BinaryExpressionNode;
        const left = evaluate(expression.left);
        const right = evaluate(expression.right);

        if (expression.operator === '+') return left + right;
        else if (expression.operator === '-') return left - right;
        else if (expression.operator === '*') return left * right;
        else if (expression.operator === '/') return left / right;
        else
            throw new Error(
                `Unexpected binary operator ${expression.operator}`
            );
    }
}
