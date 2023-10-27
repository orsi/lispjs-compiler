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
    name: 'eval test',
    fn() {
        let result = evaluate(parse(lex('1 + 2')));
        assertEquals(result, 3);

        result = evaluate(parse(lex('1 + 2 + 3')));
        assertEquals(result, 6);

        result = evaluate(parse(lex('1 + 2 * 3')));
        assertEquals(result, 7);

        result = evaluate(parse(lex('(1 + 2) * 3')));
        assertEquals(result, 9);

        result = evaluate(parse(lex('10 + 10 - (12 * 3) / 4')));
        assertEquals(result, 11);

        result = evaluate(parse(lex('1 + (2 * (5 - (6 / (1 - 2))))')));
        assertEquals(result, 23);

        result = evaluate(parse(lex('1 + (2 * (5 - (6 / (1 - 2)))) % 5')));
        assertEquals(result, 3);
    },
});
