#include <stdio.h>
#include <stdlib.h>

void show_bytes(unsigned char *start, int len)
{
    int i;
    for (i=len-1; i>=0; i--)
        printf(" %.2x", start[i]);
    printf("\n");
}

void two_s_complement()
{
    printf("\nIn generally, negative numbers are represented by the"
           " two's complement.\nThe leading bit is explained negative"
           " weight in the two's complement.\n\n");
}

void transform_integer()
{
    int x = -1;
    unsigned u_x = 4294967295;
    unsigned u = 2147483648;
    int s_u = -2147483648;

    /* The transform between signed and unsigned integer that keeps the same
     * bit, but the value maybe changed*/
    printf("\n--------------------------------------------\n");
    printf("Show the transform between signed and unsigned integer:\n\n");
    printf("The signed integer %d mapping to two's complement 0x%x\n", x, x);
    printf("The unsigned signed integer %u mapping to two's complement 0x%x\n", u_x, u_x);
    printf("The signed integer %d is converted to unsigned integer %u\n", x, x);
    printf("\n");
    printf("The unsigned integer %u mapping to two's complement 0x%x\n", u, u);
    printf("The signed integer %d mapping to two's complement 0x%x\n", s_u, s_u);
    printf("The unsigned integer %u is transformed to signed integer %d\n", u, u);
    printf("\nThe bits are the same when transforming between signed integer and "
           "unsigned integer,\n but how to explain these bits that is different,\n"
           "so the value maybe changed but bits are the same\n");

    // Extend the bits when transform between different size of integer
    short sx = -12345;
    unsigned short usx = sx;
    int x_32_bits = sx;
    unsigned ux = usx;

    printf("\n--------------------------------------------\n");
    printf("Show the extension when transform between different length of data types:\n\n");
    printf("Signed short %d", sx);
    show_bytes((unsigned char *)&sx, sizeof(short));
    printf("Unsigned short %u", usx);
    show_bytes((unsigned char *)&usx, sizeof(unsigned short));
    printf("Signed int %d", x_32_bits);
    show_bytes((unsigned char *)&x_32_bits, sizeof(int));
    printf("Unsigned int %u", ux);
    show_bytes((unsigned char *)&ux, sizeof(unsigned));
    printf("\nUnsigned integer is transformed to a longer data type which need add 0 at high bits.\n"
           "This is zero extension.\nTwo's complement is transformed to a longer data type which\n"
           "need add the highest bit at high bits. This is sign extension\n");
}

void truncate_integer()
{
    int x = 53191;
    short sx = (short)x;
    int y = sx;
    printf("\n--------------------------------------------\n");
    printf("A integer type x:%d is truncated when transformed to a short type sx:%d\n", x, sx);
    printf("A short type sx:%d is extended when transformed to a integer type y:%d\n", sx, y);
    printf("The binary of x(%d):", x);
    show_bytes((unsigned char *)&x, sizeof(int));
    printf("The binary of sx(%d):", sx);
    show_bytes((unsigned char *)&sx, sizeof(short));
    printf("The binary of y(%d):", sx);
    show_bytes((unsigned char *)&y, sizeof(int));
}

int main(void)
{
    two_s_complement();
    transform_integer();
    truncate_integer();
}
