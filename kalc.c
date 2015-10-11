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

int quiet;
double stack[256];
int top;
 
void push(double c){
  if(top>sizeof(stack)){
    printf("stack overflow\n");
    exit(1);
  }
  stack[top++]=c;
}

double pop(){
  if(top==0){
    printf("stack underflow\n");
    exit(1);
  }
  return stack[--top];
}

double peek(){
  double d;
  push(d=pop());
  return d;
}

void swap(){
  double x=pop(),y=pop();
  push(x),push(y);
}
 
int precedence(char c){
  switch(c){
    case'(':return 0;
    case'+':
    case'-':return 1;
    case'*':
    case'/':return 2;
  }
  return 0;
}

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

int infixToPostfix(char*s,int n,char*postfix){
  int i,j=0;
  for(i=0;i<n;i++){
    char c=s[i];
    if(isspace(c))
      ;
    else if(c=='(')
      push(c);
    else if(isdigit(c))
      postfix[j++]=c;
    else if(c==')'){
      while(top&&peek()!='(')
        postfix[j++]=pop();
      pop();
    }
    else{
      while(top&&precedence(peek())>=precedence(c))
        postfix[j++]=pop();
      push(c);
    }
  }
  while(top)
    postfix[j++]=pop();
  return j;
}

void printBanner(){
  if(!quiet)
    printf("Kim's calculator program. Copyright 2015 Kimberley Skelton\n");
}

void prompt(){
  if(!quiet){
    printf("kalc>");
    fflush(stdout);
  }
}

void repl(){ 
  while(1){
    char buffer[256],postfix[256];
    int i,j,n;
    prompt();
    n=read(0,buffer,sizeof(buffer));
    if(n==0)
      break;
    if(n<1){
      perror("read");
      exit(1);
    }
    j=infixToPostfix(buffer,n,postfix);
    if(isatty(0))
      printf("Postfix %.*s\n",j,postfix);
    printf("%f\n",eval(postfix,j));
  }
  if(!quiet)
    printf("\n");
}

int main(int argc,char*argv[]){
  quiet=!isatty(0);
  printBanner();
  repl();
  return 0;
}
