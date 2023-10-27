import { lex } from './lex.ts';
import { parse, printTree } from './parse.ts';

let showTokens = true;
let showAst = true;
let showTransform = true;
let showCode = true;
while (true) {
    const input = prompt('>');
    if (input) {
        switch (input) {
            case '#showTokens': {
                showTokens = !showTokens;
                break;
            }
            case '#showAst': {
                showAst = !showAst;
                break;
            }
            case '#showTransform': {
                showTransform = !showTransform;
                break;
            }
            case '#showCode': {
                showCode = !showCode;
                break;
            }
            default: {
                try {
                    // 1. Lexical Analysis -
                    const tokens = lex(input);
                    // if (showTokens) {
                    //     console.log(
                    //         'tokens: ',
                    //         JSON.stringify(tokens, null, 2)
                    //     );
                    // }
                    // 2. Syntactic Analysis
                    const ast = parse(tokens);
                    if (showAst) {
                        printTree(ast);
                    }
                    // 3. Transformation
                    // const trans = transform(ast);
                    // if (showTransform) {
                    //     console.log(
                    //         'transform: ',
                    //         JSON.stringify(trans, null, 2)
                    //     );
                    // }
                    // // 4. Code Generation
                    // const code = generate(trans);
                    // if (showCode) {
                    //     console.log('code: ', code);
                    // }
                } catch (e: unknown) {
                    console.error(e);
                }
            }
        }
    } else {
        console.error('No input.');
    }
}
