void foo(){}

void bar(float useless){}

void foo2(){}

int helloWorld(){
    print("GoodBye cruel world");
    print_nl();
}

int calcAbsolute(int number){
    return -number;
}

string literallyUseless(int noUse){}

void main(){
    print("Please enter a negative number: ");
    print_nl();
    int n;
    n = read_int();
    print_nl();


    if(n >= 0) {}
    else {
       n = calcAbsolute(n);
    }
    print("Absolute of your Number: ");
    print_nl();
    print(n);
}