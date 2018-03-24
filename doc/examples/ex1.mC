
int checkEvenOdd(int n){
    if((n/2)*2 == n){
        print("Even");
    }
    else{
        print("Odd");
    }
    print_nl();
    return 0;
}

bool checkPrim(int n){
    int i;
    i=0;
    int p;
    p=0;
    while(i<=n){
        if((num - (num/i)*i)==0){
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

    checkEvenOdd(n);
    prim=checkPrim(n);
    print_nl();
    print("Primenumber ?");
    print_nl();
    print(prim);
}