import { assertEquals } from 'https://deno.land/std@0.204.0/assert/mod.ts';
import { lex } from './lex.ts';
import { BinaryExpressionNode, parse } from './parse.ts';

Deno.test('parse test', () => {
    const expression = '1 * 2 + 3';
    const tree = parse(lex(expression));
    assertEquals(tree.body[0].type, 'binary expression');
    assertEquals((tree.body[0] as BinaryExpressionNode).operator, '+');
});
