// Placeholder for assembly code generation.
// First goal later: emit assembly that returns an integer exit code.

/* the very first step is to realise how the code in our c file looks like when translated to asssembly and what each section means 

int main(void) {
    return 42;
}

.globl main
main:
    movl $42, %eax
    ret

Program
  FunctionDefinition name=main
    ReturnStatement
      ConstantExpression value=42

program node -> calls the function
function def node -> assembler dir + main label
return statement -> calls expression
const expression -> movl... ->
return statement -> receives the expre and ret 

*/


