void lucas_number (int x, int y, int z, int num){
 while(num >= x)
 {
   print_int(x);
   print(", ");
   z=x+y;
   x=y;
   y=z;
 }

}

void main()
{
    int x;
    int y;
    int z;
    int num;
    print("Enter the limit of the Lucas number:");
    print_nl();
    x=2;
    y=1;
    num= read_int();
    print("The Lucas numbers are: ");
    lucas_number(x, y, z, num);
    print_nl();


}


