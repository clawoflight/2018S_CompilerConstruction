void main(){

    print("Which number do you want to fill into your array?");
    print_nl();
    int n;
    n = read_int();
    print_nl();
    int[3] a;

    int i;
    i =0;
    while(i<=2){
        a[i]=n;
        i=i+1;
    }
    print("This is your array: ");
    print_nl();

    int j;
    j=0;
    while(j<3){
        print(a[j]);
        print_nl();
    }
}