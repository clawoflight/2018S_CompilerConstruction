
void checkEvenOdd(int n, bool m){
    if((n/2)*2 == n){
        print("Even");
    }
    else{
        print("Odd");
    }
    print_nl();
    return ;
}

bool checkPrim(int n){
    int i;
    i = 2;
    int mod;
    while (i < n/2)
    {
        mod  = n - (n/i)*i;
        if(mod == 0){
            print("prime");
            return false;
        }
        i = i +1;
    }
    print("prime");
    return true;
}

void main(){
    print("Please enter a number: ");
    int n;
    n = read_int();
    print_nl();

    checkEvenOdd(n,false);
    bool prim;
    prim=checkPrim(n);
    print_nl();
    print("Primenumber ?");
    print_nl();
    if (prim == true) {
        print("true");
    } else {
        print("false");
    }
}
