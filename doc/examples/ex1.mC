
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
    i=1;
    int p;
    p=0;
    while(i<=n){
        if((n - (n/i)*i)==0){
            p=p+1;
        }
        i=i+1;
    }
    if(p==0){
        print("prime");
        return true;
    } else{
        print("not prime");
        return false;
    }
}

void main(){
    print("Please enter a number: ");
    print_nl();
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
        print_int(1);
    } else {
        print_int(0);
    }
}
