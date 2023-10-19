export function traverse(ast: any, visitors: any) {
    function walkNode(node: any, parent: any) {
        const method = visitors[node.type];
        if (method) {
            method(node, parent);
        }

        if (node.type === 'Program') {
            walkNodes(node.body, node);
        } else if (node.type === 'CallExpression') {
            walkNodes(node.params, node);
        }
    }
    function walkNodes(nodes: any[], parent: any) {
        nodes.forEach((node) => walkNode(node, parent));
    }

    walkNode(ast, null);
}

export function transform(ast: any) {
    const transform: any = {
        type: 'Program',
        body: [],
    };

    let position = transform.body;
    traverse(ast, {
        NumberLiteral(node: any) {
            position.push({
                type: 'NumberLiteral',
                value: node.value,
            });
        },
        CallExpression(node: any, parent: any) {
            let expression: any = {
                type: 'CallExpression',
                callee: {
                    type: 'Identifier',
                    name: node.name,
                },
                arguments: [],
            };
            const previousPosition = position;
            position = expression.arguments;
            if (parent.type !== 'CallExpression') {
                expression = {
                    type: 'ExpressionStatement',
                    expression,
                };
            }
            previousPosition.push(expression);
        },
    });

    return transform;
}
