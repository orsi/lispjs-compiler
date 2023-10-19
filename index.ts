import { compiler } from './compiler.ts';

while (true) {
    const input = prompt('>');
    if (input) {
        try {
            const output = compiler(input);
            console.log(output);
        } catch (e: unknown) {
            console.error(e);
        }
    } else {
        console.error('No input.');
    }
}
