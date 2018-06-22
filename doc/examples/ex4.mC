void foo(){}

void bar(float useless){}

void foo2(){}

void helloWorld(){
    print("GoodBye cruel world");
    print_nl();
}

int calcAbsolute(int number){
    return -number;
}

string literallyUseless(int noUse){return "useless";}

void main(){
    print("Please enter a negative number: ");
    int n;
    n = read_int();
    print_nl();


    if(n >= 0) {}
    else {
       n = calcAbsolute(n);
    }
    print("Absolute of your Number: ");
    print_nl();
    print_int(n);
}
