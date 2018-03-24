void main(){

    print("How long should your array be?");
    print_nl();
    int n;
    n = read_int();
    print_nl();
    int a[n];

    int i;
    i=0
    while(i<n){
        a[i]=i;
        i=i+1;
    }
    print("This is your array: ");
    print_nl();

    int j;
    j=0;
    while(j<n){
        print(a[j]);
        print_nl();
    }
}