export function generate(node: any): any {
    if (node.type === 'NumberLiteral') {
        return node.value;
    }
    if (node.type === 'Identifier') {
        return node.name;
    }
    if (node.type === 'CallExpression') {
        return `${generate(node.callee)}(${node.arguments.map(generate).join(', ')})`;
    }
    if (node.type === 'ExpressionStatement') {
        return `${generate(node.expression)}`;
    }
    if (node.type === 'Program') {
        return node.body.map(generate).join('\n');
    }
}