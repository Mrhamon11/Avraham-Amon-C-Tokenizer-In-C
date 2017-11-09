void func(int x, ...);

int main() {
    float x = 234.234f;
    if(x >= 1){
        printf("testing");
    }
    else if(x < 1000){
        x = 234.3e+23;
        long y = 3454u;
    }
    else
        printf("doing something else");

    char str[] = "hello, world";

    struct Object{
        double field;
    };

    struct Object o;
    o.field = 334.34f;

    typedef struct Object Thing;

    if(0 == 0 && 1 > 0)
        x << 2;

    //single line
    /*multi
      line
            */

    return 0;
}