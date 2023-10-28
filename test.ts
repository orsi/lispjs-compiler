import { assertEquals } from 'https://deno.land/std@0.204.0/assert/mod.ts';
import { evaluate } from './evaluate.ts';
import { lex } from './lex.ts';
import { BinaryExpressionNode, parse } from './parse.ts';

Deno.test({
    name: 'parse test',
    fn() {
        let expression = '1 * 2 + 3';
        let tree = parse(lex(expression));
        assertEquals(tree.body[0].type, 'binary expression');
        assertEquals((tree.body[0] as BinaryExpressionNode).operator, '+');

        expression = '(1 + 2) * 3';
        tree = parse(lex(expression));
        assertEquals(tree.body[0].type, 'binary expression');
        assertEquals((tree.body[0] as BinaryExpressionNode).operator, '*');
    },
});

Deno.test({
    name: 'binary expression evaluation test',
    fn() {
        let result = evaluate(parse(lex('1 + 2')))[0];
        assertEquals(result, 3);

        result = evaluate(parse(lex('1 + 2 + 3')))[0];
        assertEquals(result, 6);

        result = evaluate(parse(lex('1 + 2 * 3')))[0];
        assertEquals(result, 7);

        result = evaluate(parse(lex('(1 + 2) * 3')))[0];
        assertEquals(result, 9);

        result = evaluate(parse(lex('10 + 10 - (12 * 3) / 4')))[0];
        assertEquals(result, 11);

        result = evaluate(parse(lex('1 + (2 * (5 - (6 / (1 - 2))))')))[0];
        assertEquals(result, 23);

        result = evaluate(parse(lex('1 + (2 * (5 - (6 / (1 - 2)))) % 5')))[0];
        assertEquals(result, 3);
    },
});

Deno.test({
    name: 'multi-line assignment expressions evaluation test',
    fn() {
        const input = `
hi: "test";
num: 15;

hope: 10 + num;
`;
        const tokens = lex(input);
        assertEquals(tokens.length, 15);
        assertEquals(tokens[8].type, 'identifier');
        const tree = parse(tokens);
        assertEquals(tree.body.length, 4);
        assertEquals(
            (tree.body?.[2] as BinaryExpressionNode)?.left?.type,
            'assignment expression'
        );
        const results = evaluate(tree);
        assertEquals(results.length, 4);
        assertEquals(results[0], '"test"');
        assertEquals(results[1], 15);
        assertEquals(results[2], 25);
        assertEquals(results[3], undefined);
    },
});
