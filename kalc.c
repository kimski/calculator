#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<ctype.h>

/*
  A simple calculator for single digit numbers with +*-/ operations.
  It has a read-eval-print-loop which parses infix notation to postfix notation
  and then evaluates the postfix using a stack machine.
  In addition to the interactive repl mode, it can also be used in pipe mode,
  suppressing printing of banner and prompt.
  For example
    echo "(7+9*2)*(8-1)/(1+3-2)" | ./kalc  
*/

int quiet; // stores whether we can print to console or not
double stack[256]; // allow upto 256 numbers on our stack
int top; // tells us what offset the top of the stack is
 
void push(double d){ // stores a number onto the top of the stack
  if(top>sizeof(stack)){ // test if we have too many numbers on the stack
    printf("stack overflow\n");
    exit(1);
  }
  stack[top++]=d; // stores the double d at top of stack, and increments the offset 'top'
}

double pop(){ // removes a number from the top of the stack
  if(top==0){ // test if there are any numbers on the stack
    printf("stack underflow\n");
    exit(1);
  }
  return stack[--top]; // returns the value at the top of the stack 
}

double peek(){ // gets the number at the top of the stack
  double d;
  push(d=pop());
  return d;
}

void swap(){ // swaps the 2 top numbers on the stack
  double x=pop(),y=pop();
  push(x),push(y);
}
 
int precedence(char c){ // gets the precedence of the operator. e.g. * is higher then +
  switch(c){
    case'+':
    case'-':return 1;
    case'*':
    case'/':return 2; // * and / are higher precedence than + or -
    default: return 0;
  }
}

// See https://en.wikipedia.org/wiki/Reverse_Polish_notation
double eval(char*s,int n){
  int i;
  for(i=0;i<n;i++){
    char c=s[i];
    if(isdigit(c))
      push(c-'0');
    else
      switch(c){
        case'+':push(pop()+pop());break;
        case'*':push(pop()*pop());break;
        case'-':swap();push(pop()-pop());break;
        case'/':swap();push(pop()/pop());break;
        default:break;
      }
  }
  return pop();
}

int isOp(char c){ // tests if the character is a recognized binary operator
  return c=='+'||c=='-'||c=='/'||c=='*';
}

// https://en.wikipedia.org/wiki/Shunting-yard_algorithm
ssize_t infixToPostfix(char*s,ssize_t n,char*postfix){
  char c;
  ssize_t i,j=0;
  for(i=0;i<n;i++){
    c=s[i];
    if(isspace(c))
      ;
    else if(c=='(')
      push(c);
    else if(isdigit(c)){
      if(i&&isdigit(s[i-1])){
        printf("Error: juxtaposed digits ('%c') found at %ld\n",c,i);
        top=0;
        return 0;
      }
      postfix[j++]=c;
    }
    else if(c==')'){
      while(top&&peek()!='(')
        postfix[j++]=pop();
      if(top==0){
        printf("Error: unmatched ')'\n");
        return 0;
      }
      pop(); // pops the '('
      // todo: check next char here, must be op
    }
    else if(isOp(c)){
      if(i&&isOp(s[i-1])){
        printf("Error: juxtaposed operation ('%c') found at %ld\n",c,i);
        top=0;
        return 0;
      }
      while(top&&precedence(peek())>=precedence(c))
        postfix[j++]=pop();
      push(c);
    }
    else{
      printf("Error: bad character '%c' at position %ld.\nCheck that your input is ascii rather than unicode.\n",c,i);
      top=0;
      return 0;
    }
  }
  while(top){
    c=pop();
    if(c=='('){
      printf("Error: unmatched '('\n");
      top=0;
      return 0;
    }
    postfix[j++]=c;
  }
  return j;
}

void printBanner(){ // prints the startup banner to the console
  if(!quiet) // if we have a console
    printf("Kim's calculator program. Copyright 2015,2016 Kimberley Skelton\nPress ctrlD to exit\n");
}

void printPrompt(){ // prints the prompt to the console
  if(!quiet){ // if we have a console
    printf("kalc>");
    fflush(stdout); // printf uses buffering, so force a flush of buffers here so it appears on the console
  }
}

void repl(){ // read eval print loop
  while(1){ // keep looping until stdin closed
    char buffer[256],  // store console input
         postfix[256]; // store the postfix representation of the console input
    ssize_t j, // number of chars in postfix
            n;// number of chars in buffer
    printPrompt(); // print the printPrompt
    n=read(STDIN_FILENO,buffer,sizeof(buffer)); // read chars from console into buffer
    if(n==-1){ // check if read returned an error
      perror("read"); // print the error
      exit(1); // and exit the program
    }
    if(n==0) // no chars read means stdin closed
      break; // break out of while loop
    n--; // drop the \n at the end of the buffer
    if(n){ // maybe they just hit return key
      j=infixToPostfix(buffer,n,postfix); // get the postfix representation of the input
      if(j){
        if(!quiet) // if we have a console
          printf("Postfix %.*s\n",(int)j,postfix); // print the postfix representation
        printf("%f\n",eval(postfix,j)); // evaluates the postfix representation and print the result
      }
    }
  }
  if(!quiet) // if we have a console
    printf("\n"); // print a new line
}

int main(int argc,char*argv[]){ // entry point to the program
  quiet=!isatty(STDIN_FILENO); // check whether there is a console attached
  printBanner(); // print copyright etc.
  repl(); // read eval print loop
  return EXIT_SUCCESS;
}