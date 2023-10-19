import { generate } from './generate.ts';
import { lex } from './lex.ts';
import { parse } from './parse.ts';
import { transform } from './transform.ts';
export function compiler(input: string) {
    // 1. Lexical Analysis -
    const tokens = lex(input);
    // 2. Syntactic Analysis
    const ast = parse(tokens);
    // 3. Transformation
    const newAst = transform(ast);
    // 4. Code Generation
    const code = generate(newAst);

    return code;
}
